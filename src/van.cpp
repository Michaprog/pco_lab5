#include "van.h"

BikingInterface* Van::binkingInterface = nullptr;
std::array<BikeStation*, NB_SITES_TOTAL> Van::stations{};

Van::Van(unsigned int _id)
    : id(_id),
    currentSite(DEPOT_ID)
{}

void Van::run() {
    while (true /*TODO: clean stop*/) {
        loadAtDepot();
        for (unsigned int s = 0; s < NBSITES; ++s) {
            driveTo(s);
            balanceSite(s);
        }
        returnToDepot();
    }
    log("Van s'arrête proprement");
}

void Van::setInterface(BikingInterface* _binkingInterface){
    binkingInterface = _binkingInterface;
}

void Van::setStations(const std::array<BikeStation*, NB_SITES_TOTAL>& _stations) {
    stations = _stations;
}

void Van::log(const QString& msg) const {
    if (binkingInterface) {
        binkingInterface->consoleAppendText(0, msg);
    }
}

void Van::driveTo(unsigned int _dest) {
    if (currentSite == _dest)
        return;

    unsigned int travelTime = randomTravelTimeMs();
    if (binkingInterface) {
        binkingInterface->vanTravel(currentSite, _dest, travelTime);
    }

    currentSite = _dest;
}

void Van::loadAtDepot() {
    driveTo(DEPOT_ID);

    // TODO: implement this method. If possible, load at least 2 bikes

    cargo.clear();

    size_t depotCount = stations[DEPOT_ID]->nbBikes();
    size_t toLoad = std::min<size_t>(2,depotCount);
    if(toLoad == 0) return;

    auto bikes = stations[DEPOT_ID]->getBikes(toLoad);
    for (Bike* b : bikes){
        if(!b) continue;
        cargo.push_back(b);
    }

    log(QString("Van loaded %1 bikes at depot").arg(cargo.size()));

    if (binkingInterface) {
        binkingInterface->setBikes(DEPOT_ID, stations[DEPOT_ID]->nbBikes());
    }
}


void Van::balanceSite(unsigned int _site)
{
    // TODO: implement this method

    if (_site == DEPOT_ID) return;

    BikeStation* st = stations[_site];
    size_t target = BORNES - 2;

    {
        size_t Vi = st->nbBikes();
        if (Vi > target && cargo.size() < VAN_CAPACITY) {
            size_t surplus = Vi - target;
            size_t freeSlots = VAN_CAPACITY - cargo.size();
            size_t c = std::min(surplus, freeSlots);
            if (c > 0) {
                auto taken = st->getBikes(c);
                for (Bike* b : taken) {
                    cargo.push_back(b);
                }
                log(QString("Van took %1 bikes from site %2")
                            .arg(taken.size()).arg(_site));
            }
        }
    }

    {
        size_t Vi = st->nbBikes();
        if (Vi < target && !cargo.empty()) {
            size_t missing = target - Vi;
            size_t c = std::min(missing, cargo.size());
            if (c > 0) {
                size_t deposited = 0;
                std::array<bool, Bike::nbBikeTypes> hasType{};
                for (size_t t = 0; t < Bike::nbBikeTypes; ++t) {
                    hasType[t] = (st->countBikesOfType(t) > 0);
                }

                std::vector<Bike*> toDrop;
                toDrop.reserve(c);

                for (size_t t = 0; t < Bike::nbBikeTypes && deposited < c; ++t) {
                    if (hasType[t]) continue;
                    Bike* b = takeBikeFromCargo(t);
                    if (b) {
                        toDrop.push_back(b);
                        ++deposited;
                    }
                }

                while (deposited < c && !cargo.empty()) {
                    Bike* b = cargo.back();
                    cargo.pop_back();
                    toDrop.push_back(b);
                    ++deposited;
                }

                if (!toDrop.empty()) {
                    auto notInserted = st->addBikes(toDrop);
                    for (Bike* b : notInserted) {
                        cargo.push_back(b);
                    }
                    log(QString("Van deposited %1 bikes at site %2")
                                .arg(toDrop.size() - notInserted.size())
                                .arg(_site));
                }
            }
        }
    }

    if (binkingInterface) {
        binkingInterface->setBikes(DEPOT_ID, stations[DEPOT_ID]->nbBikes()); // Keep somewhere for GUI
    }
}

void Van::returnToDepot() {
    driveTo(DEPOT_ID);

    if(!cargo.empty()){
        auto notInserted = stations[DEPOT_ID]->addBikes(cargo);
        cargo.clear();
        for(Bike* b: notInserted){
            cargo.push_back(b);
        }
    }

    // TODO: implement this method. If the van carries bikes, then leave them

    if (binkingInterface) {
        binkingInterface->setBikes(DEPOT_ID, stations[DEPOT_ID]->nbBikes());
    }
}

Bike* Van::takeBikeFromCargo(size_t type) {
    for (size_t i = 0; i < cargo.size(); ++i) {
        if (cargo[i]->bikeType == type) {
            Bike* bike = cargo[i];
            cargo[i] = cargo.back();
            cargo.pop_back();
            return bike;
        }
    }
    return nullptr;
}

