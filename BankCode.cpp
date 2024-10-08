#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>

using namespace std;

// Base class for Account
class Account {
protected:
    int accountNumber;
    double balance;

public:
    Account(int accNum, double bal) : accountNumber(accNum), balance(bal) {}

    virtual ~Account() {}

    int getAccountNumber() const { return accountNumber; }
    double getBalance() const { return balance; }

    virtual void deposit(double amount) {
        if (amount <= 0) throw invalid_argument("Deposit amount must be positive.");
        balance += amount;
    }

    virtual void withdraw(double amount) {
        if (amount <= 0) throw invalid_argument("Withdrawal amount must be positive.");
        if (amount > balance) throw runtime_error("Insufficient funds.");
        balance -= amount;
    }

    virtual string getAccountType() const = 0; // Pure virtual function for account type

    virtual void display() const {
        cout << "Account Number: " << accountNumber << ", Balance: $" << balance << endl;
    }
};

// Derived class for Savings Account
class SavingsAccount : public Account {
public:
    SavingsAccount(int accNum, double bal) : Account(accNum, bal) {}

    string getAccountType() const override { return "Savings"; }
};

// Derived class for Current Account
class CurrentAccount : public Account {
public:
    CurrentAccount(int accNum, double bal) : Account(accNum, bal) {}

    string getAccountType() const override { return "Current"; }
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

// Transaction class
class Transaction {
private:
    int accountNumber;
    string type;
    double amount;

public:
    // Constructor
    Transaction(int accNum, const string& t, double amt)
        : accountNumber(accNum), type(t), amount(amt) {}

    // Getter for accountNumber
    int getAccountNumber() const { return accountNumber; }

    // Getter for type
    string getType() const { return type; }

    // Getter for amount
    double getAmount() const { return amount; }

    // Method to display transaction details
    void display() const {
        cout << "Account Number: " << accountNumber
             << ", Type: " << type
             << ", Amount: $" << amount << endl;
    }
};


// Bank class
class Bank {
private:
    vector<Account*> accounts;
    vector<Transaction*> transactions;

    void saveData();
    void loadData();

public:
    Bank() { loadData(); }
    ~Bank();

