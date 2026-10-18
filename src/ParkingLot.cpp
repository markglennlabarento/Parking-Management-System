#include "ParkingLot.h"

#include <algorithm>
#include <cerrno>
#include <cmath>
#include <ctime>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace {
std::string getCurrentTimeString() {
    auto now = std::time(nullptr);
    std::tm localTime{};
    #if defined(_WIN32)
    localtime_s(&localTime, &now);
    #else
    localtime_r(&now, &localTime);
    #endif

    std::ostringstream oss;
    std::tm t = localTime;
    const int hour = t.tm_hour % 12 == 0 ? 12 : t.tm_hour % 12;
    const std::string suffix = t.tm_hour >= 12 ? "PM" : "AM";
    oss << hour << ":" << std::setw(2) << std::setfill('0') << t.tm_min << " " << suffix;
    return oss.str();
}

std::string generateTicketId(int counter) {
    std::ostringstream oss;
    oss << "T-" << std::setw(4) << std::setfill('0') << counter++;
    return oss.str();
}
}

ParkingLot::ParkingLot(int totalSpaces)
    : availableSpots_(),
      dataFile_("parking_data.txt"),
      nextTicketNumber_(1) {
    for (int i = 1; i <= totalSpaces; ++i) {
        availableSpots_.push_back("C-" + std::to_string(i));
    }
    loadPersistedData();
}

void ParkingLot::loadPersistedData() {
    errno = 0;
    std::FILE* savedDataFile = std::fopen(dataFile_.c_str(), "r");
    if (savedDataFile == nullptr) {
        if (errno != ENOENT) {
            std::cerr << "Unable to read saved parking data from " << dataFile_ << "." << std::endl;
        }
        return;
    }
    std::fclose(savedDataFile);

    std::ifstream input(dataFile_);
    if (!input) {
        std::cerr << "Unable to read saved parking data from " << dataFile_ << "." << std::endl;
        return;
    }

    std::string fileSignature;
    int fileVersion = 0;
    int savedNextTicketNumber = 0;
    if (!(input >> fileSignature >> fileVersion >> savedNextTicketNumber) ||
        fileSignature != "PARKING_DATA" || fileVersion != 1 || savedNextTicketNumber < 1) {
        std::cerr << "Saved parking data is invalid; starting with an empty parking lot." << std::endl;
        return;
    }

    std::vector<ParkedVehicle> loadedVehicles;
    std::vector<std::string> loadedTicketIds;
    std::vector<std::string> loadedSpotIds;
    bool validData = true;

    while (true) {
        input >> std::ws;
        if (input.peek() == std::char_traits<char>::eof()) {
            break;
        }

        std::string plateNumber;
        std::string type;
        std::string ticketId;
        std::string spotId;
        long long entryTimeMilliseconds = 0;
        if (!(input >> std::quoted(plateNumber)
                    >> std::quoted(type)
                    >> std::quoted(ticketId)
                    >> std::quoted(spotId)
                    >> entryTimeMilliseconds)) {
            validData = false;
            break;
        }

        const bool spotExists = std::find(availableSpots_.begin(), availableSpots_.end(), spotId)
                                != availableSpots_.end();
        const bool duplicateSpot = std::find(loadedSpotIds.begin(), loadedSpotIds.end(), spotId)
                                   != loadedSpotIds.end();
        const bool duplicateTicket = std::find(loadedTicketIds.begin(), loadedTicketIds.end(), ticketId)
                                     != loadedTicketIds.end();
        if (plateNumber.empty() || type.empty() || ticketId.empty() || !spotExists ||
            duplicateSpot || duplicateTicket || loadedVehicles.size() >= availableSpots_.size()) {
            validData = false;
            break;
        }

        const auto entryTime = std::chrono::system_clock::time_point(
            std::chrono::milliseconds(entryTimeMilliseconds));
        loadedVehicles.push_back({Vehicle(plateNumber, type), ticketId, spotId, entryTime});
        loadedTicketIds.push_back(ticketId);
        loadedSpotIds.push_back(spotId);
    }

    if (!validData) {
        std::cerr << "Saved parking data is invalid; starting with an empty parking lot." << std::endl;
        return;
    }

    parkedVehicles_ = std::move(loadedVehicles);
    ticketIds_ = std::move(loadedTicketIds);
    nextTicketNumber_ = savedNextTicketNumber;
    availableSpots_.erase(
        std::remove_if(availableSpots_.begin(), availableSpots_.end(),
                       [this](const std::string& spotId) {
                           return std::find_if(parkedVehicles_.begin(), parkedVehicles_.end(),
                                               [&spotId](const ParkedVehicle& vehicle) {
                                                   return vehicle.spotId == spotId;
                                               }) != parkedVehicles_.end();
                       }),
        availableSpots_.end());
}

