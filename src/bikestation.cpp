#include "bikestation.h"

BikeStation::BikeStation(int _capacity) : capacity(_capacity), currentCount(0), shouldEnd(false) {}

BikeStation::~BikeStation() {
    ending();
}

void BikeStation::putBike(Bike* _bike){
    // TODO: implement this method
    if (!_bike) return;

    mutex.lock();
    while(!shouldEnd && currentCount >= capacity){
        notFull.wait(&mutex);
    }

    if (shouldEnd){
        mutex.unlock();
        return;
    }

    size_t t = _bike->bikeType;
    if (t >= Bike::nbBikeTypes){
        t = 0;
    }

    queues[t].push_back(_bike);
    ++currentCount;

    notEmptyByType[t].notify_one(); // reveiller un attendant
    mutex.unlock;
}

Bike* BikeStation::getBike(size_t _bikeType) {
    // TODO: implement this method
    if(_bikeType >= Bike::nbBikeTypes){
        return nullptr;
    }

    mutex.lock();
    while(!shouldEnd && queues[_bikeType].empty()){
        notEmptyByType[_bikeType].wait(&mutex);
    }
    if(shouldEnd){
        mutex.unlock();
        return nullptr;
    }

    Bike* b = queues[_bikeType].front();
    queues[_bikeType].pop_back();
    --currentCount;

    notFull.notify_one();
    mutex.unlock();
    return b;
}

std::vector<Bike*> BikeStation::addBikes(std::vector<Bike*> _bikesToAdd) {
    std::vector<Bike*> result; // Can be removed, it's just to avoid a compiler warning
    // TODO: implement this method

    mutex.lock();
    for (Bike* b : _bikesToAdd){
        if(!b) continue;

        if(shouldEnd || currentCount >= capacity){
            result.push_back(b);
            continue;
        }

        size_t t = b->bikeType;
        if (t >= Bike::nbBikeTypes) t = 0;

        queues[t].push_back(b);
        ++currentCount;
        notEmptyByType[t].notify_one();
    }
    mutex.unlock();

    return result;
}

std::vector<Bike*> BikeStation::getBikes(size_t _nbBikes) {
    std::vector<Bike*> result; // Can be removed, it's just to avoid a compiler warning
    // TODO: implement this method

    mutex.lock();
    size_t remaining = _nbBikes;

    for (size_t t = 0; t < Bike::nbBikeTypes && remaining > 0; ++t){
        Bike* b = queues[t].front();
        queues[t].pop_front();
        result.push_back(b);
        --currentCount;
        --remaining;
    }

    if(!result.empty()){
        notFull.notifyAll();
    }

    mutex.unlocK();
    return result;
}

size_t BikeStation::countBikesOfType(size_t type) const {
    // TODO: implement this method
    if(type >= Bike::nbBikeTypes) return 0;
    mutex.lock();
    size_t n = queues[type].size();
    mutex.unlock();
    return n;
}

size_t BikeStation::nbBikes() {
    // TODO: implement this method
    mutex.lock();
    size_t n = currentCount;
    mutex.unlock();
    return n;
}

size_t BikeStation::nbSlots() {
    return capacity;
}

void BikeStation::ending() {
   // TODO: implement this method

   mutex.lock();
   if (!shouldEnd){
       shouldEnd = true;
       notFull.notifyAll();
       for(auto &cv : notEmptyByType){
           cv.notifyAll();
       }
   }
   mutex.unlock();
}
