#pragma once
#include "Task.h"
#include <string>

class RecurringTask : public Task {
public:
    enum class Recurrence { DAILY, WEEKLY, MONTHLY };

private:
    Recurrence  recurrence;
    std::string nextDueDate;

    std::string recurrenceToString() const {
        switch (recurrence) {
            case Recurrence::DAILY:   return "Дневно";
            case Recurrence::WEEKLY:  return "Седмично";
            case Recurrence::MONTHLY: return "Месечно";
            default:                  return "";
        }
    }

public:
    RecurringTask(const std::string& title, const std::string& description,
                  const std::string& deadline, Priority priority,
                  Recurrence recurrence)
        : Task(title, description, deadline, priority),
          recurrence(recurrence), nextDueDate(deadline) {}

    Recurrence  getRecurrence()  const { return recurrence; }
    std::string getNextDueDate() const { return nextDueDate; }

    // Генерира следващото повтаряне като нова Task
    Task* generateNext() {
        Task* next = new Task(
            getTitle() + " (повторение)",
            getDescription(),
            nextDueDate,
            getPriority()
        );
        if (getAssignee()) next->assignTo(getAssignee(), getAssignee());
        // Изместваме nextDueDate (опростено – добавяме 7 дни за WEEKLY и т.н.)
        nextDueDate = nextDueDate + "+recur"; // символично
        return next;
    }

    bool validate() const override {
        return Task::validate(); // + recurrence винаги е валиден enum
    }

    std::string toString() const override {
        return Task::toString()
             + " | Повтаря се: " + recurrenceToString()
             + " | Следващо: "   + nextDueDate;
    }
};
