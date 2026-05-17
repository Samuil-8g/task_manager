#pragma once
#include "BaseEntity.h"
#include <string>

class Tag : public BaseEntity {
private:
    std::string name;
    std::string color;

public:
    Tag(const std::string& name, const std::string& color)
        : name(name), color(color) {}

    std::string getName()  const { return name; }
    std::string getColor() const { return color; }

    void setColor(const std::string& c) { color = c; markUpdated(); }

    bool validate() const override { return !name.empty(); }

    std::string toString() const { return "[" + name + "]"; }
};
