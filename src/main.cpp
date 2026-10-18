#include "ParkingLot.h"
#include "Vehicle.h"

#include <iostream>
#include <limits>
#include <string>

void printMenu() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "        PARKING MANAGEMENT SYSTEM" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "  1. Park a vehicle" << std::endl;
    std::cout << "  2. Check out a vehicle" << std::endl;
    std::cout << "  3. View available spaces" << std::endl;
    std::cout << "  4. View parked vehicles" << std::endl;
    std::cout << "  5. Exit" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "  Select an option: ";
}

std::string getVehicleTypeFromChoice(int choice) {
    switch (choice) {
        case 1: return "Motorcycle";
        case 2: return "Car";
        case 3: return "Truck";
        default: return "Car";
    }
}

std::string getTimeLabel() {
    // Simulated current time for consistent sample output
    return "10:37 PM";
}

int main() {
    ParkingLot parkingLot(10);
    int choice = 0;

    do {
        printMenu();
        std::cin >> choice;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "\n  Invalid input. Please enter a valid menu option." << std::endl;
            continue;
        }

        switch (choice) {
            case 1: {
                std::string plateNumber;
                int typeChoice = 0;

                std::cout << "\n";
                std::cout << "-------------- PARK VEHICLE ------------" << std::endl;
                std::cout << "  License plate: ";
                std::cin >> plateNumber;
                std::cout << "  Vehicle type (1. Motorcycle  2. Car  3. Truck): ";
                std::cin >> typeChoice;

                std::cout << "\n";
                Vehicle vehicle(plateNumber, getVehicleTypeFromChoice(typeChoice));
                std::string ticketId;
                std::string spotId;
                parkingLot.parkVehicle(vehicle, ticketId, spotId);
                std::cout << "\n";
                break;
            }
            case 2: {
                std::string ticketId;
                std::string plateNumber;
                std::string spotId;
                int hours = 0;
                int minutes = 0;
                double fee = 0.0;

                std::cout << "\n";
                std::cout << "  -------------- CHECK OUT ---------------" << std::endl;
                std::cout << "  Enter ticket ID: ";
                std::cin >> ticketId;

                if (parkingLot.checkOutVehicle(ticketId, plateNumber, spotId, hours, minutes, fee)) {
                    std::cout << "\n";
                    std::cout << "  License plate: " << plateNumber << std::endl;
                    std::cout << "  Parking spot: " << spotId << std::endl;
                    std::cout << "  Duration: " << hours << " hours " << minutes << " minutes" << std::endl;
                    std::cout << "  Parking fee: $" << fee << std::endl;
                    std::cout << "\n  Confirm payment? (Y/N): ";
                    char answer = 'N';
                    std::cin >> answer;

                    if (answer == 'Y' || answer == 'y') {
                        std::cout << "\n  Payment successful." << std::endl;
                        std::cout << "  Spot " << spotId << " is now available." << std::endl;
                    } else {
                        std::cout << "\n  Payment cancelled." << std::endl;
                    }
                }
                std::cout << "\n";
                break;
            }
            case 3:
                parkingLot.viewAvailableSpaces();
                std::cout << "\n";
                break;
            case 4:
                parkingLot.viewParkedVehicles();
                std::cout << "\n";
                break;
            case 5:
                std::cout << "\n  Exiting parking management system." << std::endl;
                break;
            default:
                std::cout << "\n  Invalid option. Please select a valid choice." << std::endl;
                break;
        }
    } while (choice != 5);

    return 0;
}
