#include "bikestation.h"

BikeStation::BikeStation(int _capacity) : capacity(_capacity) {}

BikeStation::~BikeStation() {
    ending();
}

void BikeStation::putBike(Bike* _bike){
    // TODO: implement this method
}

Bike* BikeStation::getBike(size_t _bikeType) {
    // TODO: implement this method
    return nullptr;
}

std::vector<Bike*> BikeStation::addBikes(std::vector<Bike*> _bikesToAdd) {
    std::vector<Bike*> result; // Can be removed, it's just to avoid a compiler warning
    // TODO: implement this method
    return result;
}

std::vector<Bike*> BikeStation::getBikes(size_t _nbBikes) {
    std::vector<Bike*> result; // Can be removed, it's just to avoid a compiler warning
    // TODO: implement this method
    return result;
}

size_t BikeStation::countBikesOfType(size_t type) const {
    // TODO: implement this method
    return 0;
}

size_t BikeStation::nbBikes() {
    // TODO: implement this method
    return 0;
}

size_t BikeStation::nbSlots() {
    return capacity;
}

void BikeStation::ending() {
   // TODO: implement this method
}
