//  /$$$$$$$   /$$$$$$   /$$$$$$         /$$$$$$   /$$$$$$   /$$$$$$  /$$$$$$$
// | $$__  $$ /$$__  $$ /$$__  $$       /$$__  $$ /$$$_  $$ /$$__  $$| $$____/
// | $$  \ $$| $$  \__/| $$  \ $$      |__/  \ $$| $$$$\ $$|__/  \ $$| $$
// | $$$$$$$/| $$      | $$  | $$        /$$$$$$/| $$ $$ $$  /$$$$$$/| $$$$$$$
// | $$____/ | $$      | $$  | $$       /$$____/ | $$\ $$$$ /$$____/ |_____  $$
// | $$      | $$    $$| $$  | $$      | $$      | $$ \ $$$| $$       /$$  \ $$
// | $$      |  $$$$$$/|  $$$$$$/      | $$$$$$$$|  $$$$$$/| $$$$$$$$|  $$$$$$/
// |__/       \______/  \______/       |________/ \______/ |________/ \______/
//
//   Tests pour le labo vélos : BikeStation + Van
//

#include <gtest/gtest.h>
#include <atomic>
#include <vector>
#include <array>

#include <pcosynchro/pcothread.h>

#include "bikestation.h"
#include "bike.h"
#include "van.h"
#include "config.h"

// ========================= Helpers ========================= //

static Bike* makeBike(size_t type)
{
    auto* b = new Bike;
    b->bikeType = type % Bike::nbBikeTypes;
    return b;
}

// Petit sleep pour laisser les autres threads s’exécuter
static void small_sleep(int us = 200)
{
    PcoThread::usleep(us);
}

// ========================= Tests BikeStation (base) ========================= //

// Insertion / retrait simple, mono-thread.
TEST(BikeStationBasic, PutGetSingleThread)
{
BikeStation st(10);
EXPECT_EQ(st.nbBikes(), 0u) << "La station doit être vide au départ";

Bike* b0 = makeBike(0);
st.putBike(b0);
EXPECT_EQ(st.nbBikes(), 1u);
EXPECT_EQ(st.countBikesOfType(0), 1u);

Bike* b1 = st.getBike(0);
EXPECT_EQ(b1, b0) << "getBike doit rendre le même vélo";
EXPECT_EQ(st.nbBikes(), 0u) << "Après retrait, la station doit être vide";

delete b1;
}

// Ajout multiple, puis retrait partiel avec getBikes().
TEST(BikeStationBasic, AddAndGetMultiple)
{
BikeStation st(10);

std::vector<Bike*> bikes;
for (size_t i = 0; i < 5; ++i) {
bikes.push_back(makeBike(i % Bike::nbBikeTypes));
}

auto leftover = st.addBikes(bikes);
EXPECT_TRUE(leftover.empty()) << "Tous les vélos doivent être insérés";
EXPECT_EQ(st.nbBikes(), 5u);

auto taken = st.getBikes(3);
EXPECT_EQ(taken.size(), 3u);
EXPECT_EQ(st.nbBikes(), 2u);

for (auto* b : taken) delete b;
for (auto* b : leftover) delete b;
}

// La capacité de la station doit être respectée.
TEST(BikeStationBasic, CapacityRespected)
{
const int CAP = 3;
BikeStation st(CAP);

std::vector<Bike*> bikes;
for (int i = 0; i < CAP + 2; ++i) {
bikes.push_back(makeBike(0));
}

auto leftover = st.addBikes(bikes);

EXPECT_EQ(st.nbBikes(), static_cast<size_t>(CAP))
<< "La station ne doit pas dépasser sa capacité";
EXPECT_EQ(leftover.size(), 2u)
<< "Les vélos en trop doivent être retournés";

for (auto* b : leftover) delete b;
}

// ending() doit réveiller un thread bloqué sur getBike().
TEST(BikeStationBasic, EndingUnblocksGetBike)
{
BikeStation st(1);

Bike* result = reinterpret_cast<Bike*>(0x1); // sentinelle
std::atomic<bool> started{false};

PcoThread t([&]{
    started.store(true);
    result = st.getBike(0); // se bloque, car aucun vélo
});

// On laisse le temps au thread de se bloquer dans getBike().
while (!started.load()) {
small_sleep(50);
}
small_sleep(200);

st.ending();
t.join();

EXPECT_EQ(result, nullptr) << "Après ending(), getBike doit retourner nullptr";
}

// ending() doit également réveiller un thread bloqué sur putBike() (station pleine).
TEST(BikeStationBasic, EndingUnblocksPutBike)
{
BikeStation st(1);

// On remplit la station
Bike* first = makeBike(0);
st.putBike(first);
EXPECT_EQ(st.nbBikes(), 1u);

Bike* toInsert = makeBike(0);
std::atomic<bool> started{false};
std::atomic<bool> finished{false};

PcoThread t([&]{
    started.store(true);
    st.putBike(toInsert);  // doit se bloquer car station pleine
    finished.store(true);  // sera mis à true quand le thread sort de putBike()
});

while (!started.load()) {
small_sleep(50);
}
small_sleep(200);

st.ending();    // doit réveiller le thread
t.join();

EXPECT_TRUE(finished.load()) << "Le thread bloqué sur putBike doit se réveiller";
// On garde first pour nettoyer
auto remaining = st.getBikes(st.nbBikes());
for (auto* b : remaining) delete b;
delete first;
// toInsert n'a jamais été inséré, mais on n'a plus le pointeur après putBike.
// C’est un léger leak acceptable pour un test unitaire.
}


