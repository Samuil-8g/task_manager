#include <iostream>
#include <string>
#include <limits>
#include "TaskService.h"

TaskService service;
User* currentUser = nullptr;

// ── Помощни функции ───────────────────────────────────────────────────────────

int readInt(int min, int max) {
    int val;
    while (true) {
        std::cin >> val;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (val >= min && val <= max) return val;
        std::cout << "Невалиден избор. Опитай пак: ";
    }
}

std::string readLine() {
    std::string line;
    std::getline(std::cin, line);
    return line;
}

Project* pickProject() {
    const auto& all = service.getAllProjects();
    if (all.empty()) { std::cout << "Няма проекти.\n\n"; return nullptr; }
    std::cout << "Изберете проект:\n";
    for (int i = 0; i < (int)all.size(); i++)
        std::cout << i+1 << ". " << all[i]->getName() << "\n";
    std::cout << "Избор: ";
    int idx = readInt(1, (int)all.size()) - 1;
    return all[idx];
}

Task* pickTask(Project* p) {
    const auto& tasks = p->getTasks();
    if (tasks.empty()) { std::cout << "Няма задачи.\n\n"; return nullptr; }
    std::cout << "Изберете задача:\n";
    for (int i = 0; i < (int)tasks.size(); i++)
        std::cout << i+1 << ". " << tasks[i]->getTitle() << "\n";
    std::cout << "Избор: ";
    int idx = readInt(1, (int)tasks.size()) - 1;
    return tasks[idx];
}

// ── Демо данни ────────────────────────────────────────────────────────────────

void seedData() {
    User* admin   = service.createUser("admin", "admin@demo.bg", "Иван Иванов",  "ADMIN");
    User* pesho   = service.createUser("pesho", "pesho@demo.bg", "Петър Петров", "MEMBER");
    User* maria   = service.createUser("maria", "maria@demo.bg", "Мария Колева", "MEMBER");

    Tag* bugTag  = service.createTag("bug",     "#FF5733");
    Tag* featTag = service.createTag("feature", "#33A1FF");
    Tag* uiTag   = service.createTag("UI",      "#A133FF");

    Project* p1 = service.createProject("Уеб платформа",
                    "Основен проект за онлайн платформа",
                    "2025-12-01", Priority::HIGH);

    Task* t1 = service.createTask(p1, "Дизайн на база данни",
                    "Схема, таблици, релации", "2025-08-01", Priority::CRITICAL);
    t1->addTag(featTag);
    t1->assignTo(admin, admin);

    Task* t2 = service.createTask(p1, "Имплементация на API",
                    "REST endpoints", "2025-09-01", Priority::HIGH);
    t2->addTag(featTag);
    t2->assignTo(pesho, admin);

    Task* t3 = service.createTask(p1, "Поправи login бутона",
                    "Не работи на мобилни", "2025-06-01", Priority::MEDIUM);
    t3->addTag(bugTag);
    t3->addTag(uiTag);
    t3->assignTo(maria, admin);
    t3->addComment("Репродуциран на iPhone 14.", maria);

    service.addDependency(t1, t2); // t2 зависи от t1

    service.createRecurringTask(p1, "Седмичен статус репорт",
        "Изпращане до клиента", "2025-07-07", Priority::LOW,
        RecurringTask::Recurrence::WEEKLY)->assignTo(admin, admin);

    Project* p2 = service.createProject("Мобилно приложение",
                    "iOS и Android версия", "2026-03-01", Priority::MEDIUM);
    Task* t4 = service.createTask(p2, "Mockup на главен екран",
                    "Figma дизайн", "2025-07-15", Priority::HIGH);
    t4->addTag(uiTag);
}

// ── Менюта ────────────────────────────────────────────────────────────────────

void showProjects() {
    const auto& all = service.getAllProjects();
    if (all.empty()) { std::cout << "Няма проекти.\n\n"; return; }
    for (int i = 0; i < (int)all.size(); i++)
        std::cout << i+1 << ". " << all[i]->toString() << "\n";
    std::cout << "\n";
}

void showTasksOfProject() {
    Project* p = pickProject();
    if (!p) return;
    const auto& tasks = p->getTasks();
    if (tasks.empty()) { std::cout << "Няма задачи.\n\n"; return; }
    for (int i = 0; i < (int)tasks.size(); i++)
        std::cout << i+1 << ". " << tasks[i]->toString() << "\n";
    std::cout << "\n";
}

void changeStatus() {
    Project* p = pickProject();
    if (!p) return;
    Task* t = pickTask(p);
    if (!t) return;

    TaskStatus statuses[] = {
        TaskStatus::TODO, TaskStatus::IN_PROGRESS,
        TaskStatus::IN_REVIEW, TaskStatus::DONE, TaskStatus::CANCELLED
    };
    std::cout << "Нов статус:\n";
    for (int i = 0; i < 5; i++)
        std::cout << i+1 << ". " << taskStatusToString(statuses[i]) << "\n";
    std::cout << "Избор: ";
    int idx = readInt(1, 5) - 1;

    bool ok = service.changeTaskStatus(t, statuses[idx], currentUser);
    std::cout << (ok ? "Статусът е сменен!\n" : "Невалиден преход!\n") << "\n";
}

