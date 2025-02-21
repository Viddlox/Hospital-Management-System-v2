#ifndef RENDER_H
#define RENDER_H

// Standard library headers
#include <iostream>      // For input and output operations (cout, cin, etc.)
#include <string>        // For handling string operations
#include <ctime>         // For time-related functions (time, localtime, etc.)
#include <cstdlib>       // For general utilities (random, memory allocation, etc.)
#include <cstring>       // For C-style string handling (strcpy, strcmp, etc.)
#include <thread>        // For multithreading support
#include <vector>        // For dynamic arrays (std::vector)
#include <sstream>       // For string stream operations (parsing, formatting, etc.)
#include <map>           // For ordered key-value storage
#include <unordered_map> // For hash-based key-value storage (faster lookups)
#include <algorithm>     // For algorithms like sorting, searching, transformations
#include <memory>        // For smart pointers and dynamic memory management
#include <cmath>         // For mathematical functions (pow, sqrt, etc.)

// ncurses headers for terminal-based UI rendering (wide character support)
#include <ncursesw/ncurses.h> // Main ncurses library for UI rendering
#include <ncursesw/form.h>    // For handling form-based UI components
#include <ncursesw/menu.h>    // For creating terminal menus

// Additional C headers for ncurses related string display, buffering, and processing
#include <assert.h> // For debugging assertions (C version)
#include <cassert>  // C++ equivalent of assert.h
#include <ctype.h>  // For character classification functions (C version)
#include <cctype>   // C++ equivalent of ctype.h
#include <math.h>   // C-style math library (prefer <cmath> in C++)

// Signal handling for clean program termination
#include <csignal> // For handling OS-level signals (SIGINT, SIGTERM, etc.)

// Platform-specific headers for handling terminal window size
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h> // Windows-specific API for console control
#else
#include <sys/ioctl.h> // For retrieving terminal window size in UNIX-like OS
#include <unistd.h>    // For UNIX system calls (sleep, close, etc.)
#endif

// Project-specific headers
#include "Admissions.hpp"  // Handles hospital department admissions
#include "UserManager.hpp" // Manages user accounts and authentication
#include "utils.hpp"       // Utility functions for general-purpose operations

// Forward declarations to reduce dependencies
class EventManager; // Handles event-driven programming
class UserManager;  // Manages users (patients, admins)
class User;         // Base class for different user types
class Admin;        // Admin user with extra privileges
class Patient;      // Patient data management class

// Enum representing different screens in the application
enum class Screen
{
    Login,                              // Login screen
    RegistrationAccountPatientScreen,   // Patient registration (account creation)
    RegistrationPersonalPatientScreen,  // Patient registration (personal details)
    RegistrationSelectionPatientScreen, // Patient registration (selection step)
    RegisterAdmin,                      // Admin registration screen
    Dashboard,                          // Main dashboard after login
    Profile,                            // User profile screen
    ProfileAdmissions,                  // Admissions section within profile
    Database,                           // Database view or management screen
    Admission,                          // Admission process screen
    UpdateAccountPatientScreen,         // Update patient account details
    UpdatePersonalPatientScreen,        // Update patient personal information
    UpdateSelectionPatientScreen,       // Update selection process for patients
    UpdateAdminScreen                   // Admin update screen
};

// Struct for defining color themes in the UI
struct Color
{
    int primary = 1;   // Primary color (default: 1)
    int secondary = 2; // Secondary color (default: 2)
    int danger = 3;    // Color for errors or warnings (default: 3)

    // Singleton implementation to ensure a single instance of Color
    static Color &getInstance()
    {
        static Color instance;
        return instance;
    }

    // Prevent copy and assignment
    Color(const Color &) = delete;
    Color &operator=(const Color &) = delete;

private:
    Color() {} // Private constructor to prevent external instantiation
};

// Struct defining control mappings for navigation
struct Controls
{
    std::string header = " Control Guide "; // Header title for control guide
    std::vector<std::string> controlArr = {
        "|---- Navigation ----|",
        "[+] (Arrow keys) - (Move)",
        "[+] (Ctrl + B) - (Back)",
        "[+] (Esc / Ctrl + C) - (Exit)",
        "[+] (Enter) - (Select/Proceed)",
        "|---- Pagination ----|",
        "[+] (Tab) - (Switch filters)",
        "[+] (PgDn) - (Next Page)",
        "[+] (PgUp) - (Prev Page)",
        "[+] (+) - (Add record)"}; // Control instructions
};

