# Parking Management System

A simple C++ parking management system that allows users to:

- park a vehicle
- check out a vehicle
- view available spaces
- view parked vehicles
- exit the program

## Project Structure

```text
Parking Management System/
├── include/
│   ├── Vehicle.h
│   └── ParkingLot.h
├── src/
│   ├── Vehicle.cpp
│   ├── ParkingLot.cpp
│   └── main.cpp
├── .vscode/
│   ├── settings.json
│   ├── launch.json
│   └── tasks.json
├── README.md
├── parking_management_system.exe
└── .gitignore
```

## Features

- Vehicle registration with plate number and type
- Simple parking slot tracking
- Available space checking
- Parked vehicle list view
- Vehicle checkout by plate number
- Automatically saves parked vehicles in `parking_data.txt` and restores them when the program starts
- Menu-driven console interface

## How to Build

Open a terminal in the project folder and run:

```bash
g++ -std=c++17 -I ./include ./src/main.cpp ./src/ParkingLot.cpp ./src/Vehicle.cpp -o parking_management_system.exe
```

## How to Run

```bash
./parking_management_system.exe
```

## Use Cases

1. Park a vehicle
2. Check out a vehicle
3. View available spaces
4. View parked vehicles
5. Exit

## Notes

This is a beginner-friendly parking management system built using OOP principles in C++.
