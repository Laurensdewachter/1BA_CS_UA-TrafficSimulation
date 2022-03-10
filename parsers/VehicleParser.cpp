// ===========================================================
// Name         : VehicleParser.cpp
// Author       : Laurens De Wachter & Nabil El Ouaamari
// Version      : 1.0
// Description  : This code is used to parse an XML file that contains either a `Vehicle`.
// ===========================================================

#include "VehicleParser.h"
#include "../objects/Vehicle.h"

VehicleParser::VehicleParser() {
    fVehicle = new Vehicle;
    VehicleParser::_initCheck = this;
    ENSURE(properlyInitialized(), "VehicleParser constructor did not end in an initialized state");
}

VehicleParser::~VehicleParser() {}

bool VehicleParser::properlyInitialized() const {
    return VehicleParser::_initCheck == this;
}

void VehicleParser::parseVehicle(TiXmlElement *VOERTUIG) {
    REQUIRE(properlyInitialized(), "VehicleParser wasn't initialized when calling parseVehicle()");

    TiXmlElement* baanElem = VOERTUIG->FirstChildElement("baan");
    TiXmlElement* positionElem = VOERTUIG->FirstChildElement("positie");
    if (baanElem == NULL && positionElem == NULL) {
        throw ParseException("Vehicle has no contents.\nIt needs both a street and a position.");
    }
    if (baanElem == NULL) {
        throw ParseException("Vehicle has no street.");
    }
    if (positionElem == NULL) {
        throw ParseException("The vehicle has no position.");
    }
    if (baanElem->FirstChild() == NULL) {
        throw ParseException("Vehicle street name is empty");
    }
    TiXmlText* streetText = baanElem->FirstChild()->ToText();
    std::string street = streetText->Value();

    if (positionElem->FirstChild() == NULL) {
        throw ParseException("Vehicle position is empty");
    }
    TiXmlText* positionText = positionElem->FirstChild()->ToText();
    std::string positionString = positionText->Value();
    int position;
    if ((std::istringstream(positionString) >> position).fail()) {
        throw ParseException("Vehicle position is not a number");
    }

    fVehicle->setStreet(street);
    fVehicle->setPosition(position);

    ENSURE(fVehicle->getStreet() == street, "parseVehicle() postcondition");
    ENSURE(fVehicle->getPosition() == position, "parseVehicle() postcondition");
}

Vehicle *VehicleParser::getVehicle() const {
    REQUIRE(properlyInitialized(), "VehicleParser wasn't initialized when calling getVehicle()");
    return fVehicle;
}