// Struct representing a user profile
struct Profile
{
    std::shared_ptr<User> user; // Pointer to associated user data
    Screen prevScreen;          // Tracks the previous screen visited

    std::vector<std::string> patientSubheaderArr = {
        "Account Information",
        "Admissions"}; // Subsections under the profile page

    std::vector<std::pair<std::string, std::string>> fieldValues; // Stores field name-value pairs

    std::string searchQuery = ""; // Search term for filtering records

    // 2D list matrix (rows represent records)
    std::vector<std::vector<std::string>> listMatrix;

    int currentPage = 0;  // Current page number
    int pageSize = 10;    // Number of records displayed per page
    int totalPages = 0;   // Total number of pages
    int selectedRow = -1; // Currently selected row (-1 means none)
    int selectedCol = 2;  // Default column selection

    // Resets profile-related attributes
    void reset()
    {
        currentPage = 0;
        searchQuery.clear();
        selectedRow = -1;
        selectedCol = 2;
        fieldValues.clear();
    }

    // Generates list matrix from admission records
    void generateListMatrix(const std::map<Admissions::Department, std::vector<std::string>> &records)
    {
        std::vector<std::vector<std::string>> listMatrixTmp;

        listMatrix.clear();
        for (const auto &record : records) // Iterate over departments
        {
            for (const auto &dateTime : record.second) // Iterate over admission dates
            {
                listMatrixTmp.push_back({Admissions::departmentToString(record.first),
                                         dateTime,
                                         "[Delete]"}); // Represents an admission entry
            }
        }
        // Sort by date time of subarray dateTime strings in lexicographical order (descending AKA newest first)
        std::sort(listMatrixTmp.begin(), listMatrixTmp.end(), [](const std::vector<std::string> &a, const std::vector<std::string> &b)
                  { return std::greater<std::string>()(a[1], b[1]); });

        // Repopulate listMatrix after correctly ordering records by latest admission date
        for (const auto &record : listMatrixTmp)
        {
            listMatrix.push_back(record);
        }
        totalPages = listMatrix.empty() ? 0 : (listMatrix.size() - 1) / pageSize + 1;
    }

    // Retrieves the current page of records
    std::vector<std::vector<std::string>> getCurrentPage()
    {
        int startIndex = currentPage * pageSize;
        int endIndex = std::min(startIndex + pageSize, static_cast<int>(listMatrix.size()));
        std::vector<std::vector<std::string>> res;

        for (int i = startIndex; i < endIndex; i++)
        {
            res.push_back(listMatrix[i]);
        }
        return res;
    }

    // Performs a search on admission records
    std::map<Admissions::Department, std::vector<std::string>> search(const std::string &query, const std::map<Admissions::Department, std::vector<std::string>> &records)
    {
        std::string filteredQuery = query.empty() ? "" : toLower(trim(query));
        std::map<Admissions::Department, std::vector<std::string>> res;

        for (const auto &record : records)
        {
            std::vector<std::string> matchedDates;

            if (filteredQuery.empty() || toLower(Admissions::departmentToString(record.first)).find(filteredQuery) != std::string::npos)
            {
                matchedDates = record.second;
            }
            else
            {
                for (const auto &dateTime : record.second)
                {
                    if (toLower(dateTime).find(filteredQuery) != std::string::npos)
                    {
                        matchedDates.push_back(dateTime);
                    }
                }
            }
            if (!matchedDates.empty())
            {
                res[record.first] = matchedDates;
            }
        }
        return res;
    }

    // Singleton Implementation
    static Profile &getInstance()
    {
        static Profile instance;
        return instance;
    }

    // Prevent copy and assignment
    Profile(const Profile &) = delete;
    Profile &operator=(const Profile &) = delete;

private:
    Profile() {} // Private constructor to enforce singleton pattern
};

// Struct representing hospital admissions
struct Admission
{
    std::string searchQuery = ""; // Stores the current search query for filtering departments

