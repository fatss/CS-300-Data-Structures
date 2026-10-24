// Advising Assistance Program
// Build: c++ -std=c++11 -Wall -Wextra -pedantic ProjectTwo.cpp -o advising
// CSV format: courseNumber,title[,prerequisiteNumber...], with no header row.

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

struct Course {
    std::string number;
    std::string title;
    std::vector<std::string> prerequisites;
};

std::string trim(const std::string& value) {
    const std::string whitespace = " \t\r\n";
    const std::size_t first = value.find_first_not_of(whitespace);
    if (first == std::string::npos) {
        return "";
    }
    return value.substr(first, value.find_last_not_of(whitespace) - first + 1);
}

std::string normalizeCourseNumber(std::string number) {
    number = trim(number);
    for (char& character : number) {
        character = static_cast<char>(
            std::toupper(static_cast<unsigned char>(character)));
    }
    return number;
}

bool isValidCourseNumber(const std::string& number) {
    return !number.empty() &&
           std::all_of(number.begin(), number.end(), [](unsigned char character) {
               return std::isalnum(character) != 0;
           });
}

// Quoted titles may contain commas; two quotes inside a quoted field mean one quote.
bool parseCsvRow(const std::string& line, std::vector<std::string>& fields) {
    std::size_t position = 0;
    while (true) {
        while (position < line.size() &&
               (line[position] == ' ' || line[position] == '\t')) {
            ++position;
        }

        std::string field;
        if (position < line.size() && line[position] == '"') {
            ++position;
            bool closed = false;
            while (position < line.size()) {
                const char character = line[position++];
                if (character != '"') {
                    field += character;
                } else if (position < line.size() && line[position] == '"') {
                    field += '"';
                    ++position;
                } else {
                    closed = true;
                    break;
                }
            }
            if (!closed) {
                return false;
            }
            while (position < line.size() &&
                   (line[position] == ' ' || line[position] == '\t' ||
                    line[position] == '\r')) {
                ++position;
            }
            if (position < line.size() && line[position] != ',') {
                return false;
            }
        } else {
            while (position < line.size() && line[position] != ',') {
                if (line[position] == '"') {
                    return false;
                }
                field += line[position++];
            }
        }
        fields.push_back(trim(field));
        if (position == line.size()) {
            return true;
        }
        ++position;  // Skip the comma, retaining any empty field that follows it.
    }
}

// Courses stay sorted after loading, allowing binary search for every lookup.
const Course* findCourse(const std::vector<Course>& courses,
                         const std::string& number) {
    const auto found = std::lower_bound(
        courses.begin(), courses.end(), number,
        [](const Course& course, const std::string& key) {
            return course.number < key;
        });
    return found != courses.end() && found->number == number ? &*found : nullptr;
}

