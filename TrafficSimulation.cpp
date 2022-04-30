// ===========================================================
// Name         : TrafficSimulation.cpp
// Author       : Laurens De Wachter & Nabil El Ouaamari
// Version      : 1.0
// Description  : This code is contains the `TrafficSimulation` class
// ===========================================================

#include "TrafficSimulation.h"
#include "Variables.h"
#include "objects/Street.h"
#include "objects/TrafficLight.h"
#include "objects/Vehicle.h"
#include "objects/VehicleGenerator.h"
#include "objects/BusStop.h"

TrafficSimulation::TrafficSimulation() {
    TrafficSimulation::fTime = 0;
    TrafficSimulation::_initCheck = this;

    ENSURE(properlyInitialized(), "TrafficSimulation constructor did not end in an initialized state");
}

TrafficSimulation::~TrafficSimulation() {}

bool TrafficSimulation::properlyInitialized() const {
    return TrafficSimulation::_initCheck == this;
}

EParserSucces TrafficSimulation::parseInputFile(const std::string &filename, std::ostream &errStream) {
    REQUIRE(properlyInitialized(), "TrafficSimulation wasn't initialized when calling parseInputFile()");
    REQUIRE(errStream.good(), "The errorStream wasn't good when calling parseInputFile()");

    EParserSucces parseSucces = Success;

    ElementParser parser;
    parseSucces = parser.parseFile(filename, errStream);

    TrafficSimulation::fStreets = parser.getStreets();
    std::vector<TrafficLight*> trafficLights = parser.getTrafficLights();
    std::vector<Vehicle*> vehicles = parser.getVehicles();
    std::vector<VehicleGenerator*> vehicleGenerators = parser.getVehicleGenerators();
    std::vector<BusStop*> busStops = parser.getBusStops();

    for (long unsigned int i = 0; i < trafficLights.size(); i++) {
        TrafficLight* curTrafficLight = trafficLights[i];
        Street* curStreet = getStreet(curTrafficLight->getStreet());
        if (curStreet != NULL && curTrafficLight->getPosition() < curStreet->getLength()) {
            std::vector<TrafficLight*> trafficLightsOnStreet = curStreet->getTrafficLights();
            bool validTrafficLight = true;
            for (long unsigned int j = 0; j < trafficLightsOnStreet.size(); j++) {
                int position = trafficLightsOnStreet[j]->getPosition();
                if (abs(position - curTrafficLight->getPosition()) < gBrakeDistance) {
                    validTrafficLight = false;
                }
            }
            if (validTrafficLight) {
                curStreet->addTrafficLight(curTrafficLight);
            } else {
                errStream << "XML IMPORT ABORT: The simulation is not consistent." << std::endl;
                return ImportAborted;
            }
        } else {
            errStream << "XML IMPORT ABORT: The simulation is not consistent." << std::endl;
            return ImportAborted;
        }
    }

    for (long unsigned int i = 0; i < vehicles.size(); i++) {
        Vehicle* curVehicle = vehicles[i];
        Street* curStreet = getStreet(curVehicle->getStreet());
        if (curStreet != NULL && curVehicle->getPosition() < curStreet->getLength()) {
            curStreet->addVehicle(curVehicle);
        } else {
            errStream << "XML IMPORT ABORT: The simulation is not consistent." << std::endl;
            return ImportAborted;
        }
    }

    for (long unsigned int i = 0; i < vehicleGenerators.size(); i++) {
        VehicleGenerator* curVehicleGenerator = vehicleGenerators[i];
        Street* curStreet = getStreet(curVehicleGenerator->getStreet());
        if (curStreet != NULL && !curStreet->hasVehicleGenerator()) {
            curStreet->setVehicleGenerator(curVehicleGenerator);
        } else {
            errStream << "XML IMPORT ABORT: The simulation is not consistent." << std::endl;
            return ImportAborted;
        }
    }

    for (long unsigned int i = 0; i < busStops.size(); i++) {
        BusStop* curBusStop = busStops[i];
        Street* curStreet = getStreet(curBusStop->getStreet());
        if (curStreet != NULL && curBusStop->getPosition() < curStreet->getLength()) {
            curStreet->addBusStop(curBusStop);
        } else {
            errStream << "XML IMPORT ABORT: The simulation is not consistent." << std::endl;
            return ImportAborted;
        }
    }

    for (unsigned int i = 0; i < fStreets.size(); i++) {
        fStreets[i]->sortVehicles();
    }

    ENSURE(errStream.good(), "The errorStream wasn't good at the end of parseInputFile()");
    ENSURE(parseSucces == Success || parseSucces == PartialImport || parseSucces == ImportAborted, "The parser did not return a proper succes or aborted value");

    return parseSucces;
}

