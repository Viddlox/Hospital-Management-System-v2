
#ifndef RENDER_H
#define RENDER_H

#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <ncursesw/ncurses.h>
#include <cstring>
#include <thread>
#include <ncursesw/form.h>
#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <cassert>
#include <vector>
#include <sstream>
#include <ncursesw/menu.h>
#include <cctype>
#include <math.h>
#include <algorithm>
#include <memory>
#include <map>
#include "Admissions.hpp"
#include "UserManager.hpp"
#include "utils.hpp"
#include <unordered_map>

#include <csignal>
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif

// Forward declarations
class EventManager;
class UserManager;
class User;
class Admin;
class Patient;

enum class Screen
{
    Login,
    RegistrationAccountPatientScreen,
    RegistrationPersonalPatientScreen,
    RegistrationSelectionPatientScreen,
    RegisterAdmin,
    Dashboard,
    Profile,
    ProfileAdmissions,
    Database,
    Admission,
    UpdateAccountPatientScreen,
    UpdatePersonalPatientScreen,
    UpdateSelectionPatientScreen,
    UpdateAdminScreen
};

struct Color
{
    int primary = 1;
    int secondary = 2;
    int danger = 3;

    // Singleton Implementation
    static Color &getInstance()
    {
        static Color instance;
        return instance;
    }

    // Delete copy constructor and assignment operator to prevent accidental copies
    Color(const Color &) = delete;
    Color &operator=(const Color &) = delete;

private:
    Color() {} // Private constructor to prevent multiple instances
};

struct Controls
{
    std::string header = " Control Guide ";
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
        "[+] (+) - (Add record)"};
};

struct Profile
{
    std::shared_ptr<User> user;
    Screen prevScreen;

    std::vector<std::string> patientSubheaderArr = {
        "Account Information",
        "Admissions"};

    std::vector<std::pair<std::string, std::string>> fieldValues;

    std::string searchQuery = "";

    // Matrix structure (rows are records)
    std::vector<std::vector<std::string>> listMatrix;

    int currentPage = 0;
    int pageSize = 10; // Number of records per page
    int totalPages = 0;
    int selectedRow = -1;
    int selectedCol = 2;

    void reset()
    {
        currentPage = 0;
        searchQuery.clear();
        selectedRow = -1;
        selectedCol = 2;
        fieldValues.clear();
    }

    void generateListMatrix(const std::map<Admissions::Department, std::vector<std::string>> &records)
    {
        listMatrix.clear();
        for (const auto &record : records) // Iterate over departments
        {
            for (const auto &dateTime : record.second) // Iterate over admission dates
            {
                listMatrix.push_back({Admissions::departmentToString(record.first),
                                      dateTime,
                                      "[Delete]"});
            }
        }
        totalPages = listMatrix.empty() ? 0 : (listMatrix.size() - 1) / pageSize + 1;
    }

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

    // Delete copy constructor and assignment operator to prevent accidental copies
    Profile(const Profile &) = delete;
    Profile &operator=(const Profile &) = delete;

private:
    Profile() {} // Private constructor to prevent multiple instances
};

struct Admission
{
    std::string searchQuery = "";

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

    std::vector<std::pair<Admissions::Department, std::string>> list;

    int currentPage = 0;
    int pageSize = 10;
    int totalPages = 0;
    int selectedRow = -1;
    Screen prevScreen;
    Admissions::Department selectedDepartment;

    void reset()
    {
        currentPage = 0;
        searchQuery.clear();
        selectedRow = -1;
    }

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

    std::vector<std::pair<Admissions::Department, std::string>> search(const std::string &query)
    {
        std::string filteredQuery = query.empty() ? "" : toLower(trim(query));
        std::vector<std::pair<Admissions::Department, std::string>> res;

        for (const auto &department : listRef)
        {
            if (filteredQuery.empty() || toLower(department.second).find(filteredQuery) != std::string::npos)
            {
                res.push_back(department);
            }
        }
        return res;
    }

    void generateList(const std::vector<std::pair<Admissions::Department, std::string>> &records)
    {
        list.clear();
        for (const auto &record : records)
        {
            list.push_back(record);
        }
        totalPages = list.empty() ? 0 : (list.size() - 1) / pageSize + 1;
    }

    // Singleton Implementation
    static Admission &getInstance()
    {
        static Admission instance;
        return instance;
    }