bool loadCourses(const std::string& filename, std::vector<Course>& courses) {
    std::ifstream input(filename);
    if (!input) {
        std::cout << "Error: Could not open file \"" << filename << "\".\n";
        return false;
    }

    // Validate temporary data first so a failed reload preserves the current catalog.
    std::vector<Course> loadedCourses;
    std::unordered_set<std::string> courseNumbers;
    std::string line;
    std::size_t lineNumber = 0;
    while (std::getline(input, line)) {
        ++lineNumber;
        if (trim(line).empty()) {
            continue;
        }

        std::vector<std::string> fields;
        if (!parseCsvRow(line, fields) || fields.size() < 2 || fields[1].empty()) {
            std::cout << "Error: Invalid course row on line " << lineNumber
                      << ". Expected courseNumber,title[,prerequisites...].\n";
            return false;
        }

        Course course;
        course.number = normalizeCourseNumber(fields[0]);
        course.title = fields[1];
        if (!isValidCourseNumber(course.number)) {
            std::cout << "Error: Invalid course number on line " << lineNumber << ".\n";
            return false;
        }
        if (!courseNumbers.insert(course.number).second) {
            std::cout << "Error: Duplicate course " << course.number
                      << " on line " << lineNumber << ".\n";
            return false;
        }

        for (std::size_t index = 2; index < fields.size(); ++index) {
            const std::string prerequisite = normalizeCourseNumber(fields[index]);
            if (!isValidCourseNumber(prerequisite) || prerequisite == course.number) {
                std::cout << "Error: Invalid prerequisite on line " << lineNumber << ".\n";
                return false;
            }
            course.prerequisites.push_back(prerequisite);
        }
        loadedCourses.push_back(course);
    }

    if (!input.eof()) {
        std::cout << "Error: Could not finish reading the file.\n";
        return false;
    }
    if (loadedCourses.empty()) {
        std::cout << "Error: The file contains no courses.\n";
        return false;
    }

    // Check references after reading all rows: prerequisites can appear later in the file.
    for (const Course& course : loadedCourses) {
        for (const std::string& prerequisite : course.prerequisites) {
            if (courseNumbers.count(prerequisite) == 0) {
                std::cout << "Error: Prerequisite " << prerequisite << " for "
                          << course.number << " is missing from the file.\n";
                return false;
            }
        }
    }

    std::sort(loadedCourses.begin(), loadedCourses.end(),
              [](const Course& left, const Course& right) {
                  return left.number < right.number;
              });
    courses.swap(loadedCourses);
    std::cout << "Loaded " << courses.size() << " courses successfully.\n";
    return true;
}

void printCourseList(const std::vector<Course>& courses) {
    std::cout << "\nHere is a sample schedule:\n";
    for (const Course& course : courses) {
        std::cout << course.number << ", " << course.title << '\n';
    }
}

void printCourseInformation(const std::vector<Course>& courses,
                            const std::string& number) {
    const Course* course = findCourse(courses, number);
    if (course == nullptr) {
        std::cout << "Error: Course " << number << " was not found.\n";
        return;
    }

    std::cout << '\n' << course->number << ", " << course->title << '\n';
    if (course->prerequisites.empty()) {
        std::cout << "Prerequisites: None\n";
    } else {
        std::cout << "Prerequisites:\n";
        for (const std::string& prerequisite : course->prerequisites) {
            const Course* requiredCourse = findCourse(courses, prerequisite);
            std::cout << "  " << requiredCourse->number << ", "
                      << requiredCourse->title << '\n';
        }
    }
}

int main() {
    std::vector<Course> courses;
    std::cout << "Welcome to the course planner.\n";
    std::string choice;
    while (true) {
        std::cout << "\n1. Load Data Structure.\n"
                  << "2. Print Course List.\n"
                  << "3. Print Course.\n"
                  << "9. Exit.\n\n"
                  << "What would you like to do? ";
        if (!std::getline(std::cin, choice)) {
            break;
        }
        choice = trim(choice);

        if (choice == "9") {
            break;
        } else if (choice == "1") {
            std::cout << "Enter the course data file name: ";
            std::string filename;
            if (!std::getline(std::cin, filename)) {
                break;
            }
            loadCourses(trim(filename), courses);
        } else if (choice == "2" || choice == "3") {
            if (courses.empty()) {
                std::cout << "Error: Load course data using option 1 first.\n";
            } else if (choice == "2") {
                printCourseList(courses);
            } else {
                std::cout << "What course do you want to know about? ";
                std::string number;
                if (!std::getline(std::cin, number)) {
                    break;
                }
                number = normalizeCourseNumber(number);
                if (!isValidCourseNumber(number)) {
                    std::cout << "Error: Enter a course number using letters and digits.\n";
                } else {
                    printCourseInformation(courses, number);
                }
            }
        } else {
            std::cout << "Error: Invalid menu option. Enter 1, 2, 3, or 9.\n";
        }
    }
    std::cout << "Thank you for using the course planner!\n";
    return 0;
}
