#include <iostream>
#include <stdexcept>

class Auditorium {
private:
    int rows;
    int seatsPerRow;
    bool** seats;

public:
    Auditorium(int r, int s)
        : rows(r < 0 ? 0 : r),
          seatsPerRow(s < 0 ? 0 : s),
          seats(nullptr) {
        if (rows > 0 && seatsPerRow > 0) {
            seats = new bool*[rows]{};
            try {
                for (int i = 0; i < rows; ++i)
                    seats[i] = new bool[seatsPerRow]{false};
            } catch (...) {
                for (int i = 0; i < rows; ++i) delete[] seats[i];
                delete[] seats;
                seats = nullptr;
                throw;
            }
        }
        std::cout << "Auditorium created: " << rows << " x "
                  << seatsPerRow << '\n';
    }

    // Avoid accidental shallow copying of owned memory.
    Auditorium(const Auditorium&) = delete;
    Auditorium& operator=(const Auditorium&) = delete;

    bool book(int row, int seat) {
        if (!valid(row, seat) || seats[row][seat]) return false;
        seats[row][seat] = true;
        return true;
    }

    bool cancel(int row, int seat) {
        if (!valid(row, seat) || !seats[row][seat]) return false;
        seats[row][seat] = false;
        return true;
    }

    void display() const {
        for (int r = 0; r < rows; ++r) {
            for (int s = 0; s < seatsPerRow; ++s)
                std::cout << (seats[r][s] ? 'X' : 'O') << ' ';
            std::cout << '\n';
        }
    }

    ~Auditorium() {
        if (seats) {
            for (int i = 0; i < rows; ++i) delete[] seats[i];
            delete[] seats;
        }
        std::cout << "Auditorium closed; allocated seat map released.\n";
    }

private:
    bool valid(int r, int s) const {
        return r >= 0 && r < rows && s >= 0 && s < seatsPerRow && seats;
    }
};

int main() {
    {
        Auditorium hall(2, 4); // stack object, destroyed at block end
        hall.book(0, 1);
        hall.book(1, 3);
        hall.display();
        hall.cancel(0, 1);
        std::cout << "After cancellation:\n";
        hall.display();
    }

    Auditorium* specialEvent = new Auditorium(1, 3);
    specialEvent->book(0, 0);
    specialEvent->display();
    delete specialEvent; // destructor runs here

    Auditorium zeroRows(0, 5);
    zeroRows.display();
    return 0;
}