    // Reference list containing all hospital departments and their string representations
    const std::vector<std::pair<Admissions::Department, std::string>> listRef = {
        {Admissions::Department::Emergency, Admissions::departmentToString(Admissions::Department::Emergency)},
        {Admissions::Department::InternalMedicine, Admissions::departmentToString(Admissions::Department::InternalMedicine)},
        {Admissions::Department::OBGYN, Admissions::departmentToString(Admissions::Department::OBGYN)},
        {Admissions::Department::Pediatrics, Admissions::departmentToString(Admissions::Department::Pediatrics)},
        {Admissions::Department::Surgery, Admissions::departmentToString(Admissions::Department::Surgery)},
        {Admissions::Department::Cardiology, Admissions::departmentToString(Admissions::Department::Cardiology)},
        {Admissions::Department::Neurology, Admissions::departmentToString(Admissions::Department::Neurology)},
        {Admissions::Department::Oncology, Admissions::departmentToString(Admissions::Department::Oncology)},
        {Admissions::Department::Orthopedics, Admissions::departmentToString(Admissions::Department::Orthopedics)},
        {Admissions::Department::Pulmonology, Admissions::departmentToString(Admissions::Department::Pulmonology)},
        {Admissions::Department::Psychiatry, Admissions::departmentToString(Admissions::Department::Psychiatry)},
        {Admissions::Department::Nephrology, Admissions::departmentToString(Admissions::Department::Nephrology)},
        {Admissions::Department::Gastroenterology, Admissions::departmentToString(Admissions::Department::Gastroenterology)},
        {Admissions::Department::InfectiousDiseases, Admissions::departmentToString(Admissions::Department::InfectiousDiseases)},
        {Admissions::Department::Endocrinology, Admissions::departmentToString(Admissions::Department::Endocrinology)},
        {Admissions::Department::Urology, Admissions::departmentToString(Admissions::Department::Urology)},
        {Admissions::Department::Dermatology, Admissions::departmentToString(Admissions::Department::Dermatology)},
        {Admissions::Department::Rheumatology, Admissions::departmentToString(Admissions::Department::Rheumatology)},
        {Admissions::Department::ENT, Admissions::departmentToString(Admissions::Department::ENT)},
        {Admissions::Department::Ophthalmology, Admissions::departmentToString(Admissions::Department::Ophthalmology)},
        {Admissions::Department::PhysicalRehab, Admissions::departmentToString(Admissions::Department::PhysicalRehab)}};

    std::vector<std::pair<Admissions::Department, std::string>> list; // List of departments filtered by search

    int currentPage = 0;                       // Current page index for pagination
    int pageSize = 10;                         // Number of items displayed per page
    int totalPages = 0;                        // Total number of pages available
    int selectedRow = -1;                      // Index of the currently selected row, -1 means none selected
    Screen prevScreen;                         // Stores the previous screen for navigation
    Admissions::Department selectedDepartment; // Stores the selected department

    // Resets search query and pagination state
    void reset()
    {
        currentPage = 0;
        searchQuery.clear();
        selectedRow = -1;
    }

    // Retrieves the records for the current page
    std::vector<std::pair<Admissions::Department, std::string>> getCurrentPage()
    {
        int startIndex = currentPage * pageSize;
        int endIndex = std::min(startIndex + pageSize, static_cast<int>(list.size()));
        std::vector<std::pair<Admissions::Department, std::string>> res;

        for (int i = startIndex; i < endIndex; i++)
        {
            res.push_back(list[i]);
        }
        return res;
    }

    // Searches for departments matching the query
    std::vector<std::pair<Admissions::Department, std::string>> search(const std::string &query)
    {
        std::string filteredQuery = query.empty() ? "" : toLower(trim(query)); // Normalize query for case-insensitive search
        std::vector<std::pair<Admissions::Department, std::string>> res;

        for (const auto &department : listRef)
        {
            if (filteredQuery.empty() || toLower(department.second).find(filteredQuery) != std::string::npos)
            {
                res.push_back(department); // Add matching department to the result list
            }
        }
        return res;
    }

    // Updates the department list based on the provided records
    void generateList(const std::vector<std::pair<Admissions::Department, std::string>> &records)
    {
        list.clear();
        for (const auto &record : records)
        {
            list.push_back(record);
        }
        totalPages = list.empty() ? 0 : (list.size() - 1) / pageSize + 1; // Compute total pages
    }

    // Singleton pattern - Ensures only one instance of Admission exists
    static Admission &getInstance()
    {
        static Admission instance;
        return instance;
    }

    // Delete copy constructor and assignment operator to prevent unintentional copying
    Admission(const Admission &) = delete;
    Admission &operator=(const Admission &) = delete;

private:
    Admission() {} // Private constructor to restrict instantiation
};

