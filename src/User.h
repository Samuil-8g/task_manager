#pragma once
#include "BaseEntity.h"
#include <string>
#include <iostream>

class User : public BaseEntity {
private:
    std::string username;
    std::string email;
    std::string fullName;
    std::string role; // "ADMIN" или "MEMBER"

public:
    User(const std::string& username, const std::string& email,
         const std::string& fullName, const std::string& role)
        : username(username), email(email), fullName(fullName), role(role) {}

    std::string getUsername() const { return username; }
    std::string getEmail()    const { return email; }
    std::string getFullName() const { return fullName; }
    std::string getRole()     const { return role; }

    void setEmail(const std::string& e) { email = e; markUpdated(); }
    void setRole(const std::string& r)  { role  = r; markUpdated(); }

    bool login(const std::string& inputUsername) const {
        return username == inputUsername;
    }

    bool validate() const override {
        return !username.empty() && email.find('@') != std::string::npos;
    }

    std::string toString() const {
        return username + " (" + fullName + ")";
    }
};
