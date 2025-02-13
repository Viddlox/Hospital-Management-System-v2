
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

#include <csignal>
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif

class EventManager;
class UserManager;

enum class Screen
{
    Login,
    Register,
    Dashboard,
    Profile,
    Database,
    Appointments
};

struct Color
{
    int primary = 1;
    int secondary = 2;
    int danger = 3;
};

struct Registration
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

    enum class Section
    {
        account,
        personal,
        selection
    };

    Section currentSection = Section::account;

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
    Registration()
    {
        selectedIndices = std::vector<int>(menuArrs.size(), 0);
    }
    void reset()
    {
        currentSection = Section::account;
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
};

struct Dashboard
{
    std::vector<std::string> patientOptionsArr = {
        "Appointments",
        "Profile",
        "Log Out"};
    std::vector<std::string> adminOptionsArr = {
        "Database",
        "Profile",
        "Log Out"};
    int selectedIndex = 0;
    void reset()
    {
        selectedIndex = 0;
    }
};

struct Database
{
    std::vector<std::string> controlInfoArr = {
        "1) PgDn (next page)",
        "2) PgUp (prev page)",
        "3) Tab (switch role)",
        "4) + (create record)"};
    std::vector<std::string> recordOptionsArr = {
        "View record",
        "Update record",
        "Delete record"};
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

    int currentPage = 0;
    int pageSize = 10; // Number of records per page
    int totalPagesAdmin = 0;
    int totalPagesPatient = 0;

    void reset()
    {
        currentPage = 0;
        currentFilter = Filter::patient;
        searchQuery = "";
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
};

void renderHeader();
void clearScreen();
void initializeColors();
void clearScrollbackBuffer();
char *trim_whitespaces(char *str);
void renderLoginScreen();
void renderDashboardScreen(Dashboard &dash);
void renderRegistrationScreen(Registration &reg);
void renderControlInfo();
void renderRegistrationAccountSection(Registration &reg, Color &colorScheme);
void renderRegistrationPersonalSection(Registration &reg, Color &colorScheme);
void renderRegistrationSelectionSection(Registration &reg, Color &colorScheme);
void backHandlerRegistration(FORM *form, FIELD **fields, WINDOW *win_form, WINDOW *win_body, Registration &reg, Color &colorScheme);
void exitHandler();
void renderHorizontalMenuStack(WINDOW *win, const std::vector<std::string> &items, const std::string &title, int y_offset, int &selected_index, int start_x);
bool validateFields(FIELD **fields, Color &colorScheme);
int calculateAge(const std::string &identityCardNumber);
double calculateBMI(const std::string &weight, const std::string &height);
bool submitRegistration(Registration &reg, Color &colorScheme);
void handleDashboardOptions(Dashboard &dash, std::string &roleStr);
void renderDatabaseScreen(Database &db);

#endif