// Struct to handle administrator registration details
struct RegistrationAdmin
{
    // Account information fields
    std::string username = "";      // Stores admin's username
    std::string password = "";      // Stores admin's password
    std::string email = "";         // Stores admin's email
    std::string fullName = "";      // Stores admin's full name
    std::string contactNumber = ""; // Stores admin's contact number

    // Function to reset all admin account details
    void reset()
    {
        username = "";
        password = "";
        email = "";
        fullName = "";
        contactNumber = "";
    }

    // Singleton Implementation - Ensures only one instance of RegistrationAdmin exists
    static RegistrationAdmin &getInstance()
    {
        static RegistrationAdmin instance; // Static instance of the class
        return instance;
    }

    // Delete copy constructor and assignment operator to prevent accidental copies
    RegistrationAdmin(const RegistrationAdmin &) = delete;
    RegistrationAdmin &operator=(const RegistrationAdmin &) = delete;

private:
    // Private constructor to prevent external instantiation
    RegistrationAdmin() {}
};

// Struct to handle patient registration details
struct RegistrationPatient
{
    // Predefined options for patient selection (Gender, Religion, Race, etc.)
    std::vector<std::string> genderArr = {
        "Male",
        "Female",
    };
    std::vector<std::string> religionArr = {
        "Islam",
        "Buddhism",
        "Christianity",
        "Hinduism",
        "Other"};
    std::vector<std::string> raceArr = {
        "Malay",
        "Chinese",
        "Indian",
        "Other"};
    std::vector<std::string> maritalStatusArr = {
        "Single",
        "Married",
        "Divorced",
        "Widowed",
        "Separated"};
    std::vector<std::string> nationalityArr = {
        "Malaysian",
        "Other",
    };

    // Labels for each selection menu
    std::vector<std::string> labelArr = {
        "Gender",
        "Religion",
        "Race",
        "Marital Status",
        "Nationality"};

    // Stores all selection menus in a single vector for easy navigation
    std::vector<std::vector<std::string>> menuArrs = {
        genderArr,
        religionArr,
        raceArr,
        maritalStatusArr,
        nationalityArr};

    // Account-related fields
    std::string username = "";      // Patient's username
    std::string password = "";      // Patient's password
    std::string email = "";         // Patient's email
    std::string address = "";       // Patient's home address
    std::string contactNumber = ""; // Patient's contact number

    // Personal information fields
    std::string fullName = "";               // Full name of the patient
    std::string identityCardNumber = "";     // Identity card number (e.g., national ID)
    std::string height = "";                 // Patient's height
    std::string weight = "";                 // Patient's weight
    std::string emergencyContactNumber = ""; // Emergency contact's phone number
    std::string emergencyContactName = "";   // Emergency contact's name

    // Selection fields for predefined options
    std::string gender = "";        // Stores selected gender
    std::string race = "";          // Stores selected race
    std::string religion = "";      // Stores selected religion
    std::string maritalStatus = ""; // Stores selected marital status
    std::string nationality = "";   // Stores selected nationality

    // Navigation variables
    std::vector<int> selectedIndices; // Stores indices of selected options for each menu
    int currentMenu = 0;              // Keeps track of which menu the user is currently on

    // Function to reset all patient information
    void reset()
    {
        // Reset account details
        username = "";
        password = "";
        email = "";
        address = "";
        contactNumber = "";

        // Reset personal information
        fullName = "";
        identityCardNumber = "";
        height = "";
        weight = "";
        emergencyContactNumber = "";
        emergencyContactName = "";

        // Reset selection details
        gender = "";
        race = "";
        religion = "";
        maritalStatus = "";
        nationality = "";

        // Reset selection indices and menu navigation
        selectedIndices = std::vector<int>(menuArrs.size(), 0);
        currentMenu = 0;
    }

    // Singleton Implementation - Ensures only one instance of RegistrationPatient exists
    static RegistrationPatient &getInstance()
    {
        static RegistrationPatient instance; // Static instance of the class
        return instance;
    }

    // Delete copy constructor and assignment operator to prevent accidental copies
    RegistrationPatient(const RegistrationPatient &) = delete;
    RegistrationPatient &operator=(const RegistrationPatient &) = delete;

private:
    // Private constructor to prevent external instantiation
    RegistrationPatient()
    {
        selectedIndices = std::vector<int>(menuArrs.size(), 0);
    }
};

// Struct to manage the dashboard menu options
struct Dashboard
{
    // Available menu options in the dashboard
    std::vector<std::string> optionsArr = {
        "Database", // View and manage records
        "Profile",  // View and edit user profile
        "Log Out"   // Exit the system
    };

