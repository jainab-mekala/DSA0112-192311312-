#include <cstring>
#include <iostream>
#include <utility>

class Text {
private:
    char* data;

public:
    Text(const char* value = "") {
        if (!value) value = "";
        data = new char[std::strlen(value) + 1];
        std::strcpy(data, value);
    }

    Text(const Text& other) {
        data = new char[std::strlen(other.data) + 1];
        std::strcpy(data, other.data);
    }

    Text& operator=(const Text& other) {
        if (this == &other) return *this;

        char* newData = new char[std::strlen(other.data) + 1];
        std::strcpy(newData, other.data);
        delete[] data;
        data = newData;
        return *this;
    }

    ~Text() {
        delete[] data;
    }

    Text operator+(const Text& other) const {
        std::size_t len1 = std::strlen(data);
        std::size_t len2 = std::strlen(other.data);
        char* combined = new char[len1 + len2 + 1];
        std::strcpy(combined, data);
        std::strcat(combined, other.data);
        Text result(combined);
        delete[] combined;
        return result;
    }

    bool operator==(const Text& other) const {
        return std::strcmp(data, other.data) == 0;
    }

    bool operator!=(const Text& other) const {
        return !(*this == other);
    }

    friend std::ostream& operator<<(std::ostream& out, const Text& text) {
        return out << text.data;
    }
};

int main() {
    Text first("Hello, ");
    Text second("World!");
    Text message = first + second;

    std::cout << "Message: " << message << '\n';
    std::cout << std::boolalpha
              << "Equal to Hello, World!: "
              << (message == Text("Hello, World!")) << '\n';

    message = message; // self-assignment test
    Text empty;
    Text combined = message + empty;
    std::cout << "After self-assignment: " << combined << '\n';
    std::cout << "Empty text: [" << empty << "]\n";
    return 0;
}
