#pragma once
#include <string>

enum class TaskStatus {
    TODO,
    IN_PROGRESS,
    IN_REVIEW,
    DONE,
    CANCELLED
};

enum class Priority {
    CRITICAL,
    HIGH,
    MEDIUM,
    LOW
};

inline std::string taskStatusToString(TaskStatus s) {
    switch (s) {
        case TaskStatus::TODO:        return "To Do";
        case TaskStatus::IN_PROGRESS: return "In Progress";
        case TaskStatus::IN_REVIEW:   return "In Review";
        case TaskStatus::DONE:        return "Done";
        case TaskStatus::CANCELLED:   return "Cancelled";
        default:                      return "Unknown";
    }
}

inline std::string priorityToString(Priority p) {
    switch (p) {
        case Priority::CRITICAL: return "Критичен";
        case Priority::HIGH:     return "Висок";
        case Priority::MEDIUM:   return "Среден";
        case Priority::LOW:      return "Нисък";
        default:                 return "Unknown";
    }
}

// Проверява дали преходът между статуси е валиден
inline bool canTransition(TaskStatus from, TaskStatus to) {
    if (to == TaskStatus::CANCELLED) return true;
    switch (from) {
        case TaskStatus::TODO:        return to == TaskStatus::IN_PROGRESS;
        case TaskStatus::IN_PROGRESS: return to == TaskStatus::IN_REVIEW;
        case TaskStatus::IN_REVIEW:   return to == TaskStatus::DONE;
        default:                      return false;
    }
}
