#pragma once
#include "BaseEntity.h"
#include "Enums.h"
#include "Task.h"
#include <string>
#include <vector>
#include <algorithm>

class Project : public BaseEntity {
private:
    std::string       name;
    std::string       description;
    std::string       deadline;
    Priority          priority;
    std::string       status; // "ACTIVE", "COMPLETED", "ON_HOLD" 
    std::vector<Task*> tasks;

public:
    Project(const std::string& name, const std::string& description,
            const std::string& deadline, Priority priority)
        : name(name), description(description),
          deadline(deadline), priority(priority), status("ACTIVE") {}

    ~Project() {
        for (auto t : tasks) delete t;
    }

    // ── Getters ──────────────────────────────────────────────────────────────
    std::string getName()        const { return name; }
    std::string getDescription() const { return description; }
    std::string getDeadline()    const { return deadline; }
    std::string getStatus()      const { return status; }
    Priority    getPriority()    const { return priority; }

    const std::vector<Task*>& getTasks() const { return tasks; }

    // ── Setters ───────────────────────────────────────────────────────────────
    void setName(const std::string& n)        { name = n;        markUpdated(); }
    void setDescription(const std::string& d) { description = d; markUpdated(); }
    void setStatus(const std::string& s)      { status = s;      markUpdated(); }

    // ── Задачи ────────────────────────────────────────────────────────────────
    void addTask(Task* task) {
        tasks.push_back(task);
        markUpdated();
    }

    bool removeTask(Task* task) {
        auto it = std::find(tasks.begin(), tasks.end(), task);
        if (it == tasks.end()) return false;
        tasks.erase(it);
        markUpdated();
        return true;
    }

    // ── Прогрес ───────────────────────────────────────────────────────────────
    int getProgress() const {
        if (tasks.empty()) return 0;
        int done = 0;
        for (auto t : tasks)
            if (t->getStatus() == TaskStatus::DONE) done++;
        return done * 100 / (int)tasks.size();
    }

    bool isOverdue() const {
        return !deadline.empty() && status != "COMPLETED";
    }

    bool validate() const override { return !name.empty(); }

    std::string toString() const {
        return "Проект: " + name
             + " | " + status
             + " | Прогрес: " + std::to_string(getProgress()) + "%"
             + " | Задачи: "  + std::to_string(tasks.size())
             + (isOverdue() ? " ЗАКЪСНЯЛ" : "");
    }
};