bool ParkingLot::savePersistedData() const {
    std::ofstream output(dataFile_, std::ios::trunc);
    if (!output) {
        std::cerr << "Unable to save parking data to " << dataFile_ << "." << std::endl;
        return false;
    }

    output << "PARKING_DATA 1 " << nextTicketNumber_ << '\n';
    for (const auto& parked : parkedVehicles_) {
        const auto entryTimeMilliseconds =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                parked.entryTime.time_since_epoch()).count();
        output << std::quoted(parked.vehicle.getPlateNumber()) << ' '
               << std::quoted(parked.vehicle.getType()) << ' '
               << std::quoted(parked.ticketId) << ' '
               << std::quoted(parked.spotId) << ' '
               << entryTimeMilliseconds << '\n';
    }

    output.close();
    if (!output) {
        std::cerr << "Unable to save parking data to " << dataFile_ << "." << std::endl;
        return false;
    }
    return true;
}

bool ParkingLot::parkVehicle(const Vehicle& vehicle, std::string& ticketId, std::string& spotId) {
    if (availableSpots_.empty()) {
        std::cout << "Parking is full." << std::endl;
        return false;
    }

    for (const auto& parked : parkedVehicles_) {
        if (parked.vehicle.getPlateNumber() == vehicle.getPlateNumber()) {
            std::cout << "Vehicle with plate number " << vehicle.getPlateNumber() << " is already parked." << std::endl;
            return false;
        }
    }

    const std::string assignedSpot = availableSpots_.front();
    availableSpots_.erase(availableSpots_.begin());

    const int previousNextTicketNumber = nextTicketNumber_;
    ticketId = generateTicketId(nextTicketNumber_++);
    while (std::find(ticketIds_.begin(), ticketIds_.end(), ticketId) != ticketIds_.end()) {
        ticketId = generateTicketId(nextTicketNumber_++);
    }
    spotId = assignedSpot;

    ParkedVehicle record{vehicle, ticketId, spotId, std::chrono::system_clock::now()};
    parkedVehicles_.push_back(record);
    ticketIds_.push_back(ticketId);

    if (!savePersistedData()) {
        parkedVehicles_.pop_back();
        ticketIds_.pop_back();
        availableSpots_.insert(availableSpots_.begin(), assignedSpot);
        nextTicketNumber_ = previousNextTicketNumber;
        return false;
    }

    std::cout << "  Parking spot assigned: " << spotId << std::endl;
    std::cout << "  Entry time: " << getCurrentTimeString() << std::endl;
    std::cout << "  Ticket ID: " << ticketId << std::endl;
    std::cout << "\n  Vehicle parked successfully." << std::endl;
    return true;
}

bool ParkingLot::checkOutVehicle(const std::string& ticketId,
                                 std::string& plateNumber,
                                 std::string& spotId,
                                 int& hours,
                                 int& minutes,
                                 double& fee) {
    auto it = std::find_if(parkedVehicles_.begin(), parkedVehicles_.end(),
                           [&ticketId](const ParkedVehicle& item) {
                               return item.ticketId == ticketId;
                           });

    if (it == parkedVehicles_.end()) {
        std::cout << "  Ticket ID not found." << std::endl;
        return false;
    }

    const auto duration = std::chrono::duration_cast<std::chrono::minutes>(
        std::chrono::system_clock::now() - it->entryTime);

    const long totalMinutes = std::max<long>(1, duration.count());
    hours = static_cast<int>(totalMinutes / 60);
    minutes = static_cast<int>(totalMinutes % 60);
    const double hourlyRate = 10.0;
    fee = (hours * 60 + minutes) / 60.0 * hourlyRate;
    fee = std::ceil(fee * 100.0) / 100.0;

    plateNumber = it->vehicle.getPlateNumber();
    spotId = it->spotId;

    const auto parkedPosition = static_cast<std::size_t>(std::distance(parkedVehicles_.begin(), it));
    const ParkedVehicle checkedOutVehicle = *it;
    availableSpots_.push_back(spotId);
    parkedVehicles_.erase(it);
    ticketIds_.erase(std::remove(ticketIds_.begin(), ticketIds_.end(), ticketId), ticketIds_.end());
    if (!savePersistedData()) {
        parkedVehicles_.insert(parkedVehicles_.begin() + parkedPosition, checkedOutVehicle);
        availableSpots_.pop_back();
        ticketIds_.push_back(ticketId);
        return false;
    }
    return true;
}

void ParkingLot::viewAvailableSpaces() const {
    std::cout << "Available spaces:" << std::endl;
    if (availableSpots_.empty()) {
        std::cout << "No available spaces." << std::endl;
        return;
    }

    for (const auto& spot : availableSpots_) {
        std::cout << spot << std::endl;
    }
}

void ParkingLot::viewParkedVehicles() const {
    if (parkedVehicles_.empty()) {
        std::cout << "No parked vehicles." << std::endl;
        return;
    }

    std::cout << "Parked vehicles:" << std::endl;
    for (const auto& parked : parkedVehicles_) {
        std::cout << "- " << parked.vehicle.getPlateNumber() << " | "
                  << parked.vehicle.getType() << " | Spot " << parked.spotId
                  << " | Ticket " << parked.ticketId << std::endl;
    }
}

int ParkingLot::getAvailableSpaces() const {
    return static_cast<int>(availableSpots_.size());
}
