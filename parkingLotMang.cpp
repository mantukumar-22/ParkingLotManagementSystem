#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <cmath>
#include <limits>
#include <algorithm>

using namespace std;

class Vehicle {
protected:
    string registration;
    time_t entryTime;
    int slotNumber;

public:
    Vehicle(const string& reg, time_t entry, int slot)
        : registration(reg), entryTime(entry), slotNumber(slot) {}
    virtual ~Vehicle() = default;

    string getRegistration() const { return registration; }
    time_t getEntryTime() const { return entryTime; }
    int getSlotNumber() const { return slotNumber; }

    virtual string getType() const = 0;
    virtual double hourlyRate() const = 0;
};

class Car : public Vehicle {
public:
    using Vehicle::Vehicle;
    string getType() const override { return "Car"; }
    double hourlyRate() const override { return 30.0; }
};

class Bike : public Vehicle {
public:
    using Vehicle::Vehicle;
    string getType() const override { return "Bike"; }
    double hourlyRate() const override { return 15.0; }
};

class Truck : public Vehicle {
public:
    using Vehicle::Vehicle;
    string getType() const override { return "Truck"; }
    double hourlyRate() const override { return 50.0; }
};

class ParkingLot {
private:
    vector<unique_ptr<Vehicle>> vehicles;
    const int capacity = 20;
    const string dataFile = "parking_records.txt";

    unique_ptr<Vehicle> createVehicle(const string& type,
                                      const string& reg,
                                      time_t entry,
                                      int slot) {
        if (type == "Car") return make_unique<Car>(reg, entry, slot);
        if (type == "Bike") return make_unique<Bike>(reg, entry, slot);
        if (type == "Truck") return make_unique<Truck>(reg, entry, slot);
        return nullptr;
    }

    int findFreeSlot() const {
        for (int slot = 1; slot <= capacity; ++slot) {
            bool occupied = false;
            for (const auto& vehicle : vehicles) {
                if (vehicle->getSlotNumber() == slot) {
                    occupied = true;
                    break;
                }
            }
            if (!occupied) return slot;
        }
        return -1;
    }

    Vehicle* findVehicle(const string& reg) const {
        for (const auto& vehicle : vehicles) {
            if (vehicle->getRegistration() == reg) return vehicle.get();
        }
        return nullptr;
    }

    void saveRecords() const {
        ofstream out(dataFile);
        if (!out) {
            cerr << "Warning: Could not save parking records.\n";
            return;
        }

        for (const auto& vehicle : vehicles) {
            out << vehicle->getType() << '|'
                << vehicle->getRegistration() << '|'
                << static_cast<long long>(vehicle->getEntryTime()) << '|'
                << vehicle->getSlotNumber() << '\n';
        }
    }

    void loadRecords() {
        ifstream in(dataFile);
        string line;

        while (getline(in, line)) {
            stringstream ss(line);
            string type, reg, entryText, slotText;

            if (!getline(ss, type, '|') ||
                !getline(ss, reg, '|') ||
                !getline(ss, entryText, '|') ||
                !getline(ss, slotText, '|')) {
                continue;
            }

            try {
                time_t entry = static_cast<time_t>(stoll(entryText));
                int slot = stoi(slotText);

                if (slot < 1 || slot > capacity || findVehicle(reg)) continue;
                auto vehicle = createVehicle(type, reg, entry, slot);
                if (vehicle && vehicles.size() < static_cast<size_t>(capacity)) {
                    vehicles.push_back(move(vehicle));
                }
            } catch (const exception&) {
                // Ignore invalid/corrupted lines.
            }
        }
    }

