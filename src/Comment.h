#pragma once
#include "BaseEntity.h"
#include "User.h"
#include <string>

class Comment : public BaseEntity {
private:
    std::string text;
    User*       author;

public:
    Comment(const std::string& text, User* author)
        : text(text), author(author) {}

    std::string getText()   const { return text; }
    User*       getAuthor() const { return author; }

    bool validate() const override {
        return !text.empty() && author != nullptr;
    }

    std::string toString() const {
        return "[" + createdAt + "] " + author->getUsername() + ": " + text;
    }
};

class ChangeLogEntry {
private:
    User*       changedBy;
    std::string field;
    std::string oldValue;
    std::string newValue;
    std::string changedAt;

public:
    ChangeLogEntry(User* changedBy, const std::string& field,
                   const std::string& oldValue, const std::string& newValue)
        : changedBy(changedBy), field(field),
          oldValue(oldValue), newValue(newValue),
          changedAt(currentDateTime()) {}

    std::string toString() const {
        return "[" + changedAt + "] " + changedBy->getUsername()
             + " промени " + field + ": " + oldValue + " -> " + newValue;
    }
};
