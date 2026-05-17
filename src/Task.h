#pragma once
#include "BaseEntity.h"
#include "Enums.h"
#include "User.h"
#include "Comment.h"
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

class Tag; // forward declaration

class Task : public BaseEntity {
private:
    std::string title;
    std::string description;
    std::string deadline;
    Priority    priority;
    TaskStatus  status;
    User*       assignee;

    std::vector<Tag*>           tags;
    std::vector<Comment*>       comments;
    std::vector<ChangeLogEntry> changeLog;

public:
    Task(const std::string& title, const std::string& description,
         const std::string& deadline, Priority priority)
        : title(title), description(description),
          deadline(deadline), priority(priority),
          status(TaskStatus::TODO), assignee(nullptr) {}

    virtual ~Task() {
        for (auto c : comments) delete c;
    }

    // ── Getters ──────────────────────────────────────────────────────────────
    std::string getTitle()       const { return title; }
    std::string getDescription() const { return description; }
    std::string getDeadline()    const { return deadline; }
    TaskStatus  getStatus()      const { return status; }
    Priority    getPriority()    const { return priority; }
    User*       getAssignee()    const { return assignee; }

    const std::vector<Tag*>&           getTags()      const { return tags; }
    const std::vector<Comment*>&       getComments()  const { return comments; }
    const std::vector<ChangeLogEntry>& getChangeLog() const { return changeLog; }

    // ── Setters ───────────────────────────────────────────────────────────────
    void setTitle(const std::string& t)       { title = t;       markUpdated(); }
    void setDescription(const std::string& d) { description = d; markUpdated(); }
    void setDeadline(const std::string& d)    { deadline = d;    markUpdated(); }

    // ── Смяна на статус с валиден преход ─────────────────────────────────────
    bool changeStatus(TaskStatus newStatus, User* changedBy) {
        if (!canTransition(status, newStatus)) return false;
        changeLog.emplace_back(changedBy, "status",
            taskStatusToString(status), taskStatusToString(newStatus));
        status = newStatus;
        markUpdated();
        return true;
    }

    // ── Назначаване ───────────────────────────────────────────────────────────
    void assignTo(User* user, User* changedBy) {
        std::string oldVal = assignee ? assignee->getUsername() : "никой";
        std::string newVal = user     ? user->getUsername()     : "никой";
        changeLog.emplace_back(changedBy, "assignee", oldVal, newVal);
        assignee = user;
        markUpdated();
    }

    // ── Приоритет ─────────────────────────────────────────────────────────────
    void setPriority(Priority p, User* changedBy) {
        changeLog.emplace_back(changedBy, "priority",
            priorityToString(priority), priorityToString(p));
        priority = p;
        markUpdated();
    }

    // ── Тагове ────────────────────────────────────────────────────────────────
    void addTag(Tag* tag) {
        if (std::find(tags.begin(), tags.end(), tag) == tags.end()) {
            tags.push_back(tag);
            markUpdated();
        }
    }

    void removeTag(Tag* tag) {
        tags.erase(std::remove(tags.begin(), tags.end(), tag), tags.end());
        markUpdated();
    }

    // ── Коментар ──────────────────────────────────────────────────────────────
    void addComment(const std::string& text, User* author) {
        comments.push_back(new Comment(text, author));
        markUpdated();
    }

    // ── Помощни ───────────────────────────────────────────────────────────────
    bool isOverdue() const {
        // Проста проверка: ако deadline е попълнен и статусът не е Done/Cancelled
        return !deadline.empty()
            && status != TaskStatus::DONE
            && status != TaskStatus::CANCELLED;
        // (За реална дата-сравнение би трябвало да парснем датата)
    }

    virtual bool validate() const override {
        return !title.empty();
    }

    virtual std::string toString() const {
        std::string asgn = assignee ? assignee->getUsername() : "-";
        std::string over = isOverdue() ? " ЗАКЪСНЯЛА" : "";
        return "[" + taskStatusToString(status) + "] " + title
             + " | " + priorityToString(priority)
             + " | срок: " + (deadline.empty() ? "без" : deadline)
             + " | изп: " + asgn + over;
    }
};
