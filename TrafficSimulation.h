// ===========================================================
// Name         : TrafficSimulation.h
// Author       : Laurens De Wachter & Nabil El Ouaamari
// Version      : 1.0
// Description  : This code is contains the `TrafficSimulation` class
// ===========================================================

#ifndef PSE_TRAFFICSIMULATION_H
#define PSE_TRAFFICSIMULATION_H

#include <iostream>
#include <vector>
#include <map>
#include "DesignByContract.h"

class Street;
class TrafficLight;
class Vehicle;
class VehicleGenerator;

class TrafficSimulation {
    std::map<std::string, Street*> fStreets;
    std::vector<TrafficLight*> fTrafficLights;
    std::vector<Vehicle*> fVehicles;
    std::map<std::string, Vehicle*> fVehiclesByStreet;
    std::vector<VehicleGenerator*> fVehicleGenerators;
    TrafficSimulation* _initCheck;

public:
    /*
     * ENSURE(properlyInitialized(), "TrafficSimulation constructor did not end in an initialized state")
     */
    TrafficSimulation();

    virtual ~TrafficSimulation();

    bool properlyInitialized() const;

    /*
     * REQUIRE(properlyInitialized(), "TrafficSimulation wasn't initialized when calling parseInputFile()")
     */
    void parseInputFile(const std::string &filename);

    /*
     * REQUIRE(properlyInitialized(), "TrafficSimulation wasn't initialized when calling drive()")
     * REQUIRE(!fVehicles.empty(), "There were no vehicles in TrafficSimulation when calling drive()")
     */
    void drive();

private:
    void sortVehicles();
};


#endif