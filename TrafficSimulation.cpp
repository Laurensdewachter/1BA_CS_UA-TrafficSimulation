// ===========================================================
// Name         : TrafficSimulation.cpp
// Author       : Laurens De Wachter & Nabil El Ouaamari
// Version      : 1.0
// Description  : This code is contains the `TrafficSimulation` class
// ===========================================================

#include <iostream>
#include <algorithm>
#include <sstream>
#include "TrafficSimulation.h"
#include "DesignByContract.h"
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

TrafficSimulation::~TrafficSimulation() {
    clearSimulation();

    ENSURE(fStreets.empty(), "TrafficSimulation destructor did not end in an empty state");
}

bool TrafficSimulation::properlyInitialized() const {
    return TrafficSimulation::_initCheck == this;
}

EParserSuccess TrafficSimulation::parseInputFile(const std::string &filename, std::ostream &errStream) {
    REQUIRE(properlyInitialized(), "TrafficSimulation wasn't initialized when calling parseInputFile()");
    REQUIRE(errStream.good(), "The errorStream wasn't good when calling parseInputFile()");

    EParserSuccess parseSuccess = Success;

    ElementParser parser;
    parseSuccess = parser.parseFile(filename, errStream);

    TrafficSimulation::fStreets = parser.getStreets();
    std::vector<TrafficLight*> trafficLights = parser.getTrafficLights();
    std::vector<Vehicle*> vehicles = parser.getVehicles();
    std::vector<VehicleGenerator*> vehicleGenerators = parser.getVehicleGenerators();
    std::vector<BusStop*> busStops = parser.getBusStops();
    std::vector<std::pair<std::pair<std::string, unsigned int>, std::pair<std::string, unsigned int> > > crossroads = parser.getCrossroads();

    for (long unsigned int i = 0; i < trafficLights.size(); i++) {
        TrafficLight* curTrafficLight = trafficLights[i];
        Street* curStreet = getStreetFromString(curTrafficLight->getStreet());
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
        Street* curStreet = getStreetFromString(curVehicle->getStreet());
        if (curStreet != NULL && curVehicle->getPosition() < curStreet->getLength()) {
            curStreet->addVehicle(curVehicle);
        } else {
            errStream << "XML IMPORT ABORT: The simulation is not consistent." << std::endl;
            return ImportAborted;
        }
    }

    for (long unsigned int i = 0; i < vehicleGenerators.size(); i++) {
        VehicleGenerator* curVehicleGenerator = vehicleGenerators[i];
        Street* curStreet = getStreetFromString(curVehicleGenerator->getStreet());
        if (curStreet != NULL && !curStreet->hasVehicleGenerator()) {
            curStreet->setVehicleGenerator(curVehicleGenerator);
        } else {
            errStream << "XML IMPORT ABORT: The simulation is not consistent." << std::endl;
            return ImportAborted;
        }
    }

    for (long unsigned int i = 0; i < busStops.size(); i++) {
        BusStop* curBusStop = busStops[i];
        Street* curStreet = getStreetFromString(curBusStop->getStreet());
        if (curStreet != NULL && curBusStop->getPosition() < curStreet->getLength()) {
            curStreet->addBusStop(curBusStop);
        } else {
            errStream << "XML IMPORT ABORT: The simulation is not consistent." << std::endl;
            return ImportAborted;
        }
    }

    for (long unsigned int i = 0; i < crossroads.size(); i++) {
        std::pair<std::string, unsigned int> curPair1 = crossroads[i].first;
        std::pair<std::string, unsigned int> curPair2 = crossroads[i].second;
        Street* curStreet1 = getStreetFromString(curPair1.first);
        Street* curStreet2 = getStreetFromString(curPair2.first);
        if (curStreet1 == NULL || curStreet2 == NULL || (int) curPair1.second > curStreet1->getLength() || (int) curPair2.second > curStreet2->getLength()) {
            errStream << "XML IMPORT ABORT: The simulation is not consistent." << std::endl;
            return ImportAborted;
        }
        curStreet1->addCrossroad(curStreet2, curPair1.second);
        curStreet2->addCrossroad(curStreet1, curPair2.second);
    }

    for (unsigned int i = 0; i < fStreets.size(); i++) {
        fStreets[i]->sortVehicles();
    }

    ENSURE(errStream.good(), "The errorStream wasn't good at the end of parseInputFile()");
    ENSURE(parseSuccess == Success || parseSuccess == PartialImport || parseSuccess == ImportAborted, "The parser did not return a proper success or aborted value");

    return parseSuccess;
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
            std::string curType = curVehicle->getType();
            if (curType == "Car") {
                onstream << ", \"type\": \"car\"}";
            }
            else if (curType == "Bus") {
                onstream << ", \"type\": \"bus\"}";
            }
            else if (curType == "FireEngine") {
                onstream << ", \"type\": \"firetruck\"}";
            }
            else if (curType == "Ambulance") {
                onstream << ", \"type\": \"ambulance\"}";
            }
            else {
                onstream << ", \"type\": \"police_cruiser\"}";
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
        if (fStreets[i]->getTrafficLights().empty()) {
            onstream << "] ";
        }
        if (i != fStreets.size()-1) {
            onstream << " }, ";
        }
    }
    if (fStreets.empty()) {
        onstream << "] }" << std::endl;
    } else {
        onstream << "} ] }" << std::endl;
    }

    ENSURE(onstream.good(), "The outputStream wasn't good at the end of visualize()");
}

