#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

using namespace std;

class Book {
public:
    Book(int id, const string& title, const string& author)
        : id(id), title(title), author(author), available(true) {}

    int getId() const { return id; }
    string getTitle() const { return title; }
    string getAuthor() const { return author; }
    bool isAvailable() const { return available; }
    void setAvailable(bool status) { available = status; }

private:
    int id;
    string title;
    string author;
    bool available;
};

class Member {
public:
    Member(int id, const string& name) : id(id), name(name) {}

    int getId() const { return id; }
    string getName() const { return name; }

private:
    int id;
    string name;
};

class Loan {
public:
    Loan(int bookId, int memberId) : bookId(bookId), memberId(memberId) {}

    int getBookId() const { return bookId; }
    int getMemberId() const { return memberId; }

private:
    int bookId;
    int memberId;
};

class Library {
public:
    Book* getBookById(int bookId) {
        return findBook(bookId);
    }
    void addBook(const Book& book) { books.push_back(book); }
    void removeBook(int bookId) {
    // Check if the book is currently issued
    auto itLoan = find_if(loans.begin(), loans.end(),
                          [bookId](const Loan& loan) { return loan.getBookId() == bookId; });

    if (itLoan != loans.end()) {
        // Book is currently issued
        cout << "First return the book.\n";
        return;
    }

    // Remove the book if it is not issued
    auto itBook = remove_if(books.begin(), books.end(),
                            [bookId](const Book& book) { return book.getId() == bookId; });

    if (itBook != books.end()) {
        books.erase(itBook, books.end());
    } else {
        throw runtime_error("Book not found");
    }
}


    void updateBook(int bookId, const Book& newBook) {
    // Check if the book is currently issued
    auto itLoan = find_if(loans.begin(), loans.end(),
                          [bookId](const Loan& loan) { return loan.getBookId() == bookId; });

    if (itLoan != loans.end()) {
        // Book is currently issued
        cout << "First return book before updating.\n";
        return;
    }

    // Update the book if it is not issued
    for (auto& book : books) {
        if (book.getId() == bookId) {
            book = newBook;
            return;
        }
    }
    throw runtime_error("Book not found");
}


    void addMember(const Member& member) { members.push_back(member); }
    void issueBook(int bookId, int memberId) {
    Book* book = findBook(bookId);
    Member* member = findMember(memberId);
    if (!book) throw runtime_error("Book not found");
    if (!member) throw runtime_error("Member not found");
    if (!book->isAvailable()) throw runtime_error("Book is not available");
    book->setAvailable(false);
    loans.push_back(Loan(bookId, memberId));
}

    void returnBook(int bookId) {
        Book* book = findBook(bookId);
        if (!book) throw runtime_error("Book not found");
        auto it = remove_if(loans.begin(), loans.end(),
                            [bookId](const Loan& loan) { return loan.getBookId() == bookId; });
        if (it != loans.end()) {
            book->setAvailable(true);
            loans.erase(it, loans.end());
        } else {
            throw runtime_error("Loan record not found");
        }
    }


    void saveData() const {
        ofstream bookFile("books.txt");
        ofstream memberFile("members.txt");
        ofstream loanFile("loans.txt");

        if (!bookFile || !memberFile || !loanFile) throw runtime_error("Unable to open file for writing");

        for (const auto& book : books) {
            bookFile << book.getId() << ',' << book.getTitle() << ',' << book.getAuthor() << ',' << (book.isAvailable() ? "1" : "0") << '\n';
        }

        for (const auto& member : members) {
            memberFile << member.getId() << ',' << member.getName() << '\n';
        }

        for (const auto& loan : loans) {
            loanFile << loan.getBookId() << ',' << loan.getMemberId() << '\n';
        }
    }

    void loadData() {
    ifstream bookFile("books.txt");
    ifstream memberFile("members.txt");
    ifstream loanFile("loans.txt");

    if (!bookFile || !memberFile || !loanFile) throw runtime_error("Unable to open file for reading");

    books.clear();
    members.clear();
    loans.clear();

    string line;
    // Load books
    while (getline(bookFile, line)) {
        istringstream iss(line);
        string token;
        if (getline(iss, token, ',')) {
            int id = stoi(token);
            if (getline(iss, token, ',')) {
                string title = token;
                if (getline(iss, token, ',')) {
                    string author = token;
                    bool available = (getline(iss, token) && token == "1"); // Read availability status
                    books.emplace_back(id, title, author);
                    books.back().setAvailable(available);
                }
            }
        }
    }

    // Load members
    while (getline(memberFile, line)) {
        istringstream iss(line);
        string token;
        if (getline(iss, token, ',')) {
            int id = stoi(token);
            if (getline(iss, token)) {
                string name = token;
                members.emplace_back(id, name);
            }
        }
    }

    // Load loans
    while (getline(loanFile, line)) {
        istringstream iss(line);
        string token;
        if (getline(iss, token, ',')) {
            int bookId = stoi(token);
            if (getline(iss, token)) {
                int memberId = stoi(token);
                loans.emplace_back(bookId, memberId);
            }
        }
    }
}

