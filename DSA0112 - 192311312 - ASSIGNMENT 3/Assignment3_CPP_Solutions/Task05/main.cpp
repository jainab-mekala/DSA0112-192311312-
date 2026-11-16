#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

class Time {
private:
    int seconds; // seconds since 00:00:00

public:
    explicit Time(int totalSeconds = 0) : seconds(totalSeconds) {
        if (seconds < 0) seconds = 0;
        seconds %= 24 * 60 * 60;
    }

    Time& operator++() { // Prefix
        seconds = (seconds + 1) % (24 * 60 * 60);
        return *this;
    }

    Time operator++(int) { // Postfix
        Time old = *this;
        ++(*this);
        return old;
    }

    bool operator==(const Time& other) const {
        return seconds == other.seconds;
    }

    bool operator<(const Time& other) const {
        return seconds < other.seconds;
    }

    friend std::ostream& operator<<(std::ostream& out, const Time& t) {
        int h = t.seconds / 3600;
        int m = (t.seconds % 3600) / 60;
        int s = t.seconds % 60;
        return out << (h < 10 ? "0" : "") << h << ':'
                   << (m < 10 ? "0" : "") << m << ':'
                   << (s < 10 ? "0" : "") << s;
    }
};

class Playlist {
private:
    std::vector<std::string> tracks;

public:
    explicit Playlist(std::vector<std::string> items) : tracks(std::move(items)) {}

    std::string& operator[](std::size_t index) {
        if (index >= tracks.size()) throw std::out_of_range("Playlist index out of range");
        return tracks[index];
    }

    const std::string& operator[](std::size_t index) const {
        if (index >= tracks.size()) throw std::out_of_range("Playlist index out of range");
        return tracks[index];
    }

    std::size_t size() const { return tracks.size(); }
};

int main() {
    Time t(59 * 60 + 59);
    Time a = t++;
    Time b = ++t;
    std::cout << "a = " << a << "\nb = " << b << "\nt = " << t << '\n';

    Playlist playlist({"Track One", "Track Two", "Track Three"});
    for (std::size_t i = 0; i < playlist.size(); ++i)
        std::cout << i << ": " << playlist[i] << '\n';

    try {
        std::cout << playlist[playlist.size()] << '\n';
    } catch (const std::out_of_range& ex) {
        std::cout << "Range error: " << ex.what() << '\n';
    }
    return 0;
}
