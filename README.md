## Parking Lot Management System
A console-based Parking Lot Management System built with C++17 and
object-oriented programming. The application manages vehicle parking,
slot allocation, vehicle lookup, checkout, parking fee calculation, and
persistence of currently parked vehicles using a text file.
## Features
- Park cars, bikes, and trucks
- Automatically allocate an available parking slot
- Prevent duplicate registration numbers while a vehicle is parked
- Check out a vehicle and generate a parking receipt
- Calculate parking fees based on vehicle type and parked duration
- Display all currently parked vehicles
- Search for a vehicle by registration number
- View total, occupied, and available parking slots
- Save and reload parked vehicle records using file handling
## Technologies and Concepts
- C++17
- Object-Oriented Programming (OOP)
- Inheritance and runtime polymorphism
- Encapsulation and abstraction
- STL: vector
- Smart pointers: unique_ptr
- File handling
- Exception handling
- Time and date handling
## Vehicle Parking Rates
  Vehicle     Rate per hour
  Bike                  ₹15
Car                   ₹30
Truck                 ₹50

The system charges at least one hour. Any partial hour is rounded up
to the next whole hour.

For example, a car parked for 2.5 hours is charged for 3 hours:
3 × ₹30 = ₹90.
## Requirements
You need a C++ compiler that supports C++17, such as:
- GCC / MinGW-w64 with C++17 support
- Clang with C++17 support
## Project Setup
1. Download or clone this repository.
2. Place ParkingLotManagement.cpp in your project folder.
3. Open a terminal in that folder.
## Compile
g++ -std=c++17 ParkingLotManagement.cpp -o parking
Run on Windows
.\parking.exe
Run on Linux or macOS
./parking
Menu
When the application starts, you will see the following menu:
===== Parking Lot Management System =====
1. Park vehicle
2. Remove vehicle / Checkout
3. Display parked vehicles
4. Search vehicle
5. Parking lot status
0. Exit
Choose option:
Select an option by entering its number.
Data Persistence
The application stores active parking records in:
parking_records.txt
The file is created in the current working directory when the
application saves its first record. On startup, the program attempts to
load the saved records.

Each record contains the vehicle type, registration number, entry
timestamp, and slot number, separated by |.
Keep the data file in the same working directory when you want the
application to reload its saved records.

## Current Limitations
- The parking capacity is fixed at 20 slots.
- Records are stored in a plain text file rather than a database.
- This is a terminal application; it does not include a GUI or web
  interface.
- Parking rates are fixed in the source code.
- Payment is calculated and displayed; no online payment integration
  is included.
## Future Improvements
- Separate the classes into header and source files
- Add unit tests
- Configure capacity and rates through a settings file
- Add a database such as SQLite or PostgreSQL
- Add a graphical or web-based interface
- Add parking history and reports
- Add automated build configuration using CMake