    static string currentTimeString(time_t timeValue) {
        tm* timeInfo = localtime(&timeValue);
        if (!timeInfo) return "Unknown";
        stringstream ss;
        ss << put_time(timeInfo, "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

public:
    ParkingLot() {
        loadRecords();
    }

    void parkVehicle() {
        if (vehicles.size() >= static_cast<size_t>(capacity)) {
            cout << "Parking lot is full.\n";
            return;
        }

        string reg;
        cout << "Enter vehicle registration number: ";
        getline(cin, reg);

        if (reg.empty()) {
            cout << "Registration number cannot be empty.\n";
            return;
        }

        if (findVehicle(reg)) {
            cout << "This vehicle is already parked.\n";
            return;
        }

        cout << "Select vehicle type (1. Car  2. Bike  3. Truck): ";
        int choice;
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input.\n";
            return;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        string type;
        if (choice == 1) type = "Car";
        else if (choice == 2) type = "Bike";
        else if (choice == 3) type = "Truck";
        else {
            cout << "Invalid vehicle type.\n";
            return;
        }

        int slot = findFreeSlot();
        if (slot == -1) {
            cout << "No parking slot available.\n";
            return;
        }

        auto vehicle = createVehicle(type, reg, time(nullptr), slot);
        vehicles.push_back(move(vehicle));
        saveRecords();

        cout << "Vehicle parked successfully. Slot: " << slot << '\n';
    }

    void removeVehicle() {
        string reg;
        cout << "Enter registration number: ";
        getline(cin, reg);

        auto it = find_if(vehicles.begin(), vehicles.end(),
            [&](const unique_ptr<Vehicle>& v) {
                return v->getRegistration() == reg;
            });

        if (it == vehicles.end()) {
            cout << "Vehicle not found.\n";
            return;
        }

        Vehicle& vehicle = **it;
        time_t exitTime = time(nullptr);
        double hours = max(1.0, ceil(difftime(exitTime, vehicle.getEntryTime()) / 3600.0));
        double fee = hours * vehicle.hourlyRate();

        cout << fixed << setprecision(2);
        cout << "\n--- Parking Receipt ---\n";
        cout << "Vehicle: " << vehicle.getType() << '\n';
        cout << "Registration: " << vehicle.getRegistration() << '\n';
        cout << "Slot: " << vehicle.getSlotNumber() << '\n';
        cout << "Entry: " << currentTimeString(vehicle.getEntryTime()) << '\n';
        cout << "Exit: " << currentTimeString(exitTime) << '\n';
        cout << "Charged hours: " << hours << '\n';
        cout << "Total fee: Rs. " << fee << '\n';

        vehicles.erase(it);
        saveRecords();
    }

    void displayVehicles() const {
        if (vehicles.empty()) {
            cout << "No vehicles currently parked.\n";
            return;
        }

        cout << "\n--- Parked Vehicles ---\n";
        cout << left << setw(10) << "Slot"
             << setw(12) << "Type"
             << setw(20) << "Registration"
             << "Entry Time\n";

        for (const auto& vehicle : vehicles) {
            cout << left << setw(10) << vehicle->getSlotNumber()
                 << setw(12) << vehicle->getType()
                 << setw(20) << vehicle->getRegistration()
                 << currentTimeString(vehicle->getEntryTime()) << '\n';
        }
    }

    void searchVehicle() const {
        string reg;
        cout << "Enter registration number: ";
        getline(cin, reg);

        Vehicle* vehicle = findVehicle(reg);
        if (!vehicle) {
            cout << "Vehicle not found.\n";
            return;
        }

        cout << "Vehicle found: " << vehicle->getType()
             << ", Slot: " << vehicle->getSlotNumber()
             << ", Entry: " << currentTimeString(vehicle->getEntryTime())
             << '\n';
    }

    void showStatus() const {
        cout << "Total slots: " << capacity << '\n';
        cout << "Occupied slots: " << vehicles.size() << '\n';
        cout << "Available slots: " << capacity - vehicles.size() << '\n';
    }
};

int main() {
    ParkingLot parkingLot;

    while (true) {
        cout << "\n===== Parking Lot Management System =====\n";
        cout << "1. Park vehicle\n";
        cout << "2. Remove vehicle / Checkout\n";
        cout << "3. Display parked vehicles\n";
        cout << "4. Search vehicle\n";
        cout << "5. Parking lot status\n";
        cout << "0. Exit\n";
        cout << "Choose option: ";

        int choice;
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Please enter a valid number.\n";
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1: parkingLot.parkVehicle(); break;
            case 2: parkingLot.removeVehicle(); break;
            case 3: parkingLot.displayVehicles(); break;
            case 4: parkingLot.searchVehicle(); break;
            case 5: parkingLot.showStatus(); break;
            case 0:
                cout << "Thank you for using the Parking Lot System.\n";
                return 0;
            default:
                cout << "Invalid option. Try again.\n";
        }
    }
}
