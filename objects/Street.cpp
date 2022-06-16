// ===========================================================
// Name         : Street.cpp
// Author       : Laurens De Wachter & Nabil El Ouaamari
// Version      : 1.1
// Description  : This code is contains the `Street` class
// ===========================================================

#include <algorithm>
#include "Street.h"
#include "TrafficLight.h"
#include "VehicleGenerator.h"
#include "BusStop.h"
#include "Vehicle.h"
#include "vehicles/Car.h"
#include "vehicles/Bus.h"
#include "vehicles/FireEngine.h"
#include "vehicles/Ambulance.h"
#include "vehicles/PoliceCar.h"
#include "../DesignByContract.h"
#include "../Variables.h"

Street::Street(const std::string &name, int length) : fName(name), fLength(length) {
    fVehicleGenerator = NULL;
    _initCheck = this;

    ENSURE(properlyInitialized(), "Street constructor did not end in an initialized state");
}

Street::~Street() {
    for (unsigned int i = 0; i < fTrafficLights.size(); i++) {
        delete fTrafficLights[i];
    }
    for (unsigned int i = 0; i < fVehicles.size(); i++) {
        delete fVehicles[i];
    }
    delete fVehicleGenerator;
}

bool Street::properlyInitialized() const {
    return Street::_initCheck == this;
}

void Street::addTrafficLight(TrafficLight *t) {
    REQUIRE(properlyInitialized(), "Street wasn't initialized when calling addTrafficLight()");

    unsigned int trafficLightsSize = fTrafficLights.size();
    fTrafficLights.push_back(t);

    ENSURE(fTrafficLights.size() == trafficLightsSize+1, "addTrafficLight() postcondition");
}

void Street::addVehicle(Vehicle *v) {
    REQUIRE(properlyInitialized(), "Street wasn't initialized when calling addVehicle()");

    unsigned int vehiclesSize = fVehicles.size();
    fVehicles.push_back(v);

    ENSURE(fVehicles.size() == vehiclesSize+1, "addVehicle() postcondition");
}

void Street::addBusStop(BusStop *b) {
    REQUIRE(properlyInitialized(), "Street wasn't initialized when calling addBusStop()");

    unsigned int busStopsSize = fBusStops.size();
    fBusStops.push_back(b);

    ENSURE(fBusStops.size() == busStopsSize+1, "addBusStop() postcondition");
}

void Street::addCrossroad(Street* crossingStreet, unsigned int position) {
    REQUIRE(properlyInitialized(), "Street wasn't initialized when calling addCrossroad()");

    unsigned int crossroadSize = fCrossroads.size();
    std::pair<Street*, unsigned int> addPair;
    addPair.first = crossingStreet;
    addPair.second = position;
    fCrossroads.push_back(addPair);

    ENSURE(fCrossroads.size() == crossroadSize+1, "addCrossroad() postcondition");
}

void Street::setVehicleGenerator(VehicleGenerator *vg) {
    REQUIRE(properlyInitialized(), "Street wasn't initialized when calling addVehicleGenerator()");

    fVehicleGenerator = vg;

    ENSURE(fVehicleGenerator == vg, "addVehicleGenerator() postcondition");
}

void Street::removeVehicle() {
    REQUIRE(properlyInitialized(), "Street wasn't initialized when calling removeVehicle()");

    unsigned int vehiclesSize = fVehicles.size();
    delete fVehicles[0];
    fVehicles.erase(fVehicles.begin());

    ENSURE(fVehicles.size() == vehiclesSize-1, "removeVehicle() postcondition");
}

const std::string &Street::getName() const {
    REQUIRE(properlyInitialized(), "Street wasn't initialized when calling getName()");

    return Street::fName;
}

int Street::getLength() const {
    REQUIRE(properlyInitialized(), "Street wasn't initialized when calling getLength()");

    return Street::fLength;
}

std::vector<TrafficLight *> Street::getTrafficLights() const {
    REQUIRE(properlyInitialized(), "Street wasn't initialized when calling getTrafficLights()");

    return fTrafficLights;
}

std::vector<Vehicle*> Street::getVehicles() const {
    REQUIRE(properlyInitialized(), "Street wasn't initialized when calling getVehicles()");

    return fVehicles;
}

std::vector<BusStop*> Street::getBusStops() const {
    REQUIRE(properlyInitialized(), "Street wasn't initialized when calling getBusStops()");

    return fBusStops;
}

std::vector<std::pair<Street*, unsigned int> > Street::getCrossroads() const {
    REQUIRE(properlyInitialized(), "Street wasn't initialized when calling getCrossroads()");

    return fCrossroads;
}

bool Street::hasVehicleGenerator() const {
    REQUIRE(properlyInitialized(), "Street wasn't initialized when calling hasVehicleGenerator()");

    return fVehicleGenerator != NULL;
}

void Street::driveVehicles() {
    REQUIRE(properlyInitialized(), "Street wasn't initialized when calling driveVehicles()");

    if (fVehicles.empty()) {
        return;
    }

    std::vector<double> originalPositions;
    for (unsigned int i = 0; i < fVehicles.size(); i++) {
        originalPositions.push_back(fVehicles[i]->getPosition());
        if (i == 0) {
            fVehicles[i]->drive(NULL);
            if (fVehicles[i]->getPosition() > fLength) {
                this->removeVehicle();
                originalPositions.erase(originalPositions.begin());
                i -= 1;
            }
            continue;
        }
        fVehicles[i]->drive(fVehicles[i-1]);
    }

    for (unsigned int i = 0; i < fVehicles.size(); i++) {
        ENSURE(fVehicles[i]->getPosition() >= originalPositions[i] - 0.001, "driveVehicles() postcondition");
    }
}

