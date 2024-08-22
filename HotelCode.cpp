#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <fstream>
#include <sstream>

using namespace std;

// Base class for Room
class Room {
protected:
    int roomNumber;
    bool isAvailable;

public:
    Room(int num) : roomNumber(num), isAvailable(true) {}
    virtual ~Room() {}

    int getRoomNumber() const { return roomNumber; }
    bool getAvailability() const { return isAvailable; }
    void setAvailability(bool availability) { isAvailable = availability; }

    virtual string getRoomType() const = 0; // Pure virtual function for room type

    void display() const {
        cout << getRoomType() << " Room Number: " << roomNumber << ", Available: " << (isAvailable ? "Yes" : "No") << endl;
    }
};

// Derived classes for different room types
class SingleRoom : public Room {
public:
    SingleRoom(int num) : Room(num) {}
    string getRoomType() const override { return "Single"; }
};

class DoubleRoom : public Room {
public:
    DoubleRoom(int num) : Room(num) {}
    string getRoomType() const override { return "Double"; }
};

class SuiteRoom : public Room {
public:
    SuiteRoom(int num) : Room(num) {}
    string getRoomType() const override { return "Suite"; }
};

// Customer class
class Customer {
private:
    string name;
    int customerID;

public:
    Customer(const string& n, int id) : name(n), customerID(id) {}
    string getName() const { return name; }
    int getCustomerID() const { return customerID; }

    void display() const {
        cout << "Customer Name: " << name << ", ID: " << customerID << endl;
    }
};

// Booking class
class Booking {
private:
    Room* room;
    Customer* customer;

public:
    Booking(Room* r, Customer* c) : room(r), customer(c) {}
    Room* getRoom() const { return room; }
    Customer* getCustomer() const { return customer; }
};

// Hotel class with file I/O
class Hotel {
private:
    vector<Room*> rooms;
    vector<Booking*> bookings;

    void saveRooms() const;
    void loadRooms();
    void saveBookings() const;
    void loadBookings();

public:
    ~Hotel();
    void addRoom(Room* room);
    void bookRoom(int roomNumber, Customer* customer);
    void cancelBooking(int roomNumber);
    void checkAvailability(int roomNumber) const;
    void displayRooms() const;
    void displayBookings() const;
    void saveData() const;
    void loadData();
};

Hotel::~Hotel() {
    for (auto room : rooms) delete room;
    for (auto booking : bookings) delete booking;
}

void Hotel::addRoom(Room* room) {
    rooms.push_back(room);
}

void Hotel::bookRoom(int roomNumber, Customer* customer) {
    for (auto room : rooms) {
        if (room->getRoomNumber() == roomNumber) {
            if (room->getAvailability()) {
                room->setAvailability(false);
                bookings.push_back(new Booking(room, customer));
                cout << "Room " << roomNumber << " has been booked for " << customer->getName() << endl;
                saveBookings();
                return;
            } else {
                throw runtime_error("Room is not available.");
            }
        }
    }
    throw runtime_error("Room not found.");
}

void Hotel::cancelBooking(int roomNumber) {
    for (auto it = bookings.begin(); it != bookings.end(); ++it) {
        if ((*it)->getRoom()->getRoomNumber() == roomNumber) {
            (*it)->getRoom()->setAvailability(true);
            delete *it;
            bookings.erase(it);
            cout << "Booking for room " << roomNumber << " has been canceled." << endl;
            saveBookings();
            return;
        }
    }
    throw runtime_error("Booking not found.");
}

void Hotel::checkAvailability(int roomNumber) const {
    for (auto room : rooms) {
        if (room->getRoomNumber() == roomNumber) {
            cout << "Room " << roomNumber << " is " << (room->getAvailability() ? "available" : "not available") << endl;
            return;
        }
    }
    cout << "Room not found." << endl;
}

void Hotel::displayRooms() const {
    for (auto room : rooms) {
        room->display();
    }
}

void Hotel::displayBookings() const {
    for (auto booking : bookings) {
        cout << "Room: " << booking->getRoom()->getRoomNumber() << ", Customer: " << booking->getCustomer()->getName() << endl;
    }
}