    int selectedIndex = 0; // Stores the currently selected menu option

    // Function to reset the selected index to the default value
    void reset()
    {
        selectedIndex = 0;
    }

    // Singleton Implementation - Ensures only one instance of Dashboard exists
    static Dashboard &getInstance()
    {
        static Dashboard instance; // Static instance of the class
        return instance;
    }

    // Delete copy constructor and assignment operator to prevent accidental copies
    Dashboard(const Dashboard &) = delete;
    Dashboard &operator=(const Dashboard &) = delete;

private:
    Dashboard() {} // Private constructor to prevent multiple instances
};

// Struct to manage the database records for patients and admins
struct Database
{
    // Subheaders representing different categories in the database
    std::vector<std::string> subheaderArr = {
        "Patient records", // Section for patient-related data
        "Admin records"    // Section for admin-related data
    };

    // Enum to represent filtering options (either patient or admin)
    enum class Filter
    {
        patient, // Filter for patient records
        admin    // Filter for admin records
    };

    std::string searchQuery = "";           // Stores the current search query
    Filter currentFilter = Filter::patient; // Default filter is set to "patient"

    // Matrix structure to store records (each row represents a record)
    std::vector<std::vector<std::string>> listMatrixPatient, listMatrixAdmin, listMatrixCurrent;

    // Stores raw patient and admin records (pair of ID and other info)
    std::vector<std::pair<std::string, std::string>> patientRecords, adminRecords;

    int currentPage = 0;       // Current page number for pagination
    int pageSize = 10;         // Number of records displayed per page
    int totalPagesAdmin = 0;   // Total pages for admin records
    int totalPagesPatient = 0; // Total pages for patient records
    int selectedRow = -1;      // Currently selected row index (-1 means no selection)
    int selectedCol = 1;       // Currently selected column index

    // Function to reset the database filters and selection
    void reset()
    {
        currentPage = 0;
        currentFilter = Filter::patient;
        searchQuery = "";
        selectedRow = -1;
        selectedCol = 1;
    }

    // Function to generate the patient record matrix with action buttons
    void generateListMatrixPatient(const std::vector<std::pair<std::string, std::string>> &records)
    {
        listMatrixPatient.clear();
        for (const auto &record : records)
        {
            listMatrixPatient.push_back({record.first, "[View]", "[Update]", "[Delete]", record.second});
        }
        totalPagesPatient = listMatrixPatient.empty() ? 0 : (listMatrixPatient.size() - 1) / pageSize + 1;
    }

    // Function to generate the admin record matrix with action buttons
    void generateListMatrixAdmin(const std::vector<std::pair<std::string, std::string>> &records)
    {
        UserManager &userManager = UserManager::getInstance();
        auto currentUser = userManager.getCurrentUser();

        listMatrixAdmin.clear();
        for (const auto &record : records)
        {
            if (record.second == currentUser->getId())
            {
                listMatrixAdmin.push_back({record.first + " (me)", "[View]", "[Update]", "[Delete]", record.second});
            }
            else
            {
                listMatrixAdmin.push_back({record.first, "[View]", "[Update]", "[Delete]", record.second});
            }
        }
        totalPagesAdmin = listMatrixAdmin.empty() ? 0 : (listMatrixAdmin.size() - 1) / pageSize + 1;
    }

    // Function to return the current page of admin records
    std::vector<std::vector<std::string>> getCurrentPageAdmin()
    {
        int startIndex = currentPage * pageSize;
        int endIndex = std::min(startIndex + pageSize, static_cast<int>(listMatrixAdmin.size()));
        std::vector<std::vector<std::string>> res;

        for (int i = startIndex; i < endIndex; i++)
        {
            res.push_back(listMatrixAdmin[i]);
        }
        return res;
    }

    // Function to return the current page of patient records
    std::vector<std::vector<std::string>> getCurrentPagePatient()
    {
        int startIndex = currentPage * pageSize;
        int endIndex = std::min(startIndex + pageSize, static_cast<int>(listMatrixPatient.size()));

        std::vector<std::vector<std::string>> res;

        for (int i = startIndex; i < endIndex; i++)
        {
            res.push_back(listMatrixPatient[i]);
        }
        return res;
    }

    // Singleton Implementation - Ensures only one instance of Database exists
    static Database &getInstance()
    {
        static Database instance; // Static instance of the class
        return instance;
    }