    void createAccount(Account* account);
    void deposit(int accountNumber, double amount);
    void withdraw(int accountNumber, double amount);
    void transfer(int fromAccount, int toAccount, double amount);
    void displayAccounts() const;
    void displayTransactions() const;
};

// Destructor to clean up dynamically allocated memory
Bank::~Bank() {
    for (size_t i = 0; i < accounts.size(); ++i) delete accounts[i];
    for (size_t i = 0; i < transactions.size(); ++i) delete transactions[i];
}

// Create a new account
void Bank::createAccount(Account* account) {
    accounts.push_back(account);
    saveData();
}

// Deposit money into an account
void Bank::deposit(int accountNumber, double amount) {
    for (size_t i = 0; i < accounts.size(); ++i) {
        if (accounts[i]->getAccountNumber() == accountNumber) {
            accounts[i]->deposit(amount);
            transactions.push_back(new Transaction(accountNumber, "Deposit", amount));
            saveData();
            return;
        }
    }
    throw runtime_error("Account not found.");
}

// Withdraw money from an account
void Bank::withdraw(int accountNumber, double amount) {
    for (size_t i = 0; i < accounts.size(); ++i) {
        if (accounts[i]->getAccountNumber() == accountNumber) {
            accounts[i]->withdraw(amount);
            transactions.push_back(new Transaction(accountNumber, "Withdrawal", amount));
            saveData();
            return;
        }
    }
    throw runtime_error("Account not found.");
}

// Transfer money between accounts
void Bank::transfer(int fromAccount, int toAccount, double amount) {
    Account* from = nullptr;
    Account* to = nullptr;

    for (size_t i = 0; i < accounts.size(); ++i) {
        if (accounts[i]->getAccountNumber() == fromAccount) {
            from = accounts[i];
        }
        if (accounts[i]->getAccountNumber() == toAccount) {
            to = accounts[i];
        }
    }

    if (!from) throw runtime_error("From account not found.");
    if (!to) throw runtime_error("To account not found.");

    from->withdraw(amount);
    to->deposit(amount);

    transactions.push_back(new Transaction(fromAccount, "Transfer Out", amount));
    transactions.push_back(new Transaction(toAccount, "Transfer In", amount));
    saveData();
}

// Display all accounts
void Bank::displayAccounts() const {
    for (size_t i = 0; i < accounts.size(); ++i) {
        accounts[i]->display();
    }
}

// Display all transactions
void Bank::displayTransactions() const {
    for (size_t i = 0; i < transactions.size(); ++i) {
        transactions[i]->display();
    }
}

// Save data to a file
void Bank::saveData() {
    ofstream accountFile("accounts.txt");
    ofstream transactionFile("transactions.txt");

    for (size_t i = 0; i < accounts.size(); ++i) {
        accountFile << accounts[i]->getAccountNumber() << "|" << accounts[i]->getBalance() << "|" << accounts[i]->getAccountType() << endl;
    }

    for (size_t i = 0; i < transactions.size(); ++i) {
        transactionFile << transactions[i]->getAccountNumber() << "|" << transactions[i]->getType() << "|" << transactions[i]->getAmount() << endl;
    }
}

// Load data from a file
void Bank::loadData() {
    ifstream accountFile("accounts.txt");
    ifstream transactionFile("transactions.txt");

    int accNum;
    double bal;
    string type;

    while (accountFile >> accNum) {
        accountFile.ignore();
        accountFile >> bal;
        accountFile.ignore();
        getline(accountFile, type);

        Account* account = nullptr;
        if (type == "Savings") {
            account = new SavingsAccount(accNum, bal);
        } else if (type == "Current") {
            account = new CurrentAccount(accNum, bal);
        }
        if (account) createAccount(account);
    }

    int tAccNum;
    string tType;
    double tAmount;

    while (transactionFile >> tAccNum) {
        transactionFile.ignore();
        getline(transactionFile, tType, '|');
        transactionFile >> tAmount;
        transactionFile.ignore();

        transactions.push_back(new Transaction(tAccNum, tType, tAmount));
    }
}

// Simple user interface
void userInterface(Bank& bank) {
    int choice;
    while (true) {
        cout << "\nBanking System\n";
        cout << "1. Create Account\n";
        cout << "2. Deposit Money\n";
        cout << "3. Withdraw Money\n";
        cout << "4. Transfer Money\n";
        cout << "5. Display All Accounts\n";
        cout << "6. Display All Transactions\n";
        cout << "7. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                int num, type;
                double bal;
                cout << "Enter account number: ";
                cin >> num;
                cout << "Enter balance: ";
                cin >> bal;
                cout << "Enter account type (1: Savings, 2: Current): ";
                cin >> type;
                Account* account = nullptr;
                switch (type) {
                    case 1: account = new SavingsAccount(num, bal); break;
                    case 2: account = new CurrentAccount(num, bal); break;
                    default: cout << "Invalid type." << endl; break;
                }
                if (account) bank.createAccount(account);
                break;
            }
            case 2: {
                int num;
                double amount;
                cout << "Enter account number: ";
                cin >> num;
                cout << "Enter amount to deposit: ";
                cin >> amount;
                try {
                    bank.deposit(num, amount);
                } catch (const runtime_error& e) {
                    cout << "Error: " << e.what() << endl;
                }
                break;
            }
            case 3: {
                int num;
                double amount;
                cout << "Enter account number: ";
                cin >> num;
                cout << "Enter amount to withdraw: ";
                cin >> amount;
                try {
                    bank.withdraw(num, amount);
                } catch (const runtime_error& e) {
                    cout << "Error: " << e.what() << endl;
                }
                break;
            }
            case 4: {
                int from, to;
                double amount;
                cout << "Enter from account number: ";
                cin >> from;
                cout << "Enter to account number: ";
                cin >> to;
                cout << "Enter amount to transfer: ";
                cin >> amount;
                try {
                    bank.transfer(from, to, amount);
                } catch (const runtime_error& e) {
                    cout << "Error: " << e.what() << endl;
                }
                break;
            }
            case 5:
                bank.displayAccounts();
                break;
            case 6:
                bank.displayTransactions();
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
    Bank bank;

    // Simple user interface to interact with the banking system
    userInterface(bank);

    return 0;
}
