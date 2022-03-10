// ===========================================================
// Name         : VehicleParser.h
// Author       : Laurens De Wachter & Nabil El Ouaamari
// Version      : 1.0
// Description  : This code is used to parse an XML file that contains a `Vehicle`.
// ===========================================================

#ifndef PSE_VEHICLEPARSER_H
#define PSE_VEHICLEPARSER_H

#include <iostream>
#include <sstream>
#include "ParseException.h"
#include "tinyxml/tinyxml.h"

class Vehicle;

class VehicleParser {
    Vehicle* fVehicle;
    VehicleParser* _initCheck;

public:
    /*
     * ENSURE(properlyInitialized(), "VehicleParser constructor did not end in an initialized state")
     */
    VehicleParser();

    virtual ~VehicleParser();

    bool properlyInitialized() const;

    /*
     * REQUIRE(properlyInitialized(), "VehicleParser wasn't initialized when calling parseVehicle()")
     * ENSURE(fVehicle->getStreet() == fStreet, "parseVehicle() postcondition")
     * ENSURE(fVehicle->getPosition() == fPosition, "parseVehicle() postcondition")
     */
    void parseVehicle(TiXmlElement* VOERTUIG);

    /*
     * REQUIRE(properlyInitialized(), "VehicleParser wasn't initialized when calling getVehicle()")
     */
    Vehicle *getVehicle() const;
};


#endif
