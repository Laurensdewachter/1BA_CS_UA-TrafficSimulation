// ===========================================================
// Name         : TrafficLightParser.cpp
// Author       : Laurens De Wachter & Nabil El Ouaamari
// Version      : 1.1
// Description  : This code is used to parse an XML file that contains a `TrafficLight`.
// ===========================================================

#include <sstream>
#include "TrafficLightParser.h"
#include "../DesignByContract.h"
#include "../objects/TrafficLight.h"

TrafficLightParser::TrafficLightParser() {
    TrafficLightParser::_initCheck = this;

    ENSURE(properlyInitialized(), "TrafficLightParser constructor did not end in an initialized state");
}

TrafficLightParser::~TrafficLightParser() {}

bool TrafficLightParser::properlyInitialized() const {
    return TrafficLightParser::_initCheck == this;
}

bool TrafficLightParser::parseTrafficLight(TiXmlElement* VERKEERSLICHT, std::ostream &errStream) {
    REQUIRE(properlyInitialized(), "TrafficLightParser wasn't initialized when calling parseTrafficLight()");
    REQUIRE(errStream.good(), "The errorStream wasn't good at the beginning of parseTrafficLight()");

    TiXmlElement* streetElem = VERKEERSLICHT->FirstChildElement("baan");
    TiXmlElement* positionElem = VERKEERSLICHT->FirstChildElement("positie");
    TiXmlElement* cycleElem = VERKEERSLICHT->FirstChildElement("cyclus");

    std::string street;
    int position;
    int cycle;

    bool wrongTypes = false;
    bool missingElements = false;
    if (streetElem == NULL || streetElem->FirstChild() == NULL) {
        errStream << "XML PARTIAL IMPORT: Expected <baan> ... </baan>." << std::endl;
        missingElements = true;
    } else {
        TiXmlText* streetText = streetElem->FirstChild()->ToText();
        street = streetText->Value();
    }
    if (positionElem == NULL || positionElem->FirstChild() == NULL) {
        errStream << "XML PARTIAL IMPORT: Expected <positie> ... </positie>." << std::endl;
        missingElements = true;
    } else {
        std::string positionString = positionElem->FirstChild()->ToText()->Value();
        if ((std::istringstream(positionString) >> position).fail()) {
            errStream << "XML PARTIAL IMPORT: Expected <positie> to be an integer." << std::endl;
            wrongTypes = true;
        }
    }
    if (cycleElem == NULL || cycleElem->FirstChild() == NULL) {
        errStream << "XML PARTIAL IMPORT: Expected <cyclus> ... </cyclus>." << std::endl;
        missingElements = true;
    } else {
        std::string cycleString = cycleElem->FirstChild()->ToText()->Value();
        if ((std::istringstream(cycleString) >> cycle).fail()) {
            errStream << "XML PARTIAL IMPORT: Expected <cyclus> to be an integer." << std::endl;
            wrongTypes = true;
        }
        if (cycle <= 0) {
            errStream << "XML PARTIAL IMPORT: Expected <cyclus> to be a positive integer." << std::endl;
            wrongTypes = true;
        }
    }

    if (missingElements || wrongTypes) {
        return false;
    }

    fTrafficLight = new TrafficLight(street, position, cycle);

    ENSURE(fTrafficLight != NULL, "TrafficLightParser could not create a TrafficLight");
    ENSURE(fTrafficLight->getStreet() == street, "parseTrafficLight() postcondition");
    ENSURE(fTrafficLight->getPosition() == position, "parseTrafficLight() postcondition");
    ENSURE(fTrafficLight->getCycle() == cycle, "parseTrafficLight() postcondition");
    ENSURE(errStream.good(), "The errorStream wasn't good at the end of parseTrafficLight()");

    return true;
}

TrafficLight* TrafficLightParser::getTrafficLight() const {
    REQUIRE(properlyInitialized(), "TrafficLightParser wasn't initialized when calling getTrafficLight()");
    REQUIRE(fTrafficLight != NULL, "TrafficLightParser had no traffic light when calling getTrafficLight()");

    return fTrafficLight;
}
