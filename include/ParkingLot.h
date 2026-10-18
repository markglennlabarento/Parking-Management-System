#ifndef PARKINGLOT_H
#define PARKINGLOT_H

#include "Vehicle.h"

#include <chrono>
#include <string>
#include <vector>

class ParkingLot {
public:
    explicit ParkingLot(int totalSpaces = 10);

    bool parkVehicle(const Vehicle& vehicle, std::string& ticketId, std::string& spotId);
    bool checkOutVehicle(const std::string& ticketId,
                         std::string& plateNumber,
                         std::string& spotId,
                         int& hours,
                         int& minutes,
                         double& fee);
    void viewAvailableSpaces() const;
    void viewParkedVehicles() const;
    int getAvailableSpaces() const;

private:
    struct ParkedVehicle {
        Vehicle vehicle;
        std::string ticketId;
        std::string spotId;
        std::chrono::system_clock::time_point entryTime;
    };

    void loadPersistedData();
    bool savePersistedData() const;

    std::vector<std::string> availableSpots_;
    std::vector<ParkedVehicle> parkedVehicles_;
    std::vector<std::string> ticketIds_;
    std::string dataFile_;
    int nextTicketNumber_;
};

#endif
