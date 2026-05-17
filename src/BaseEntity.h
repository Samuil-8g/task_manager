#pragma once
#include <string>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <random>

// Генерира прост уникален ID
inline std::string generateId() {
    static int counter = 1;
    return "ID-" + std::to_string(counter++);
}

inline std::string currentDateTime() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm* tm_info = std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(tm_info, "%Y-%m-%d %H:%M");
    return oss.str();
}

// Абстрактен базов клас – всички обекти имат id, createdAt, updatedAt
class BaseEntity {
protected:
    std::string id;
    std::string createdAt;
    std::string updatedAt;

    void markUpdated() {
        updatedAt = currentDateTime();
    }

public:
    BaseEntity() {
        id        = generateId();
        createdAt = currentDateTime();
        updatedAt = currentDateTime();
    }

    virtual ~BaseEntity() = default;

    std::string getId()        const { return id; }
    std::string getCreatedAt() const { return createdAt; }
    std::string getUpdatedAt() const { return updatedAt; }

    // Чисто виртуален метод – всеки наследник имплементира своята валидация
    virtual bool validate() const = 0;
};