void addTask() {
    Project* p = pickProject();
    if (!p) return;

    std::cout << "Заглавие: ";
    std::string title = readLine();
    std::cout << "Описание: ";
    std::string desc = readLine();
    std::cout << "Краен срок (YYYY-MM-DD, Enter = без): ";
    std::string deadline = readLine();

    Priority pvals[] = { Priority::CRITICAL, Priority::HIGH,
                         Priority::MEDIUM,   Priority::LOW };
    std::cout << "Приоритет:\n";
    for (int i = 0; i < 4; i++)
        std::cout << i+1 << ". " << priorityToString(pvals[i]) << "\n";
    std::cout << "Избор: ";
    int pi = readInt(1, 4) - 1;

    Task* t = service.createTask(p, title, desc, deadline, pvals[pi]);
    std::cout << "Задача \"" << t->getTitle() << "\" е създадена.\n\n";
}

void addComment() {
    Project* p = pickProject();
    if (!p) return;
    Task* t = pickTask(p);
    if (!t) return;
    std::cout << "Коментар: ";
    std::string text = readLine();
    if (text.empty()) { std::cout << "Празен коментар.\n\n"; return; }
    t->addComment(text, currentUser);
    std::cout << "Коментарът е добавен.\n\n";
}

void filterTasks() {
    Project* p = pickProject();
    if (!p) return;

    std::cout << "Търсен текст (Enter = без): ";
    std::string text = readLine();

    TaskStatus statuses[] = {
        TaskStatus::TODO, TaskStatus::IN_PROGRESS,
        TaskStatus::IN_REVIEW, TaskStatus::DONE, TaskStatus::CANCELLED
    };
    std::cout << "Статус:\n";
    std::cout << "0. Всички\n";
    for (int i = 0; i < 5; i++)
        std::cout << i+1 << ". " << taskStatusToString(statuses[i]) << "\n";
    std::cout << "Избор: ";
    int si = readInt(0, 5);

    bool filterS = (si > 0);
    TaskStatus fs = filterS ? statuses[si-1] : TaskStatus::TODO;

    auto result = service.filterTasks(p, filterS, fs, false, Priority::LOW, nullptr, text);
    if (result.empty()) {
        std::cout << "Няма намерени задачи.\n\n";
    } else {
        for (auto t : result)
            std::cout << "  * " << t->toString() << "\n";
        std::cout << "\n";
    }
}

void addDependency() {
    Project* p = pickProject();
    if (!p) return;
    std::cout << "Изберете блокер (трябва да завърши ПЪРВО):\n";
    Task* blocker = pickTask(p);
    if (!blocker) return;
    std::cout << "Изберете блокирана задача:\n";
    Task* blocked = pickTask(p);
    if (!blocked) return;
    bool ok = service.addDependency(blocker, blocked);
    std::cout << (ok ? "Зависимостта е добавена.\n" : "Неуспех.\n") << "\n";
}

void showLog() {
    Project* p = pickProject();
    if (!p) return;
    Task* t = pickTask(p);
    if (!t) return;
    std::cout << "-- Лог на промените --\n";
    for (const auto& e : t->getChangeLog())
        std::cout << "  " << e.toString() << "\n";
    std::cout << "-- Коментари --\n";
    for (auto c : t->getComments())
        std::cout << "  " << c->toString() << "\n";
    std::cout << "\n";
}

void login() {
    std::cout << "=== Мениджър на задачи (C++) ===\n";
    while (true) {
        std::cout << "Потребителско име (admin / pesho / maria): ";
        std::string name = readLine();
        currentUser = service.findUserByUsername(name);
        if (currentUser) {
            std::cout << "Добре дошъл, " << currentUser->getFullName() << "!\n\n";
            return;
        }
        std::cout << "Няма такъв потребител. Опитай пак.\n";
    }
}

int main() {
    seedData();
    login();

    while (true) {
        std::cout << "-------- МЕНЮ --------\n";
        std::cout << "1. Покажи всички проекти\n";
        std::cout << "2. Покажи задачите на проект\n";
        std::cout << "3. Смени статус на задача\n";
        std::cout << "4. Добави задача\n";
        std::cout << "5. Добави коментар\n";
        std::cout << "6. Филтрирай задачи\n";
        std::cout << "7. Добави зависимост\n";
        std::cout << "8. Покажи лог на задача\n";
        std::cout << "9. Dashboard\n";
        std::cout << "0. Изход\n";
        std::cout << "Избор: ";
        int choice = readInt(0, 9);
        std::cout << "\n";

        switch (choice) {
            case 1: showProjects();    break;
            case 2: showTasksOfProject(); break;
            case 3: changeStatus();    break;
            case 4: addTask();         break;
            case 5: addComment();      break;
            case 6: filterTasks();     break;
            case 7: addDependency();   break;
            case 8: showLog();         break;
            case 9: service.printDashboard(); break;
            case 0: std::cout << "Довиждане!\n"; return 0;
        }
    }
}
