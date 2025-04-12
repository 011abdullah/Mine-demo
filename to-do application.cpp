#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <chrono>
#include <algorithm> // For sort function

using namespace std;

// Enum for task priority
enum Priority {
    LOW = 1,
    MEDIUM,
    HIGH
};

// Task class
class Task {
private:
    static int idCounter;
    int id;
    string title;
    string dueDate;
    string description;
    string reminder;
    string category;
    Priority priority;
    bool completed;

public:
    Task() = default;

    Task(const string& t, const string& date, const string& desc, const string& remind,
         const string& cat, Priority p)
        : title(t), dueDate(date), description(desc), reminder(remind),
          category(cat), priority(p), completed(false) {
        id = ++idCounter;
    }

    static void setIdCounter(int val) {
        idCounter = val;
    }

    int getId() const { return id; }
    string getTitle() const { return title; }
    string getDueDate() const { return dueDate; }
    string getDescription() const { return description; }
    string getReminder() const { return reminder; }
    string getCategory() const { return category; }
    Priority getPriority() const { return priority; }
    bool isCompleted() const { return completed; }

    void markDone() { completed = true; }

    bool isOverdue() const {
        time_t now = time(0);
        tm now_tm = *localtime(&now);

        int y, m, d;
        sscanf(dueDate.c_str(), "%d-%d-%d", &y, &m, &d);

        tm due_tm = {};
        due_tm.tm_year = y - 1900;
        due_tm.tm_mon = m - 1;
        due_tm.tm_mday = d;

        time_t due_time = mktime(&due_tm);
        return !completed && difftime(due_time, now) < 0;
    }

    void displaySummary() const {
        cout << setw(3) << id << " | "
             << (completed ? "[?]" : (isOverdue() ? "[!]" : "[ ]")) << " "
             << setw(20) << title << " | "
             << setw(10) << dueDate << " | "
             << setw(10) << category << " | "
             << priorityToString(priority) << "\n";
    }

    void displayDetails() const {
        cout << "\n?? Task ID: " << id << "\n"
             << "Title      : " << title << "\n"
             << "Due Date   : " << dueDate << (isOverdue() ? " (?Overdue!)" : "") << "\n"
             << "Priority   : " << priorityToString(priority) << "\n"
             << "Completed  : " << (completed ? "Yes" : "No") << "\n"
             << "Reminder   : " << reminder << "\n"
             << "Category   : " << category << "\n"
             << "Description:\n" << description << "\n";
    }

    static string priorityToString(Priority p) {
        switch (p) {
            case LOW: return "Low";
            case MEDIUM: return "Medium";
            case HIGH: return "High";
            default: return "Unknown";
        }
    }

    string serialize() const {
        stringstream ss;
        ss << id << "|" << title << "|" << dueDate << "|" << description << "|"
           << reminder << "|" << category << "|" << priority << "|" << completed;
        return ss.str();
    }

    void deserialize(const string& data) {
        stringstream ss(data);
        string token;
        getline(ss, token, '|'); id = stoi(token);
        getline(ss, title, '|');
        getline(ss, dueDate, '|');
        getline(ss, description, '|');
        getline(ss, reminder, '|');
        getline(ss, category, '|');
        getline(ss, token, '|'); priority = static_cast<Priority>(stoi(token));
        getline(ss, token, '|'); completed = stoi(token);
    }
};

int Task::idCounter = 0;

// ToDoList class
class ToDoList {
private:
    vector<Task> tasks;

public:
    void addTask(const string& title, const string& date, const string& desc,
                 const string& reminder, const string& category, Priority priority) {
        Task newTask(title, date, desc, reminder, category, priority);
        tasks.push_back(newTask);
        cout << "? Task added with ID " << newTask.getId() << "!\n";
    }

    void showTasks() const {
        if (tasks.empty()) {
            cout << "?? No tasks to show.\n";
            return;
        }

        cout << "\nID  | Status Title                | Due Date  | Category   | Priority\n";
        cout << "----------------------------------------------------------------------\n";
        for (const auto& task : tasks) {
            task.displaySummary();
        }
    }

    void showTaskDetails(int id) const {
        for (const auto& task : tasks) {
            if (task.getId() == id) {
                task.displayDetails();
                return;
            }
        }
        cout << "? Task ID not found.\n";
    }

    void markTaskDone(int id) {
        for (auto& task : tasks) {
            if (task.getId() == id) {
                task.markDone();
                cout << "? Task ID " << id << " marked as done.\n";
                return;
            }
        }
        cout << "? Task ID not found.\n";
    }

    void deleteTask(int id) {
        for (auto it = tasks.begin(); it != tasks.end(); ++it) {
            if (it->getId() == id) {
                tasks.erase(it);
                cout << "??? Task ID " << id << " deleted.\n";
                return;
            }
        }
        cout << "? Task ID not found.\n";
    }

    void showProgress() const {
        int total = tasks.size(), completed = 0;
        for (const auto& task : tasks) {
            if (task.isCompleted()) completed++;
        }

        int percent = total > 0 ? (completed * 100 / total) : 0;
        cout << "\n?? Progress: " << completed << "/" << total << " tasks completed (" << percent << "%)\n";
    }

