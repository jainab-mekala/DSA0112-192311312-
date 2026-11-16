#include <iostream>
#include <string>
#include <stdexcept>

class Book {
private:
    std::string title;
    std::string author;
    int copies;

    static int validateCopies(int value) {
        if (value < 0) throw std::invalid_argument("Copies cannot be negative.");
        return value;
    }

public:
    Book() : Book("Untitled", "Unknown", 0) {}

    explicit Book(const std::string& bookTitle)
        : Book(bookTitle, "Unknown", 0) {}

    Book(const std::string& bookTitle, const std::string& bookAuthor, int count)
        : title(bookTitle), author(bookAuthor), copies(validateCopies(count)) {}

    void display() const {
        std::cout << "Title: " << title
                  << ", Author: " << author
                  << ", Copies: " << copies << '\n';
    }
};

int main() {
    try {
        Book emptyBook;
        Book donatedBook("The Alchemist");
        Book fullBook("Clean Code", "Robert C. Martin", 5);

        emptyBook.display();
        donatedBook.display();
        fullBook.display();

        // This would throw std::invalid_argument:
        // Book invalidBook("Invalid", "Author", -2);
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << '\n';
        return 1;
    }
    return 0;
}