    void displayBooks() const {
    if (books.empty()) {
        cout << "No books available.\n";
        return;
    }
    cout << "Books:\n";
    for (const auto& book : books) {
        cout << "ID: " << book.getId() << ", Title: " << book.getTitle() << ", Author: " << book.getAuthor()
             << ", Available: " << (book.isAvailable() ? "Yes" : "No") << '\n';
    }
}


    void displayMembers() const {
        if (members.empty()) {
            cout << "No members available.\n";
            return;
        }
        cout << "Members:\n";
        for (const auto& member : members) {
            cout << "ID: " << member.getId() << ", Name: " << member.getName() << '\n';
        }
    }

    void displayLoans() const {
        if (loans.empty()) {
            cout << "No loans recorded.\n";
            return;
        }
        cout << "Loans:\n";
        for (const auto& loan : loans) {
            cout << "Book ID: " << loan.getBookId() << ", Member ID: " << loan.getMemberId() << '\n';
        }
    }

private:
    vector<Book> books;
    vector<Member> members;
    vector<Loan> loans;

    Book* findBook(int bookId) {
        for (auto& book : books) {
            if (book.getId() == bookId) return &book;
        }
        return nullptr;
    }

    Member* findMember(int memberId) {
        for (auto& member : members) {
            if (member.getId() == memberId) return &member;
        }
        return nullptr;
    }

    Loan* findLoan(int bookId, int memberId) {
        for (auto& loan : loans) {
            if (loan.getBookId() == bookId && loan.getMemberId() == memberId) return &loan;
        }
        return nullptr;
    }
};

void userInterface(Library& library) {
    while (true) {
        cout << "\nLibrary Management System\n";
        cout << "1. Add Book\n";
        cout << "2. Remove Book\n";
        cout << "3. Update Book\n";
        cout << "4. Add Member\n";
        cout << "5. Issue Book\n";
        cout << "6. Return Book\n";
        cout << "7. View All Books\n";
        cout << "8. View All Members\n";
        cout << "9. View All Loans\n";
        cout << "10. Exit\n";
        cout << "Enter choice: ";
        int choice;
        cin >> choice;
        cin.ignore();

        try {
            switch (choice) {
                case 1: {
                    int id;
                    string title, author;
                    cout << "Enter Book ID: ";
                    cin >> id;
                    cin.ignore();
                    cout << "Enter Title: ";
                    getline(cin, title);
                    cout << "Enter Author: ";
                    getline(cin, author);
                    library.addBook(Book(id, title, author));
                    break;
                }
                case 2: {
                    int id;
                    cout << "Enter Book ID to remove: ";
                    cin >> id;
                    library.removeBook(id);
                    break;
                }
                case 3: {
                    int id;
                    string title, author;
                    cout << "Enter Book ID to update: ";
                    cin >> id;
                    cin.ignore();

                    // Check if the book is issued
                    Book* existingBook = library.getBookById(id);
                    if (existingBook) {
                        // Book found, check if it's available
                        if (!existingBook->isAvailable()) {
                            cout << "First return book before updating.\n";
                            break;
                        }
                    } else {
                        cout << "Book not found.\n";
                        break;
                    }

                    cout << "Enter New Title: ";
                    getline(cin, title);
                    cout << "Enter New Author: ";
                    getline(cin, author);

                    // Update the book details
                    library.updateBook(id, Book(id, title, author));
                    break;
                }
                case 4: {
                    int id;
                    string name;
                    cout << "Enter Member ID: ";
                    cin >> id;
                    cin.ignore();
                    cout << "Enter Name: ";
                    getline(cin, name);
                    library.addMember(Member(id, name));
                    break;
                }
                case 5: {
                    int bookId, memberId;
                    cout << "Enter Book ID to issue: ";
                    cin >> bookId;
                    cout << "Enter Member ID: ";
                    cin >> memberId;
                    library.issueBook(bookId, memberId);
                    break;
                }
                case 6: {
                    int bookId;
                    cout << "Enter Book ID to return: ";
                    cin >> bookId;
                    library.returnBook(bookId);
                    break;
                }
                case 7:
                    library.displayBooks();
                    break;
                case 8:
                    library.displayMembers();
                    break;
                case 9:
                    library.displayLoans();
                    break;
                case 10:
                    return;
                default:
                    cout << "Invalid choice, please try again.\n";
                    break;
            }
        } catch (const exception& e) {
            cout << "Error: " << e.what() << '\n';
        }
    }
}

int main() {
    Library library;

    library.loadData();  // Load data from files

    userInterface(library);

    library.saveData();  // Save data to files

    return 0;
}
