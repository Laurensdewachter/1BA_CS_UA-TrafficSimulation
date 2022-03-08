// ===========================================================
// Name         : ElementParser.h
// Author       : Laurens De Wachter & Nabil El Ouaamari
// Version      : 1.1
// Description  : This code is used to parse an XML file that contains either a `Street`, `TrafficLight`, `Vehicle`
//                or `VehicleGenerator` element
// ===========================================================

#ifndef PSE_ELEMENTPARSER_H
#define PSE_ELEMENTPARSER_H

#include <iostream>
#include <vector>
#include "ParseException.h"
#include "tinyxml/tinyxml.h"
#include "../DesignByContract.h"
#include "StreetParser.h"
#include "TrafficLightParser.h"
#include "VehicleParser.h"
#include "VehicleGeneratorParser.h"

class Street;
class TrafficLight;
class Vehicle;
class VehicleGenerator;

class ElementParser {
    TiXmlElement* root;
    TiXmlDocument doc;
    ElementParser* _initCheck;

    std::vector<Street*> streets;
    std::vector<TrafficLight*> trafficLights;
    std::vector<Vehicle*> vehicles;
    std::vector<VehicleGenerator*> vehicleGenerators;

public:
    /*
     * ENSURE(properlyInitialized(), "ElementParser constructor did not end in an initialized state")
     */
    ElementParser();

    ~ElementParser();

    bool properlyInitialized() const;

    /*
     * This method loads an XML file and immediately parses it using the individual parsers for each element type.
     * Returns true when the file was loaded successfully.
     * REQUIRE(properlyInitialized(), "ElementParser wasn't initialized when calling parseFile()")
     */
    void parseFile(const std::string &filename);
};


#endif