void Hotel::saveRooms() const {
    ofstream roomFile("rooms.txt");
    for (auto room : rooms) {
        roomFile << room->getRoomNumber() << " " << room->getRoomType() << " " << room->getAvailability() << endl;
    }
}

void Hotel::loadRooms() {
    ifstream roomFile("rooms.txt");
    int roomNumber;
    string roomType;
    bool availability;
    while (roomFile >> roomNumber >> roomType >> availability) {
        Room* room = nullptr;
        if (roomType == "Single") {
            room = new SingleRoom(roomNumber);
        } else if (roomType == "Double") {
            room = new DoubleRoom(roomNumber);
        } else if (roomType == "Suite") {
            room = new SuiteRoom(roomNumber);
        }
        if (room) {
            room->setAvailability(availability);
            addRoom(room);
        }
    }
}

void Hotel::saveBookings() const {
    ofstream bookingFile("bookings.txt");
    for (auto booking : bookings) {
        bookingFile << booking->getRoom()->getRoomNumber() << " " << booking->getCustomer()->getName() << " " << booking->getCustomer()->getCustomerID() << endl;
    }
}

void Hotel::loadBookings() {
    ifstream bookingFile("bookings.txt");
    int roomNumber;
    string customerName;
    int customerID;
    while (bookingFile >> roomNumber >> customerName >> customerID) {
        Room* bookedRoom = nullptr;
        Customer* customer = new Customer(customerName, customerID);

        // Find the room from the room list
        for (auto room : rooms) {
            if (room->getRoomNumber() == roomNumber) {
                bookedRoom = room;
                break;
            }
        }

        if (bookedRoom) {
            bookings.push_back(new Booking(bookedRoom, customer));
        } else {
            delete customer; // Clean up if no room found
        }
    }
}

void Hotel::saveData() const {
    saveRooms();
    saveBookings();
}

void Hotel::loadData() {
    loadRooms();
    loadBookings();
}

// Simple user interface
void userInterface(Hotel& hotel) {
    int choice;
    while (true) {
        cout << "\nHotel Booking System\n";
        cout << "1. Add Room\n";
        cout << "2. Book Room\n";
        cout << "3. Cancel Booking\n";
        cout << "4. Check Room Availability\n";
        cout << "5. Display All Rooms\n";
        cout << "6. Display All Bookings\n";
        cout << "7. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                int num, type;
                cout << "Enter room number: ";
                cin >> num;
                cout << "Enter room type (1: Single, 2: Double, 3: Suite): ";
                cin >> type;
                Room* room = nullptr;
                switch (type) {
                    case 1: room = new SingleRoom(num); break;
                    case 2: room = new DoubleRoom(num); break;
                    case 3: room = new SuiteRoom(num); break;
                    default: cout << "Invalid type." << endl; break;
                }
                if (room) hotel.addRoom(room);
                break;
            }
            case 2: {
                int num, id;
                string name;
                cout << "Enter room number: ";
                cin >> num;
                cout << "Enter customer name: ";
                cin >> name;
                cout << "Enter customer ID: ";
                cin >> id;
                Customer* customer = new Customer(name, id);
                try {
                    hotel.bookRoom(num, customer);
                } catch (const runtime_error& e) {
                    cout << "Error: " << e.what() << endl;
                }
                break;
            }
            case 3: {
                int num;
                cout << "Enter room number: ";
                cin >> num;
                try {
                    hotel.cancelBooking(num);
                } catch (const runtime_error& e) {
                    cout << "Error: " << e.what() << endl;
                }
                break;
            }
            case 4: {
                int num;
                cout << "Enter room number: ";
                cin >> num;
                hotel.checkAvailability(num);
                break;
            }
            case 5:
                hotel.displayRooms();
                break;
            case 6:
                hotel.displayBookings();
                break;
            case 7:
                return;
            default:
                cout << "Invalid choice." << endl;
                break;
        }
    }
}

int main() {
    Hotel hotel;

    hotel.loadData();  // Load data from files at the start

    // Simple user interface to interact with the hotel system
    userInterface(hotel);

    hotel.saveData();  // Save data to files before exiting

    return 0;
}