    // Delete copy constructor and assignment operator to prevent accidental copies
    Database(const Database &) = delete;
    Database &operator=(const Database &) = delete;

private:
    Database() {} // Private constructor to prevent multiple instances
};

// Struct to manage updating patient records
struct UpdatePatient
{
    std::shared_ptr<User> user;                                                       // Pointer to the user object
    std::unordered_map<std::string, std::pair<std::string, std::string>> fieldValues; // Stores field name with previous and current values

    // Resets all field values
    void reset()
    {
        fieldValues.clear();
    }

    // Populates the field values from a given patient object
    void populateValues(std::shared_ptr<Patient> patient)
    {
        if (!patient)
            return;

        fieldValues = {
            {"username", {patient->username, patient->username}},
            {"password", {patient->password, patient->password}},
            {"fullName", {patient->fullName, patient->fullName}},
            {"religion", {patient->religion, patient->religion}},
            {"nationality", {patient->nationality, patient->nationality}},
            {"identityCardNumber", {patient->identityCardNumber, patient->identityCardNumber}},
            {"maritalStatus", {patient->maritalStatus, patient->maritalStatus}},
            {"gender", {patient->gender, patient->gender}},
            {"race", {patient->race, patient->race}},
            {"contactNumber", {patient->contactNumber, patient->contactNumber}},
            {"emergencyContactNumber", {patient->emergencyContactNumber, patient->emergencyContactNumber}},
            {"emergencyContactName", {patient->emergencyContactName, patient->emergencyContactName}},
            {"email", {patient->email, patient->email}},
            {"address", {patient->address, patient->address}},
            {"height", {patient->height, patient->height}},
            {"weight", {patient->weight, patient->weight}},
        };
    }

    // Updates a specific field with a new value
    void updateFieldValue(const std::string &fieldName, const std::string &newValue)
    {
        if (fieldValues.find(fieldName) != fieldValues.end())
        {
            fieldValues[fieldName].second = newValue; // Update only the current value
        }
    }

    // Handles updating the patient's record in the UserManager
    void handleUpdatePatient(const std::string &userId)
    {
        UserManager &userManager = UserManager::getInstance();
        auto patient = std::dynamic_pointer_cast<Patient>(userManager.getUserById(userId));

        for (const auto &entry : fieldValues)
        {
            const std::string &fieldName = entry.first;
            const std::string &prev = entry.second.first;  // Old value
            const std::string &curr = entry.second.second; // New value

            if (prev != curr) // Update only if there is a change
            {
                if (fieldName == "identityCardNumber") // If IC number changes, recalculate age
                {
                    int newAge = calculateAge(curr);
                    userManager.updateUser(userId, "age", std::to_string(newAge));
                }

                if (fieldName == "height" || fieldName == "weight") // If height or weight changes, recalculate BMI
                {
                    double newBMI = calculateBMI(fieldValues["weight"].second, fieldValues["height"].second);
                    userManager.updateUser(userId, "bmi", std::to_string(newBMI));
                }

                userManager.updateUser(userId, fieldName, curr); // Update field in database
            }
        }
    }

    // Singleton Implementation - Ensures only one instance exists
    static UpdatePatient &getInstance()
    {
        static UpdatePatient instance;
        return instance;
    }

    // Delete copy constructor and assignment operator to prevent accidental copies
    UpdatePatient(const UpdatePatient &) = delete;
    UpdatePatient &operator=(const UpdatePatient &) = delete;

private:
    UpdatePatient() {} // Private constructor to prevent multiple instances
};

// Struct to manage updating admin records
struct UpdateAdmin
{
    std::shared_ptr<User> user;                                                       // Pointer to the user object
    std::unordered_map<std::string, std::pair<std::string, std::string>> fieldValues; // Stores field name with previous and current values

    // Resets all field values
    void reset()
    {
        fieldValues.clear();
    }

    // Populates the field values from a given admin object
    void populateValues(std::shared_ptr<Admin> admin)
    {
        if (!admin)
            return;

        fieldValues = {
            {"username", {admin->username, admin->username}},
            {"password", {admin->password, admin->password}},
            {"fullName", {admin->fullName, admin->fullName}},
            {"email", {admin->email, admin->email}},
            {"contactNumber", {admin->contactNumber, admin->contactNumber}},
        };
    }