void TrafficSimulation::graph(std::ostream &onstream) const {
    REQUIRE(properlyInitialized(), "TrafficSimulation wasn't initialized when calling graph()");
    REQUIRE(onstream.good(), "The outputStream wasn't good when calling graph()");

    std::string verkeerslichten = " > verkeerslichten";
    std::string bushaltes = " > bushaltes";

    for(int i = 0; i<(int)getStreets().size(); i++){
        std::vector<Vehicle*> vehicles = getStreets()[i]->getVehicles();
        std::vector<TrafficLight*> lights = getStreets()[i]->getTrafficLights();
        std::vector<BusStop*> busstops = getStreets()[i]->getBusStops();
        
        std::vector<int> sizes; sizes.push_back(fStreets[i]->getName().size()); sizes.push_back(bushaltes.size()); sizes.push_back(verkeerslichten.size());
        int alignment = *max_element(sizes.begin(),sizes.end()) + 2;

        int streetlength = (int)getStreets()[i]->getLength()/10;

        std::string line1; std::string line2; std::string line3;

        for(int stl = 0; stl<streetlength;stl++){
            line1 += "="; line2 += " "; line3 += " ";
        }

        for(int s = 0; s < (int)getStreets()[i]->getLength()/10;s++){
            for(int j = 0; j < (int)vehicles.size(); j++){
                if(s == (int)vehicles[j]->getPosition()/10){
                    line1[s] = vehicles[j]->getAcronym();
                }
            }

            for(int l = 0; l < (int)lights.size(); l++){

                if(s == (int)lights[l]->getPosition()/10){
                    char state = 'R';

                    if(lights[l]->isGreen()){state = 'G';}

                    line2[s] = state;
                }
            }

            for(int b = 0; b<(int)busstops.size();b++){
                line3[busstops[b]->getPosition()/10] = 'B';
                line2[busstops[b]->getPosition()/10] = '|';
            }

        }

        onstream << std::left;
        onstream.width(alignment); onstream  << fStreets[i]->getName()  << "| " << line1 << std::endl;
        onstream.width(alignment); onstream  << verkeerslichten         << "| " << line2 << std::endl;
        onstream.width(alignment); onstream  << bushaltes               << "| " << line3 << std::endl;
        onstream  << std::endl;
    }

    ENSURE(onstream.good(), "The outputStream wasn't good at the end of graph()");
}

void TrafficSimulation::simulate() {
    REQUIRE(properlyInitialized(), "TrafficSimulation wasn't initialized when calling simulate()");

    unsigned int beginSize = fStreets.size();
    for (long unsigned int i = 0; i < fStreets.size(); i++) {
        simCrossroads();
        fStreets[i]->simGenerator(fTime);
        fStreets[i]->driveVehicles();
        fStreets[i]->simTrafficLights(fTime);
        fStreets[i]->simBusStops(fTime);
    }

    fTime += gSimulationTime;

    ENSURE(fStreets.size() == beginSize, "The number of streets changed when calling simulate()");
}


