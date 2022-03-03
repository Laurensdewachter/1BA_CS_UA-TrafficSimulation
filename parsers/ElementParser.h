// ===========================================================
// Name         : ElementParser.h
// Author       : Laurens De Wachter & Nabil El Ouaamari
// Version      : 1.0
// Description  : This code is used to parse an XML file that contains either a `Street`, `TrafficLight` or `Vehicle` element
// ===========================================================

#ifndef PSE_ELEMENTPARSER_H
#define PSE_ELEMENTPARSER_H

#include <iostream>
#include <vector>
#include <exception>
#include "tinyxml/tinyxml.h"
#include "StreetParser.h"
#include "TrafficLightParser.h"
#include "VehicleParser.h"

class Street;
class TrafficLight;
class Vehicle;

class ElementParser {
    TiXmlElement* root;
    TiXmlDocument doc;

    std::vector<Street*> streets;
    std::vector<TrafficLight*> trafficLights;
    std::vector<Vehicle*> vehicles;

public:
    ElementParser() {}
    ~ElementParser() {}

    /*
     * This method loads an XML file and immediately parses it using the individual parsers for each element type.
     * Returns true when the file was loaded successfully.
     */
    bool parseFile(const std::string &filename);
};


#endif //PSE_ELEMENTPARSER_H
