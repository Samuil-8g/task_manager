#pragma once
#include "Project.h"
#include "Task.h"
#include "RecurringTask.h"
#include "TaskDependency.h"
#include "User.h"
#include "Tag.h"
#include <vector>
#include <string>
#include <iostream>
#include <set>
#include <algorithm>

// За Tag трябва да го включим тук (Comment.h го декларира forward)

class TaskService {
private:
    std::vector<Project*>      projects;
    std::vector<User*>         users;
    std::vector<Tag*>          tags;
    std::vector<TaskDependency*> dependencies;

    // DFS за проверка на цикъл
    bool reachable(Task* from, const std::string& targetId, std::set<std::string>& visited) {
        if (from->getId() == targetId) return true;
        if (visited.count(from->getId())) return false;
        visited.insert(from->getId());
        for (auto d : dependencies) {
            if (d->getBlockerTask()->getId() == from->getId()) {
                if (reachable(d->getBlockedTask(), targetId, visited)) return true;
            }
        }
        return false;
    }

    bool wouldCreateCycle(Task* blocker, Task* blocked) {
        std::set<std::string> visited;
        return reachable(blocked, blocker->getId(), visited);
    }

public:
    ~TaskService() {
        for (auto p : projects) delete p;
        for (auto u : users)    delete u;
        for (auto t : tags)     delete t;
        for (auto d : dependencies) delete d;
    }

    // ── Потребители ───────────────────────────────────────────────────────────
    User* createUser(const std::string& username, const std::string& email,
                     const std::string& fullName, const std::string& role) {
        User* u = new User(username, email, fullName, role);
        users.push_back(u);
        return u;
    }

    User* findUserByUsername(const std::string& username) {
        for (auto u : users)
            if (u->getUsername() == username) return u;
        return nullptr;
    }

    const std::vector<User*>& getAllUsers() const { return users; }

    // ── Проекти (CRUD) ────────────────────────────────────────────────────────
    Project* createProject(const std::string& name, const std::string& description,
                           const std::string& deadline, Priority priority) {
        Project* p = new Project(name, description, deadline, priority);
        projects.push_back(p);
        return p;
    }

    const std::vector<Project*>& getAllProjects() const { return projects; }

    bool deleteProject(const std::string& id) {
        for (auto it = projects.begin(); it != projects.end(); ++it) {
            if ((*it)->getId() == id) {
                delete *it;
                projects.erase(it);
                return true;
            }
        }
        return false;
    }

    // ── Задачи (CRUD) ─────────────────────────────────────────────────────────
    Task* createTask(Project* project, const std::string& title,
                     const std::string& description, const std::string& deadline,
                     Priority priority) {
        Task* t = new Task(title, description, deadline, priority);
        project->addTask(t);
        return t;
    }

    RecurringTask* createRecurringTask(Project* project, const std::string& title,
                                       const std::string& description,
                                       const std::string& deadline,
                                       Priority priority,
                                       RecurringTask::Recurrence recurrence) {
        RecurringTask* t = new RecurringTask(title, description, deadline,
                                             priority, recurrence);
        project->addTask(t);
        return t;
    }

    bool deleteTask(Project* project, Task* task) {
        dependencies.erase(
            std::remove_if(dependencies.begin(), dependencies.end(),
                [&](TaskDependency* d) {
                    return d->getBlockerTask()->getId() == task->getId()
                        || d->getBlockedTask()->getId() == task->getId();
                }),
            dependencies.end());
        return project->removeTask(task);
    }

    // ── Статус ────────────────────────────────────────────────────────────────
    bool changeTaskStatus(Task* task, TaskStatus newStatus, User* changedBy) {
        if (newStatus == TaskStatus::IN_PROGRESS && isBlocked(task)) {
            std::cout << "Задачата е блокирана от незавършена задача!\n";
            return false;
        }
        return task->changeStatus(newStatus, changedBy);
    }