    void showReminders() const {
        cout << "\n? Upcoming Reminders:\n";
        for (const auto& task : tasks) {
            if (!task.isCompleted() && !task.getReminder().empty()) {
                cout << "- [" << task.getTitle() << "] -> " << task.getReminder() << "\n";
            }
        }
    }

    void searchTasks(const string& keyword) const {
        cout << "\n?? Searching for tasks with keyword: " << keyword << "\n";
        bool found = false;
        for (const auto& task : tasks) {
            if (task.getTitle().find(keyword) != string::npos || task.getCategory().find(keyword) != string::npos) {
                task.displaySummary();
                found = true;
            }
        }
        if (!found) {
            cout << "No tasks found with the given keyword.\n";
        }
    }

    void sortTasksByDueDate() {
        sort(tasks.begin(), tasks.end(), [](const Task& t1, const Task& t2) {
            return t1.getDueDate() < t2.getDueDate();
        });
        cout << "Tasks sorted by due date.\n";
    }

    void sortTasksByPriority() {
        sort(tasks.begin(), tasks.end(), [](const Task& t1, const Task& t2) {
            return t1.getPriority() < t2.getPriority();
        });
        cout << "Tasks sorted by priority.\n";
    }

    void filterTasksByCompletion(bool completedStatus) const {
        cout << "\n?? Showing tasks that are " << (completedStatus ? "completed" : "incomplete") << ":\n";
        for (const auto& task : tasks) {
            if (task.isCompleted() == completedStatus) {
                task.displaySummary();
            }
        }
    }

    void saveToFile(const string& filename) const {
        ofstream file(filename);
        for (const auto& task : tasks) {
            file << task.serialize() << "\n";
        }
        file.close();
        cout << "?? Tasks saved to file.\n";
    }

    void loadFromFile(const string& filename) {
        ifstream file(filename);
        string line;
        int maxId = 0;

        while (getline(file, line)) {
            Task task;
            task.deserialize(line);
            tasks.push_back(task);
            if (task.getId() > maxId) maxId = task.getId();
        }

        Task::setIdCounter(maxId);
        file.close();
        cout << "?? Tasks loaded from file.\n";
    }
};

// App class
class App {
private:
    ToDoList todo;

public:
    void run() {
        todo.loadFromFile("tasks.txt");
        todo.showReminders(); // Display reminders on startup

        int choice;
        while (true) {
            cout << "\n===== TO-DO APP MENU =====\n"
                 << "1. Add Task\n"
                 << "2. View All Tasks\n"
                 << "3. View Task Details\n"
                 << "4. Mark Task as Done\n"
                 << "5. Delete Task\n"
                 << "6. Show Progress\n"
                 << "7. Search Tasks\n"
                 << "8. Sort Tasks by Due Date\n"
                 << "9. Sort Tasks by Priority\n"
                 << "10. Filter Tasks by Completion\n"
                 << "11. Exit\n"
                 << "Enter your choice: ";
            cin >> choice;
            cin.ignore();

            switch (choice) {
                case 1: {
                    string title, due, desc, reminder, category;
                    cout << "Enter task title: "; getline(cin, title);
                    cout << "Enter due date (YYYY-MM-DD): "; getline(cin, due);
                    cout << "Enter task description: "; getline(cin, desc);
                    cout << "Enter reminder note/time: "; getline(cin, reminder);
                    cout << "Enter category: "; getline(cin, category);
                    Priority priority;
                    cout << "Enter priority (1 = Low, 2 = Medium, 3 = High): "; 
                    cin >> priority;
                    cin.ignore();
                    todo.addTask(title, due, desc, reminder, category, priority);
                    break;
                }
                case 2:
                    todo.showTasks();
                    break;
                case 3: {
                    int id;
                    cout << "Enter task ID to view details: ";
                    cin >> id;
                    todo.showTaskDetails(id);
                    break;
                }
                case 4: {
                    int id;
                    cout << "Enter task ID to mark as done: ";
                    cin >> id;
                    todo.markTaskDone(id);
                    break;
                }
                case 5: {
                    int id;
                    cout << "Enter task ID to delete: ";
                    cin >> id;
                    todo.deleteTask(id);
                    break;
                }
                case 6:
                    todo.showProgress();
                    break;
                case 7: {
                    string keyword;
                    cout << "Enter keyword to search for tasks: ";
                    cin.ignore();
                    getline(cin, keyword);
                    todo.searchTasks(keyword);
                    break;
                }
                case 8:
                    todo.sortTasksByDueDate();
                    break;
                case 9:
                    todo.sortTasksByPriority();
                    break;
                case 10: {
                    bool completed;
                    cout << "Filter completed tasks? (1 for Yes, 0 for No): ";
                    cin >> completed;
                    todo.filterTasksByCompletion(completed);
                    break;
                }
                case 11:
                    todo.saveToFile("tasks.txt");
                    cout << "Goodbye!\n";
                    return;
                default:
                    cout << "Invalid choice. Please try again.\n";
            }
        }
    }
};

int main() {
    App app;
    app.run();
    return 0;
}

