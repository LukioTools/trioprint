#pragma once

class CustomCout {
public:
    template<typename T>
    CustomCout& operator<<(T const& value) {
        Serial.print(value);
        return *this;
    }

    CustomCout& operator<<(std::ostream& (*func)(std::ostream&)) {
        Serial.println();
        return *this;
    }
};

CustomCout cout;
