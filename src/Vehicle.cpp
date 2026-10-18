#include "Vehicle.h"

Vehicle::Vehicle(std::string plateNumber, std::string type)
    : plateNumber_(std::move(plateNumber)),
      type_(std::move(type)) {}

std::string Vehicle::getPlateNumber() const {
    return plateNumber_;
}

std::string Vehicle::getType() const {
    return type_;
}
