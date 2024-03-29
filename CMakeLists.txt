cmake_minimum_required(VERSION 3.5)
project(TrafficSimulation)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_FLAGS "-Wall -Werror")
set(CMAKE_EXE_LINKER_FLAGS -pthread)

# Set include dir
include_directories(gtest/include)

# Set Library dir
link_directories(gtest/lib)

# Set source files for RELEASE target
set(RELEASE_SOURCE_FILES TrafficSimulationMain.cpp parsers/tinyxml/tinyxml.cpp parsers/tinyxml/tinyxmlparser.cpp
        parsers/tinyxml/tinyxmlerror.cpp parsers/tinyxml/tinyxmlerror.cpp parsers/tinyxml/tinystr.cpp
        parsers/ElementParser.cpp objects/Street.cpp objects/TrafficLight.cpp objects/Vehicle.cpp parsers/StreetParser.cpp
        parsers/TrafficLightParser.cpp parsers/VehicleParser.cpp parsers/VehicleGeneratorParser.cpp
        objects/VehicleGenerator.cpp TrafficSimulation.cpp objects/BusStop.cpp objects/BusStop.h parsers/BusStopParser.cpp parsers/BusStopParser.h
        parsers/CrossroadParser.cpp objects/Crossroad.cpp objects/Crossroad.h objects/vehicles/Car.cpp objects/vehicles/Car.h objects/vehicles/Bus.cpp objects/vehicles/Bus.h objects/vehicles/FireEngine.cpp objects/vehicles/FireEngine.h objects/vehicles/Ambulance.cpp objects/vehicles/Ambulance.h objects/vehicles/PoliceCar.cpp objects/vehicles/PoliceCar.h)

# Set source files for DEBUG target
set(DEBUG_SOURCE_FILES tests/TrafficSimulationTests.cpp parsers/tinyxml/tinyxml.cpp parsers/tinyxml/tinyxmlparser.cpp
        parsers/tinyxml/tinyxmlerror.cpp parsers/tinyxml/tinyxmlerror.cpp parsers/tinyxml/tinystr.cpp tests/TrafficSimulationInputTests.cpp
        Utils.cpp TrafficSimulation.cpp parsers/ElementParser.cpp parsers/VehicleParser.cpp parsers/VehicleGeneratorParser.cpp
        parsers/StreetParser.cpp parsers/TrafficLightParser.cpp objects/Street.cpp objects/TrafficLight.cpp objects/Vehicle.cpp
        objects/VehicleGenerator.cpp tests/TrafficSimulationOutputTests.cpp objects/BusStop.cpp objects/BusStop.h parsers/BusStopParser.cpp parsers/BusStopParser.h parsers/CrossroadParser.cpp objects/Crossroad.cpp objects/Crossroad.h objects/vehicles/Car.cpp objects/vehicles/Car.h objects/vehicles/Bus.cpp objects/vehicles/Bus.h objects/vehicles/FireEngine.cpp objects/vehicles/FireEngine.h objects/vehicles/Ambulance.cpp objects/vehicles/Ambulance.h objects/vehicles/PoliceCar.cpp objects/vehicles/PoliceCar.h tests/BusStopDomainTests.cpp tests/CrossroadDomainTests.cpp tests/StreetDomainTests.cpp tests/TrafficLightTests.cpp tests/VehicleDomainTests.cpp tests/VehicleGeneratorDomainTests.cpp)

# Create RELEASE target
add_executable(TrafficSimulation ${RELEASE_SOURCE_FILES})

# Create DEBUG target
add_executable(TrafficSimulationTests ${DEBUG_SOURCE_FILES})

# Link library
target_link_libraries(TrafficSimulationTests gtest)