void TrafficSimulation::simCrossroads() {
    REQUIRE(properlyInitialized(), "TrafficSimulation wasn't initialized when calling simCrossroads()");

    for(int s = 0; s<(int)fStreets.size();s++) {
        std::map<Street*,int> kruispunten = fStreets[s]->getCrossroads();

        for(int v = 0; v<(int)fStreets[s]->getVehicles().size();v++){

            Vehicle * veh = fStreets[s]->getVehicles()[v];

            int pos_veh = veh->getPosition();
            std::string str_veh = veh->getStreet();
            std::string type_veh = veh->getType();

            for (std::map<Street*, int>::iterator it = kruispunten.begin(); it != kruispunten.end(); it++){
                int pos_new = findPosition(fStreets[s],it->first->getCrossroads());

                Street * kruispunt = it->first;
                int at_pos = it->second;
                if(at_pos == fStreets[s]->getLength()){
                    return;
                }
                if(pos_veh == at_pos && !veh->hasTurned()){
                    std::cout << "DRIVING (" << pos_veh << ", " << str_veh << ", " << veh << ") ---> (" << at_pos << ", " << kruispunt->getName() << ", " << pos_new<<")" << std::endl;

                    if(rand() % 2){

                        std::string type = veh->getType();

                        Vehicle * newVehicle;
                        newVehicle = fStreets[s]->CreateTypeVehicle(type,kruispunt->getName(),pos_new);
                        newVehicle->setTurn(true);

                        it->first->addVehicle(newVehicle);
                        it->first->sortVehicles();

                        std::cout << "C H O S E N - " << veh  <<std::endl<<std::endl;
                        fStreets[s]->removeVehicleAddress(veh,v);
                        break;
                    }
                    else{
                        std::cout << "S K I P P E D - " << veh <<std::endl<<std::endl;
                        veh->setTurn(true);
                        continue;
                    }
                }
                else if(pos_veh != at_pos && veh->hasTurned() && !contains(pos_veh,kruispunten)){
                    veh->setTurn(false);
                }
            }
        }
    }
}

