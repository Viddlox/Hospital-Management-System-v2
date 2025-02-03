
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
#include <unordered_map>
#include <cctype>
#include <math.h>

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
    Roster,
    Profile,
    Userbase,
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
        "Roster",
        "Profile",
        "Log Out"};
    std::vector<std::string> adminOptionsArr = {
        "Roster",
        "Userbase",
        "Profile",
        "Log Out"};
    int selectedIndex = 0;
    void reset()
    {
        selectedIndex = 0;
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
void renderHorizontalMenuStack(WINDOW *win, const std::vector<std::string> &items, const std::string &title, int y_offset, int &selected_index);
bool validateFields(FIELD **fields, Color &colorScheme);
int calculateAge(const std::string &identityCardNumber);
double calculateBMI(const std::string &weight, const std::string &height);
bool submitRegistration(Registration &reg, Color &colorScheme);

#endif