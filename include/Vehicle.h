#ifndef VEHICLE_H
#define VEHICLE_H

#include <string>

class Vehicle {
public:
    Vehicle(std::string plateNumber, std::string type);

    std::string getPlateNumber() const;
    std::string getType() const;

private:
    std::string plateNumber_;
    std::string type_;
};

#endif