    // Delete copy constructor and assignment operator to prevent accidental copies
    Admission(const Admission &) = delete;
    Admission &operator=(const Admission &) = delete;

private:
    Admission() {} // Private constructor to prevent multiple instances
};

struct RegistrationAdmin
{
    // account information
    std::string username = "";
    std::string password = "";
    std::string email = "";
    std::string fullName = "";
    std::string contactNumber = "";

    void reset()
    {
        username = "";
        password = "";
        email = "";
        fullName = "";
        contactNumber = "";
    }
    // Singleton Implementation
    static RegistrationAdmin &getInstance()
    {
        static RegistrationAdmin instance;
        return instance;
    }

    // Delete copy constructor and assignment operator to prevent accidental copies
    RegistrationAdmin(const RegistrationAdmin &) = delete;
    RegistrationAdmin &operator=(const RegistrationAdmin &) = delete;

private:
    RegistrationAdmin() {} // Private constructor to prevent multiple instances
};

struct RegistrationPatient
{
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
    std::vector<std::string> labelArr = {
        "Gender",
        "Religion",
        "Race",
        "Marital Status",
        "Nationality"};
    std::vector<std::vector<std::string>> menuArrs = {
        genderArr,
        religionArr,
        raceArr,
        maritalStatusArr,
        nationalityArr};

    // account information
    std::string username = "";
    std::string password = "";
    std::string email = "";
    std::string address = "";
    std::string contactNumber = "";

    // personal info
    std::string fullName = "";
    std::string identityCardNumber = "";
    std::string height = "";
    std::string weight = "";
    std::string emergencyContactNumber = "";
    std::string emergencyContactName = "";

    // selection
    std::string gender = "";
    std::string race = "";
    std::string religion = "";
    std::string maritalStatus = "";
    std::string nationality = "";
    std::vector<int> selectedIndices;
    int currentMenu = 0;
    void reset()
    {
        username = "";
        password = "";
        email = "";
        address = "";
        contactNumber = "";
        fullName = "";
        identityCardNumber = "";
        height = "";
        weight = "";
        emergencyContactNumber = "";
        emergencyContactName = "";
        gender = "";
        race = "";
        religion = "";
        maritalStatus = "";
        nationality = "";
        selectedIndices = std::vector<int>(menuArrs.size(), 0);
        currentMenu = 0;
    }
    // Singleton Implementation
    static RegistrationPatient &getInstance()
    {
        static RegistrationPatient instance;
        return instance;
    }

    // Delete copy constructor and assignment operator to prevent accidental copies
    RegistrationPatient(const RegistrationPatient &) = delete;
    RegistrationPatient &operator=(const RegistrationPatient &) = delete;

private:
    RegistrationPatient() { selectedIndices = std::vector<int>(menuArrs.size(), 0); } // Private constructor to prevent multiple instances
};

struct Dashboard
{
    std::vector<std::string> optionsArr = {
        "Database",
        "Profile",
        "Log Out"};
    int selectedIndex = 0;
    void reset()
    {
        selectedIndex = 0;
    }
    // Singleton Implementation
    static Dashboard &getInstance()
    {
        static Dashboard instance;
        return instance;
    }

    // Delete copy constructor and assignment operator to prevent accidental copies
    Dashboard(const Dashboard &) = delete;
    Dashboard &operator=(const Dashboard &) = delete;

private:
    Dashboard() {} // Private constructor to prevent multiple instances
};

struct Database
{
    std::vector<std::string> subheaderArr = {
        "Patient records",
        "Admin records"};
    enum class Filter
    {
        patient,
        admin
    };
    std::string searchQuery = "";
    Filter currentFilter = Filter::patient;

    // Matrix structure (rows are records)
    std::vector<std::vector<std::string>> listMatrixPatient, listMatrixAdmin, listMatrixCurrent;
    std::vector<std::pair<std::string, std::string>> patientRecords, adminRecords;

    int currentPage = 0;
    int pageSize = 10; // Number of records per page
    int totalPagesAdmin = 0;
    int totalPagesPatient = 0;
    int selectedRow = -1;
    int selectedCol = 1;

    void reset()
    {
        currentPage = 0;
        currentFilter = Filter::patient;
        searchQuery = "";
        selectedRow = -1;
        selectedCol = 1;
    }