void Street::simTrafficLights(double &time) {
    REQUIRE(properlyInitialized(), "Street wasn't initialized when calling simTrafficLights()");
    if (fTrafficLights.empty()) {
        return;
    }

    for (unsigned int i = 0; i < fTrafficLights.size(); i++) {
        // Change traffic light if necessary
        TrafficLight *curTrafficLight = fTrafficLights[i];

        if (time >= curTrafficLight->getLastUpdateTime()) {
            curTrafficLight->changeLight();
            curTrafficLight->setLastUpdateTime(curTrafficLight->getLastUpdateTime() + curTrafficLight->getCycle());
        }

        // Get vehicle closest to the traffic light
        if (fVehicles.empty()) {
            return;
        }
        Vehicle *closestVehicle = NULL;
        for (unsigned int v = 0; v < fVehicles.size(); v++) {
            if (fVehicles[v]->hasPriority()) {
                continue;
            }
            if (fVehicles[v]->getPosition() < curTrafficLight->getPosition()) {
                if (closestVehicle == NULL || fVehicles[v]->getPosition() > closestVehicle->getPosition()) {
                    closestVehicle = fVehicles[v];
                }
            }
        }

        // Go to next traffic light if there is no vehicle before the traffic light
        if (closestVehicle == NULL) {
            continue;
        }

        // Traffic light is green
        if (curTrafficLight->isGreen()) {
            closestVehicle->setMaxSpeed();
        }

        // Traffic light is red
        else {
            double distance = curTrafficLight->getPosition() - closestVehicle->getPosition();
            if (distance > 0 && distance < gStopDistance) {
                closestVehicle->stop();
            }
            else if (distance > 0 && distance < gBrakeDistance) {
                closestVehicle->brake();
            }
        }
    }

    // TODO: add ENSURE here
}

void Street::simGenerator(double &time) {
    REQUIRE(properlyInitialized(), "Street wasn't initialized when calling simGenerator()");

    long unsigned int startSize = fVehicles.size();

    if (fVehicleGenerator == NULL) {
        ENSURE(fVehicles.size() == startSize, "simGenerator() postcondition");
        return;
    }

    bool spawn = false;
    if (fVehicleGenerator->getTimeSinceLastSpawn() < time) {
        spawn = true;

        Vehicle* newVehicle;
        std::string type = fVehicleGenerator->getType();
        if (type == "auto") {
            newVehicle = new Car(fName, 0);
        } else if (type == "bus") {
            newVehicle = new Bus(fName, 0);
        } else if (type == "brandweerwagen") {
            newVehicle = new FireEngine(fName, 0);
        } else if (type == "ziekenwagen") {
            newVehicle = new Ambulance(fName, 0);
        } else {
            newVehicle = new PoliceCar(fName, 0);
        }

        fVehicles.push_back(newVehicle);

        fVehicleGenerator->setTimeSinceLastSpawn(fVehicleGenerator->getTimeSinceLastSpawn() + fVehicleGenerator->getFrequency());
    }

    if (spawn) {
        ENSURE(fVehicles.size() == startSize+1, "simGenerator() postcondition");
    }
    else {
        ENSURE(fVehicles.size() == startSize, "simGenerator() postcondition");
    }
}

void Street::simBusStops(double &time) {
    REQUIRE(properlyInitialized(), "Street wasn't initialized when calling simBusStops()");
    if (fBusStops.empty()) {
        return;
    }

    for (unsigned int i = 0; i < fBusStops.size(); i++) {
        BusStop *curBusStop = fBusStops[i];
        std::vector<Vehicle*> arrivedBusses = curBusStop->getArrivedBusses();

        if (fVehicles.empty()) {
            return;
        }
        Vehicle *closestBus = NULL;
        for (unsigned int v = 0; v < fVehicles.size(); v++) {
            if (fVehicles[v]->getType() != "Bus") {
                continue;
            }
            if (fVehicles[v]->getPosition() < curBusStop->getPosition()) {
                if ((closestBus == NULL || fVehicles[v]->getPosition() > closestBus->getPosition())
                        && std::find(arrivedBusses.begin(), arrivedBusses.end(), fVehicles[v]) == arrivedBusses.end()) {
                    closestBus = fVehicles[v];
                }
            }
        }
        if (closestBus == NULL) {
            continue;
        }

        Bus* closestBusType = dynamic_cast<Bus*>(closestBus);

        double distance = curBusStop->getPosition() - closestBus->getPosition();
        if (distance > 0 && distance < gStopDistance) {
            closestBus->stop();
            closestBusType->addWaitTime(gSimulationTime);
        }
        else if (distance > 0 && distance < gBrakeDistance) {
            closestBus->brake();
        }
        if (closestBusType->getWaitTime() > curBusStop->getWaitTime()) {
            curBusStop->addArrivedBus(closestBusType);
        }
    }
}

void Street::sortVehicles() {
    REQUIRE(properlyInitialized(), "Street wasn't initialized when calling sortVehicles()");

    if (fVehicles.empty()) {
        return;
    }

    std::vector<Vehicle *> newVehicles;
    newVehicles.push_back(fVehicles[0]);

    for (unsigned int i = 1; i < fVehicles.size(); i++) {
        for (unsigned int j = 0; j < newVehicles.size(); j++) {
            if (fVehicles[i]->getPosition() > newVehicles[j]->getPosition()) {
                newVehicles.insert(newVehicles.begin() + j, fVehicles[i]);
                break;
            } else if (j == newVehicles.size() - 1) {
                newVehicles.push_back(fVehicles[i]);
                break;
            }
        }
    }
    fVehicles = newVehicles;

    ENSURE(!fVehicles.empty(), "sortVehicles() postcondition");
}