int TrafficSimulation::findPosition(Street * street, std::map<Street *, int> kruispunten) {
    for (std::map<Street*, int>::iterator it = kruispunten.begin(); it != kruispunten.end(); it++){
        if(it->first->getName() == street->getName()){
            return it->second;
        }
    }
    return -1;
}
bool TrafficSimulation::contains(int pos_veh, std::map<Street *, int> kruispunten) {
    bool exists = false;
    for (std::map<Street*, int>::iterator it = kruispunten.begin(); it != kruispunten.end(); it++){
        if(it->second == pos_veh){
            exists = true;
        }
    }
    return exists;
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

void TrafficSimulation::createImage(unsigned int i, const std::string &size) {
    REQUIRE(properlyInitialized(), "TrafficSimulation wasn't initialized when calling createImage()");

    std::ostringstream convert;
    convert << i+1;
    std::string fileName = "images/image" + convert.str() + ".ini";

    unsigned int amountOfFigures = 0;
    for (unsigned int k = 0; k < fStreets.size(); k++) {
        amountOfFigures += 1;
        amountOfFigures += fStreets[k]->getVehicles().size();
        amountOfFigures += fStreets[k]->getTrafficLights().size();
        amountOfFigures += fStreets[k]->getBusStops().size();
    }
    std::ostringstream convert2;
    convert2 << amountOfFigures;

    std::ofstream ini;
    ini.open(fileName.c_str());

    ini << "[General]" << std::endl
    << "size = " << size << std::endl
    << "backgroundcolor = (1, 1, 1)" << std::endl
    << "type = \"LightedZBuffering\"" << std::endl
    << "nrLights = 1" << std::endl
    << "eye = (0, 200, -200)" << std::endl
    << "nrFigures = " << convert2.str() << std::endl << std::endl

    << "[Light0]" << std::endl
    << "ambientLight = (1, 1, 1)" << std::endl << std::endl;

    unsigned int figureNumber = 0;
    for (unsigned int k = 0; k < fStreets.size(); k++) {
        std::ostringstream convert3;
        convert3 << figureNumber;

        int centerOffset = k*20;
        std::ostringstream convert4;
        convert4 << centerOffset;
        std::string center = "(0, " + convert4.str() + ", 0)";

        ini << "[Figure" << convert3.str() << "]" << std::endl
        << "type = \"Road\"" << std::endl
        << "scale = 1" << std::endl
        << "rotateX = 0" << std::endl
        << "rotateY = 0" << std::endl
        << "rotateZ = 0" << std::endl
        << "center = " << center << std::endl
        << "ambientReflection = (0.20, 0.20, 0.20)" << std::endl
        << "length = " << fStreets[k]->getLength()+10 << std::endl << std::endl;

        figureNumber += 1;

        std::vector<Vehicle*> vehicles = fStreets[k]->getVehicles();
        for (unsigned int l = 0; l < vehicles.size(); l++) {
            std::ostringstream convert5;
            convert5 << figureNumber;

            int vehicleOffset = k * 20 + 4;
            std::ostringstream convert6;
            convert6 << vehicleOffset;

            std::ostringstream convert7;
            convert7 << fStreets[k]->getLength()/2 - vehicles[l]->getPosition();

            std::string centerVehicle = "(" + convert7.str() + ", " + convert6.str() + ", 0)";

            std::string color;
            std::string type = vehicles[l]->getType();
            if (type == "Ambulance") {
                color = "(1.00, 1.00, 1.00)";
            } else if (type == "Bus") {
                color = "(0.00, 1.00, 0.00)";
            } else if (type == "Car") {
                color = "(0.00, 0.00, 0.00)";
            } else if (type == "FireEngine") {
                color = "(1.00, 0.00, 0.00)";
            } else {
                color = "(0.00, 0.00, 1.00)";
            }

            ini << "[Figure" << convert5.str() << "]" << std::endl
            << "type = \"Cube\"" << std::endl
            << "scale = 1" << std::endl
            << "rotateX = 0" << std::endl
            << "rotateY = 0" << std::endl
            << "rotateZ = 0" << std::endl
            << "center = " << centerVehicle << std::endl
            << "ambientReflection = " << color << std::endl << std::endl;

            figureNumber += 1;
        }

        std::vector<TrafficLight*> trafficLights = fStreets[k]->getTrafficLights();
        for (unsigned int l = 0; l < trafficLights.size(); l++) {
            std::ostringstream convert8;
            convert8 << figureNumber;

            int trafficLightOffset = k * 20 + 10;
            std::ostringstream convert9;
            convert9 << trafficLightOffset;

            std::ostringstream convert10;
            convert10 << fStreets[k]->getLength()/2 - trafficLights[l]->getPosition();

            std::string trafficLightCenter = "(" + convert10.str() + ", " + convert9.str() + ", 0)";

            std::string color;
            if (trafficLights[l]->isGreen()) {
                color = "(0, 1, 0)";
            } else {
                color = "(1, 0, 0)";
            }

            ini << "[Figure" << convert8.str() << "]" << std::endl
                << "type = \"Cube\"" << std::endl
                << "scale = 1" << std::endl
                << "rotateX = 0" << std::endl
                << "rotateY = 0" << std::endl
                << "rotateZ = 0" << std::endl
                << "center = " << trafficLightCenter << std::endl
                << "ambientReflection = " << color << std::endl << std::endl;

            figureNumber += 1;
        }

        std::vector<BusStop*> busStops = fStreets[k]->getBusStops();
        for (unsigned int l = 0; l < busStops.size(); l++) {
            std::ostringstream convert11;
            convert11 << figureNumber;

            int busStopOffset = k * 20 - 3;
            std::ostringstream convert12;
            convert12 << busStopOffset;

            std::ostringstream convert13;
            convert13 << fStreets[k]->getLength() / 2 - busStops[l]->getPosition();

            std::string busStopCenter = "(" + convert13.str() + ", " + convert12.str() + ", 0)";

            ini << "[Figure" << convert11.str() << "]" << std::endl
                << "type = \"Cube\"" << std::endl
                << "scale = 1" << std::endl
                << "rotateX = 0" << std::endl
                << "rotateY = 0" << std::endl
                << "rotateZ = 0" << std::endl
                << "center = " << busStopCenter << std::endl
                << "ambientReflection = (0, 0, 1)" << std::endl << std::endl;

            figureNumber += 1;
        }
    }
    ini.close();

    std::string command = "./engine " + fileName;
    system(command.c_str());
}

Street *TrafficSimulation::getStreetFromString(const std::string &name) const {
    for (long unsigned int i = 0; i < fStreets.size(); i++) {
        if (fStreets[i]->getName() == name) {
            return fStreets[i];
        }
    }
    return NULL;
}
