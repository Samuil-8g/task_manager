#pragma once
#include "BaseEntity.h"
#include "Task.h"

class TaskDependency : public BaseEntity {
private:
    Task* blockerTask;
    Task* blockedTask;

public:
    TaskDependency(Task* blocker, Task* blocked)
        : blockerTask(blocker), blockedTask(blocked) {}

    Task* getBlockerTask() const { return blockerTask; }
    Task* getBlockedTask() const { return blockedTask; }

    bool validate() const override {
        return blockerTask != nullptr
            && blockedTask != nullptr
            && blockerTask->getId() != blockedTask->getId();
    }

    std::string toString() const {
        return blockerTask->getTitle() + " блокира " + blockedTask->getTitle();
    }
};
