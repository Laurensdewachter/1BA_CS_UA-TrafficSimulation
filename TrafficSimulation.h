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
#include <fstream>
#include "DesignByContract.h"
#include "parsers/ElementParser.h"

class Street;

class TrafficSimulation {
    TrafficSimulation* _initCheck;

    std::vector<Street*> fStreets;
    double fTime;

public:
    /*
     * ENSURE(properlyInitialized(), "TrafficSimulation constructor did not end in an initialized state")
     */
    TrafficSimulation();

    virtual ~TrafficSimulation();

    bool properlyInitialized() const;

    /*
     * REQUIRE(properlyInitialized(), "TrafficSimulation wasn't initialized when calling parseInputFile()")
     * REQUIRE(errStream.good(), "The errorStream wasn't good when calling parseInputFile()")
     * ENSURE(errStream.good(), "The errorStream wasn't good at the end of parseInputFile()")
     * ENSURE(parseSucces == Success || parseSucces == ImportAborted, "The parser did not return a proper succes or aborted value")
     */
    EParserSucces parseInputFile(const std::string &filename, std::ostream &errStream = std::cerr);

    /*
     * REQUIRE(properlyInitialized(), "TrafficSimulation wasn't initialized when calling writeOn()")
     * REQUIRE(onstream.good(), "The outputStream wasn't good when calling writeOn()")
     * ENSURE(onstream.good(), "The outputStream wasn't good at the end of writeOn()")
     */
    void writeOn(std::ostream &onstream = std::cout) const;

    /*
     * REQUIRE(properlyInitialized(), "TrafficSimulation wasn't initialized when calling visualize()")
     * REQUIRE(onstream.good(), "The outputStream wasn't good when calling visualize()")
     * ENSURE(onstream.good(), "The outputStream wasn't good at the end of visualize()")
     */
    void visualize(std::ostream &onstream = std::cout) const;

    /*
     * REQUIRE(properlyInitialized(), "TrafficSimulation wasn't initialized when calling simulate()")
     */
    void simulate();

    /*
     * REQUIRE(properlyInitialized(), "TrafficSimulation wasn't initialized when calling clearSimulation()")
     * ENSURE(fStreets.empty(), "The streets vector wasn't empty at the end of clearSimulation()")
     */
    void clearSimulation();

private:
    Street* getStreet(const std::string &name) const;
};


#endif