// ========================= Tests BikeStation (concurrence) ========================= //

// Producteur / consommateur sur un seul type de vélo.
TEST(BikeStationConcurrent, ProducerConsumerSingleType)
{
BikeStation st(5);
const int N = 30;

std::atomic<int> produced{0};
std::atomic<int> consumed{0};

PcoThread producer([&]{
    for (int i = 0; i < N; ++i) {
        Bike* b = makeBike(0);     // toujours type 0
        st.putBike(b);
        produced.fetch_add(1);
        small_sleep(200);
    }
});

PcoThread consumer([&]{
    for (int i = 0; i < N; ++i) {
        Bike* b = st.getBike(0);   // bloque si aucun vélo type 0
        ASSERT_NE(b, nullptr) << "getBike ne doit pas retourner nullptr sans ending()";
        consumed.fetch_add(1);
        delete b;
        small_sleep(100);
    }
});

producer.join();
consumer.join();

EXPECT_EQ(produced.load(), N);
EXPECT_EQ(consumed.load(), N);
EXPECT_EQ(st.nbBikes(), 0u) << "Tous les vélos produits doivent être consommés";
}


// ========================= Tests Van (logique) ========================= //

// Test de loadAtDepot() : la camionnette charge min(2, D) vélos.
TEST(VanLogic, LoadAtDepotLoadsAtMostTwo)
{
// Création du tableau de stations "réel" de la config
std::array<BikeStation*, NB_SITES_TOTAL> stations{};
for (size_t s = 0; s < NBSITES; ++s) {
stations[s] = new BikeStation(BORNES);
}
// Dépôt : beaucoup de place pour les tests
stations[DEPOT_ID] = new BikeStation(100);

// On met 5 vélos au dépôt
const int D = 5;
{
std::vector<Bike*> depotBikes;
for (int k = 0; k < D; ++k) {
depotBikes.push_back(makeBike(k % Bike::nbBikeTypes));
}
auto leftover = stations[DEPOT_ID]->addBikes(depotBikes);
ASSERT_TRUE(leftover.empty());
}
EXPECT_EQ(stations[DEPOT_ID]->nbBikes(), static_cast<size_t>(D));

// On associe les stations à la camionnette
Van van(0);
Van::setStations(stations);

// Appel de loadAtDepot()
van.loadAtDepot();

// La fonction charge min(2, D) vélos via getBikes(toLoad)
size_t remainingDepot = stations[DEPOT_ID]->nbBikes();
EXPECT_EQ(remainingDepot, static_cast<size_t>(D - std::min(2, D)))
<< "Après loadAtDepot(), il doit rester D - min(2, D) vélos au dépôt";

// Nettoyage
for (size_t i = 0; i < NB_SITES_TOTAL; ++i) {
if (!stations[i]) continue;
auto rem = stations[i]->getBikes(stations[i]->nbBikes());
for (Bike* b : rem) delete b;
delete stations[i];
}
}

// Test de balanceSite() sur un site en surplus puis un site en déficit.
// - site0 : plein (BORNES vélos) -> surplus de 2 par rapport à BORNES-2
// - site1 : en déficit de 2 vélos par rapport à BORNES-2
TEST(VanLogic, BalanceSiteSurplusThenDeficit)
{
std::array<BikeStation*, NB_SITES_TOTAL> stations{};
for (size_t s = 0; s < NBSITES; ++s) {
stations[s] = new BikeStation(BORNES);
}
stations[DEPOT_ID] = new BikeStation(100);

const size_t target = BORNES - 2;

// site 0 : plein (BORNES vélos) => surplus = 2
{
std::vector<Bike*> bikes;
for (size_t k = 0; k < BORNES; ++k) {
bikes.push_back(makeBike(k % Bike::nbBikeTypes));
}
auto leftover = stations[0]->addBikes(bikes);
ASSERT_TRUE(leftover.empty());
EXPECT_EQ(stations[0]->nbBikes(), static_cast<size_t>(BORNES));
}

// site 1 : déficit de 2 vélos par rapport à target
size_t initialSite1 = (target >= 2) ? target - 2 : 0;
{
std::vector<Bike*> bikes;
for (size_t k = 0; k < initialSite1; ++k) {
bikes.push_back(makeBike(k % Bike::nbBikeTypes));
}
auto leftover = stations[1]->addBikes(bikes);
ASSERT_TRUE(leftover.empty());
EXPECT_EQ(stations[1]->nbBikes(), initialSite1);
}

Van van(0);
Van::setStations(stations);

// Rééquilibrage du site 0 (en surplus)
van.balanceSite(0);
size_t after0 = stations[0]->nbBikes();
EXPECT_EQ(after0, target)
<< "Le site 0 doit être ramené à BORNES-2 vélos";

// Rééquilibrage du site 1 (en déficit)
van.balanceSite(1);
size_t after1 = stations[1]->nbBikes();
EXPECT_EQ(after1, target)
<< "Le site 1 doit être complété à BORNES-2 vélos";

// Nettoyage
for (size_t i = 0; i < NB_SITES_TOTAL; ++i) {
if (!stations[i]) continue;
auto rem = stations[i]->getBikes(stations[i]->nbBikes());
for (Bike* b : rem) delete b;
delete stations[i];
}
}