void TrafficSimulation::writeOn(std::ostream &onstream) const {
    REQUIRE(properlyInitialized(), "TrafficSimulation wasn't initialized when calling writeOn()");
    REQUIRE(onstream.good(), "The outputStream wasn't good when calling writeOn()");

    onstream << "Tijd: " << fTime << std::endl;

    int voertuigCounter = 1;
    for (unsigned int i = 0; i < fStreets.size(); i++) {
        std::vector<Vehicle*> vehicles = fStreets[i]->getVehicles();
        for (unsigned int j = 0; j < vehicles.size(); j++) {
            Vehicle* curVehicle = vehicles[j];
            onstream << "Voertuig " << voertuigCounter << std::endl;
            onstream << "-> baan: " << curVehicle->getStreet() << std::endl;
            onstream << "-> positie: " << curVehicle->getPosition() << std::endl;
            onstream << "-> snelheid: " << curVehicle->getSpeed() << std::endl << std::endl;
            voertuigCounter++;
        }
    }

    ENSURE(onstream.good(), "The outputStream wasn't good at the end of writeOn()");
}

void TrafficSimulation::visualize(std::ostream &onstream) const {
    REQUIRE(properlyInitialized(), "TrafficSimulation wasn't initialized when calling visualize()");
    REQUIRE(onstream.good(), "The outputStream wasn't good when calling visualize()");

    onstream << "{\"time\": " << fTime << ", \"roads\": [ ";
    for (unsigned int i = 0; i < fStreets.size(); i++) {
        onstream << "{\"name\": \"" << fStreets[i]->getName() << "\", "
        << "\"length\": " << fStreets[i]->getLength() << ", "
        << "\"cars\": [ ";
        for (unsigned int k = 0; k < fStreets[i]->getVehicles().size(); k++) {
            Vehicle* curVehicle = fStreets[i]->getVehicles()[k];
            onstream << "{\"x\": " << curVehicle->getPosition();
            EVehicleType curType = curVehicle->getType();
            switch (curType) {
                case Car: {
                    onstream << ", \"type\": \"car\"}";
                    break;
                }
                case Bus: {
                    onstream << ", \"type\": \"bus\"}";
                    break;
                }
                case FireEngine: {
                    onstream << ", \"type\": \"firetruck\"}";
                    break;
                }
                case Ambulance: {
                    onstream << ", \"type\": \"ambulance\"}";
                    break;
                }
                case PoliceCar: {
                    onstream << ", \"type\": \"police_cruiser\"}";
                    break;
                }
            }
            if (k != fStreets[i]->getVehicles().size()-1) {
                onstream << ", ";
            }
        }
        onstream << " ], \"lights\": [ ";
        for (unsigned int k = 0; k < fStreets[i]->getTrafficLights().size(); k++) {
            TrafficLight* curTrafficLight = fStreets[i]->getTrafficLights()[k];
            onstream << "{\"x\": " << curTrafficLight->getPosition() << ", \"green\": " << int(curTrafficLight->isGreen())
                     << ", \"xs\": " << gBrakeDistance << ", \"xs0\": " << gStopDistance << "}";
            if (k != fStreets[i]->getTrafficLights().size()-1) {
                onstream << ", ";
            } else {
                onstream << " ]";
            }
        }
        if (fStreets[i]->getTrafficLights().size() == 0) {
            onstream << "]";
        }
        if (i != fStreets.size()-1) {
            onstream << " }, ";
        }
    }
    onstream << "} ] }" << std::endl;

    ENSURE(onstream.good(), "The outputStream wasn't good at the end of visualize()");
}

void TrafficSimulation::simulate() {
    REQUIRE(properlyInitialized(), "TrafficSimulation wasn't initialized when calling simulate()");

    for (long unsigned int i = 0; i < fStreets.size(); i++) {
        fStreets[i]->simGenerator(fTime);
        fStreets[i]->driveVehicles();
        fStreets[i]->simTrafficLights(fTime);
    }
    fTime += gSimulationTime;
}

void TrafficSimulation::clearSimulation() {
    REQUIRE(properlyInitialized(), "TrafficSimulation wasn't initialized when calling clearSimulation()");

    fTime = 0;
    for (unsigned int i = 0; i < fStreets.size(); i++) {
        delete fStreets[i];
    }
    fStreets.clear();

    ENSURE(fStreets.empty(), "The streets vector wasn't empty at the end of clearSimulation()");
}

const std::vector<Street *> &TrafficSimulation::getStreets() const {
    REQUIRE(properlyInitialized(), "TrafficSimulation wasn't initialized when calling getStreets()");
    return fStreets;
}

double TrafficSimulation::getTime() const {
    REQUIRE(properlyInitialized(), "TrafficSimulation wasn't initialized when calling getTime()");
    return fTime;
}

Street *TrafficSimulation::getStreet(const std::string &name) const {
    for (long unsigned int i = 0; i < fStreets.size(); i++) {
        if (fStreets[i]->getName() == name) {
            return fStreets[i];
        }
    }
    return NULL;
}