    // Updates a specific field with a new value
    void updateFieldValue(const std::string &fieldName, const std::string &newValue)
    {
        if (fieldValues.find(fieldName) != fieldValues.end())
        {
            fieldValues[fieldName].second = newValue; // Update only the current value
        }
    }

    // Handles updating the admin's record in the UserManager
    void handleUpdateAdmin(const std::string &userId)
    {
        UserManager &userManager = UserManager::getInstance();

        for (const auto &entry : fieldValues)
        {
            const std::string &fieldName = entry.first;
            const std::string &prev = entry.second.first;  // Old value
            const std::string &curr = entry.second.second; // New value

            if (prev != curr) // Update only if there is a change
            {
                userManager.updateUser(userId, fieldName, curr); // Update field in database
            }
        }
    }

    // Singleton Implementation - Ensures only one instance exists
    static UpdateAdmin &getInstance()
    {
        static UpdateAdmin instance;
        return instance;
    }

    // Delete copy constructor and assignment operator to prevent accidental copies
    UpdateAdmin(const UpdateAdmin &) = delete;
    UpdateAdmin &operator=(const UpdateAdmin &) = delete;

private:
    UpdateAdmin() {} // Private constructor to prevent multiple instances
};

/* FUNCTION DECLARATIONS */

/* 1. HELPERS */

// Clears the screen, usually used when refreshing or switching views.
void clearScreen();

// Initializes color settings for the UI to improve readability and design.
void initializeColors();

// Handles the exit logic, such as saving data or performing clean-up tasks when the program ends.
void exitHandler(FORM *form, FIELD **fields, std::vector<WINDOW *> &windows);

// Handles the navigation logic for forms during registration or update, including validation, clean-up tasks, and switching between screens.
void navigationHandler(FORM *form, FIELD **fields, std::vector<WINDOW *> &windows, Screen screen);

// Validates the form fields to ensure the user input is correct before submission.
bool validateFields(FIELD **fields);

// Submits the patient registration form after successful validation.
bool submitRegistrationPatient();

// Submits the admin registration form after successful validation.
bool submitRegistrationAdmin(RegistrationAdmin &reg);

// Handles controls for the database management interface, such as CRUD operations on data.
void handleDatabaseControls(WINDOW *win_form, WINDOW *win_body);

// Handles controls for menu components.
void driver_form(int ch, FORM *form, FIELD **fields, WINDOW *win_form, WINDOW *win_body,
                 std::function<void()> exitHandler, std::function<void()> navigationHandler);

/* 2. COMMONS */

// Renders the header section of the UI (like title or navigation bar).
void renderHeader();

// Renders the control information, such as key bindings or instructions for navigating the interface.
void renderControlInfo();

// Renders a horizontal menu stack, likely used for navigating between options in the UI.
void renderHorizontalMenuStack(WINDOW *win, const std::vector<std::string> &items, const std::string &title, int y_offset, int &selected_index, int start_x);

/* 3. SCREENS */

// Renders the login screen where users can input their credentials.
void renderLoginScreen();

// Renders the dashboard screen, typically shown after logging in.
void renderDashboardScreen();

// Handles various options available on the dashboard, like navigating to different sections.
void handleDashboardOptions();

// Renders the registration screen for an admin user.
void renderRegistrationScreenAdmin();

// Renders the initial registration screen for an admin to create a patient account.
void renderRegistrationAccountPatientScreen();

// Renders the personal details screen for the patient during registration.
void renderRegistrationPersonalPatientScreen();

// Renders selection menus for further personal details of a patient during registration.
void renderRegistrationSelectionPatientScreen();

// Renders the database screen, possibly for admin or authorized users to manage and view data.
void renderDatabaseScreen();

// Renders the user profile screen, typically displaying personal information and activity.
void renderProfileScreen();

// Renders the profile admissions screen, likely for reviewing or updating admission records of a patient.
void renderProfileAdmissionsScreen();

// Renders the admission screen, where users (admins or patients) can input or view admission information.
void renderAdmissionScreen();

// Renders the update account screen for patients to modify their registration details.
void renderUpdateAccountPatientScreen();

// Renders the update personal information screen for patients to modify their personal details.
void renderUpdatePersonalPatientScreen();

// Renders the update selection screen, which could be for choosing which section or part of the account to update.
void renderUpdateSelectionPatientScreen();

// Renders the update admin screen, used for admins to manage user data and permissions.
void renderUpdateAdminScreen();

#endif // RENDER_H
