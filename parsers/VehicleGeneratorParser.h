// ===========================================================
// Name         : VehicleGeneratorParser.h
// Author       : Laurens De Wachter & Nabil El Ouaamari
// Version      : 1.1
// Description  : This code is used to parse an XML file that contains a `VehicleGenerator`.
// ===========================================================

#ifndef PSE_VEHICLEGENERATORPARSER_H
#define PSE_VEHICLEGENERATORPARSER_H

#include <iostream>
#include <sstream>
#include "tinyxml/tinyxml.h"
#include "../DesignByContract.h"

class VehicleGenerator;

class VehicleGeneratorParser {
    VehicleGenerator* fVehicleGenerator;
    VehicleGeneratorParser* _initCheck;

public:
    /*
     * ENSURE(properlyInitialized(), "VehicleGeneratorParser constructor did not end in an initialized state")
     */
    VehicleGeneratorParser();

    virtual ~VehicleGeneratorParser();

    bool properlyInitialized() const;

    /*
     * REQUIRE(properlyInitialized(), "VehicleGeneratorParser wasn't initialized when calling parseVehicleGenerator()")
     * REQUIRE(errStream.good(), "The errorStream wasn't good")
     * ENSURE(vehicleGenerator->getStreet() == fStreet, "parseVehicleGenerator() postcondition")
     * ENSURE(vehicleGenerator->getFrequency() == fFrequency, "parseVehicleGenerator() postcondition")
     */
    bool parseVehicleGenerator(TiXmlElement* VOERTUIGGENERATOR, std::ostream &errStream);

    /*
     * REQUIRE(properlyInitialized(), "VehicleGeneratorParser wasn't initialized when calling getVehicleGenerator()")
     * REQUIRE(fVehicleGenerator != NULL, "VehicleGeneratorParser had no vehicle generator when calling getVehicleGenerator()")
     */
    VehicleGenerator* getVehicleGenerator() const;
};


#endif