    // ── Тагове ────────────────────────────────────────────────────────────────
    Tag* createTag(const std::string& name, const std::string& color) {
        Tag* t = new Tag(name, color);
        tags.push_back(t);
        return t;
    }

    const std::vector<Tag*>& getAllTags() const { return tags; }

    // ── Зависимости ───────────────────────────────────────────────────────────
    bool addDependency(Task* blocker, Task* blocked) {
        if (wouldCreateCycle(blocker, blocked)) {
            std::cout << "Циклична зависимост! Не е добавена.\n";
            return false;
        }
        TaskDependency* dep = new TaskDependency(blocker, blocked);
        if (!dep->validate()) { delete dep; return false; }
        dependencies.push_back(dep);
        return true;
    }

    bool isBlocked(Task* task) {
        for (auto d : dependencies) {
            if (d->getBlockedTask()->getId() == task->getId()
             && d->getBlockerTask()->getStatus() != TaskStatus::DONE)
                return true;
        }
        return false;
    }

    const std::vector<TaskDependency*>& getDependencies() const { return dependencies; }

    // ── Филтриране ────────────────────────────────────────────────────────────
    std::vector<Task*> filterTasks(Project* project,
                                   bool filterStatus, TaskStatus status,
                                   bool filterPriority, Priority priority,
                                   User* assignee,
                                   const std::string& searchText) {
        std::vector<Task*> result;
        for (auto t : project->getTasks()) {
            if (filterStatus   && t->getStatus()   != status)   continue;
            if (filterPriority && t->getPriority() != priority)  continue;
            if (assignee != nullptr
             && (t->getAssignee() == nullptr
                 || t->getAssignee()->getId() != assignee->getId())) continue;
            if (!searchText.empty()) {
                std::string title = t->getTitle();
                std::string low   = searchText;
                // Прост case-insensitive: само ASCII
                std::transform(title.begin(), title.end(), title.begin(), ::tolower);
                std::transform(low.begin(),   low.end(),   low.begin(),   ::tolower);
                if (title.find(low) == std::string::npos) continue;
            }
            result.push_back(t);
        }
        return result;
    }

    // ── Dashboard ─────────────────────────────────────────────────────────────
    void printDashboard() const {
        std::cout << "\n========= DASHBOARD =========\n";

        // Всички задачи
        std::vector<Task*> all;
        for (auto p : projects)
            for (auto t : p->getTasks())
                all.push_back(t);

        std::cout << "Задачи по статус:\n";
        auto countStatus = [&](TaskStatus s) {
            int n = 0;
            for (auto t : all) if (t->getStatus() == s) n++;
            return n;
        };
        std::cout << "  To Do      : " << countStatus(TaskStatus::TODO)        << "\n";
        std::cout << "  In Progress: " << countStatus(TaskStatus::IN_PROGRESS) << "\n";
        std::cout << "  In Review  : " << countStatus(TaskStatus::IN_REVIEW)   << "\n";
        std::cout << "  Done       : " << countStatus(TaskStatus::DONE)        << "\n";
        std::cout << "  Cancelled  : " << countStatus(TaskStatus::CANCELLED)   << "\n";

        std::cout << "\nЗакъснели проекти:\n";
        bool any = false;
        for (auto p : projects)
            if (p->isOverdue()) { std::cout << "  ! " << p->getName() << "\n"; any = true; }
        if (!any) std::cout << "  Няма\n";

        std::cout << "\nНатоварване по изпълнители:\n";
        for (auto u : users) {
            int cnt = 0;
            for (auto t : all)
                if (t->getAssignee()
                 && t->getAssignee()->getId() == u->getId()
                 && t->getStatus() != TaskStatus::DONE
                 && t->getStatus() != TaskStatus::CANCELLED)
                    cnt++;
            if (cnt > 0)
                std::cout << "  " << u->getUsername() << " : " << cnt << " активни\n";
        }
        std::cout << "=============================\n\n";
    }
};