    void generateListMatrixPatient(const std::vector<std::pair<std::string, std::string>> &records)
    {
        listMatrixPatient.clear();
        for (const auto &record : records)
        {
            listMatrixPatient.push_back({record.first, "[View]", "[Update]", "[Delete]", record.second});
        }
        totalPagesPatient = listMatrixPatient.empty() ? 0 : (listMatrixPatient.size() - 1) / pageSize + 1;
    }

    void generateListMatrixAdmin(const std::vector<std::pair<std::string, std ::string>> &records)
    {
        listMatrixAdmin.clear();
        for (const auto &record : records)
        {
            listMatrixAdmin.push_back({record.first, "[View]", "[Update]", "[Delete]", record.second});
        }
        totalPagesAdmin = listMatrixAdmin.empty() ? 0 : (listMatrixAdmin.size() - 1) / pageSize + 1;
    }

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
    // Singleton Implementation
    static Database &getInstance()
    {
        static Database instance;
        return instance;
    }

    // Delete copy constructor and assignment operator to prevent accidental copies
    Database(const Database &) = delete;
    Database &operator=(const Database &) = delete;

private:
    Database() {} // Private constructor to prevent multiple instances
};

struct UpdatePatient
{
    std::shared_ptr<User> user;
    std::unordered_map<std::string, std::pair<std::string, std::string>> fieldValues;

    void reset()
    {
        fieldValues.clear();
    }

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

    void updateFieldValue(const std::string &fieldName, const std::string &newValue)
    {
        if (fieldValues.find(fieldName) != fieldValues.end())
        {
            fieldValues[fieldName].second = newValue;
        }
    }

    void handleUpdatePatient(const std::string &userId)
    {
        UserManager &userManager = UserManager::getInstance();
        auto patient = std::dynamic_pointer_cast<Patient>(userManager.getUserById(userId));

        for (const auto &entry : fieldValues)
        {
            const std::string &fieldName = entry.first;
            const std::string &prev = entry.second.first;
            const std::string &curr = entry.second.second;

            if (prev != curr)
            {
                if (fieldName == "identityCardNumber")
                {
                    int newAge = calculateAge(curr);
                    userManager.updateUser(userId, "age", std::to_string(newAge));
                }
                if (fieldName == "height" || fieldName == "weight")
                {
                    double newBMI = calculateBMI(fieldValues["weight"].second, fieldValues["height"].second);
                    userManager.updateUser(userId, "bmi", std::to_string(newBMI));
                }
                userManager.updateUser(userId, fieldName, curr);
            }
        }
    }

    // Singleton Implementation
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

struct UpdateAdmin
{
    std::shared_ptr<User> user;
    std::unordered_map<std::string, std::pair<std::string, std::string>> fieldValues;

    void reset()
    {
        fieldValues.clear();
    }

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

    void updateFieldValue(const std::string &fieldName, const std::string &newValue)
    {
        if (fieldValues.find(fieldName) != fieldValues.end())
        {
            fieldValues[fieldName].second = newValue;
        }
    }

    void handleUpdateAdmin(const std::string &userId)
    {
        UserManager &userManager = UserManager::getInstance();

        for (const auto &entry : fieldValues)
        {
            const std::string &fieldName = entry.first;
            const std::string &prev = entry.second.first;
            const std::string &curr = entry.second.second;

            if (prev != curr)
            {
                userManager.updateUser(userId, fieldName, curr);
            }
        }
    }

    // Singleton Implementation
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

void renderLoginScreen();
void renderDashboardScreen();
void handleDashboardOptions();
void renderHeader();
void clearScreen();
void initializeColors();
void clearScrollbackBuffer();
void renderControlInfo();
void renderRegistrationScreenAdmin();
void renderRegistrationAccountPatientScreen();
void renderRegistrationPersonalPatientScreen();
void renderRegistrationSelectionPatientScreen();
void navigationHandler(FORM *form, FIELD **fields, WINDOW *win_form, WINDOW *win_body, Screen screen);
void exitHandler();
void renderHorizontalMenuStack(WINDOW *win, const std::vector<std::string> &items, const std::string &title, int y_offset, int &selected_index, int start_x);
bool validateFields(FIELD **fields);
bool submitRegistrationPatient();
void renderDatabaseScreen();
void handleDatabaseControls(WINDOW *win_form, WINDOW *win_body);
void renderProfileScreen();
void renderProfileAdmissionsScreen();
void renderAdmissionScreen();
void renderUpdateAccountPatientScreen();
void renderUpdatePersonalPatientScreen();
void renderUpdateSelectionPatientScreen();
void renderUpdateAdminScreen();

#endif