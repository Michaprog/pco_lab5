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

    if (binkingInterface) {
        binkingInterface->setBikes(DEPOT_ID, stations[DEPOT_ID]->nbBikes());
    }
}


void Van::balanceSite(unsigned int _site)
{
    // TODO: implement this method
    if (binkingInterface) {
        binkingInterface->setBikes(DEPOT_ID, stations[DEPOT_ID]->nbBikes()); // Keep somewhere for GUI
    }
}

void Van::returnToDepot() {
    driveTo(DEPOT_ID);

    size_t cargoCount = cargo.size();

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

