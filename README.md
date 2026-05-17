# Task Manager – Мениджър на задачи и проекти (C++)

## Как се компилира и стартира

```bash
# Компилиране (от папката task-manager-cpp/)
g++ -std=c++17 -Wall -o task_manager src/main.cpp

# Стартиране
./task_manager          # Linux / Mac
task_manager.exe        # Windows
```

## Функционалности

| # | Функционалност | Статус |
|---|---------------|--------|
| 1 | CRUD проекти и задачи | ✅ |
| 2 | Статуси с валиден преход | ✅ |
| 3 | Назначаване на изпълнители | ✅ |
| 4 | Приоритети и закъснели задачи | ✅ |
| 5 | Тагове | ✅ |
| 6 | Коментари и лог на промените | ✅ |
| 7 | Зависимости (с проверка за цикъл) | ✅ |
| 8 | Филтриране и търсене | ✅ |
| 9 | Dashboard | ✅ |

## Git команди (минимум 5 commits)

```bash
git init
git add .
git commit -m "Add base class hierarchy: BaseEntity, User, Task, Project"

# (направи малка промяна и продължи)
git commit -m "Add RecurringTask subclass"
git commit -m "Add TaskDependency with cycle detection"
git commit -m "Add TaskService with filter and dashboard"
git commit -m "Add CLI menu in main.cpp"

# Виж хешовете:
git log --oneline
```

## Бележка за промени спрямо чекпойнт 1

Добавен е `ChangeLogEntry` (не е в диаграмата, но е логично следствие
от изискването за лог на промените). `TaskService` е нов сервизен клас
за да се раздели бизнес логиката от UI-a.
