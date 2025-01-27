
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
    Userbase,
    Appointment,
};

struct Color
{
    int primary = 1;
    int secondary = 2;
    int danger = 3;
};

struct Registration
{
    enum class Gender
    {
        Male,
        Female,
    };
    enum class Religion
    {
        Islam,
        Buddhism,
        Christianity,
        Hinduism,
        Sikkhism,
        Other
    };
    enum class Race
    {
        Malay,
        Chinese,
        Indian,
        Other
    };
    enum class MaritalStatus
    {
        Single,
        Married,
    };
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
    std::string gender;
    std::string race;
    std::string religion;
    std::string maritalStatus;
};

void renderHeader();
void renderTime(std::time_t time);
void clearScreen();
void initializeColors();
void clearScrollbackBuffer();
char *trim_whitespaces(char *str);
void renderLoginScreen();
void renderDashboardScreen();
void renderRegistrationScreen(Registration &reg);
void renderControlInfo();
void renderRegistrationAccountSection(Registration &reg, Color &colorScheme);
void renderRegistrationPersonalSection(Registration &reg, Color &colorScheme);
void backHandlerRegistration(FORM *form, FIELD **fields, WINDOW *win_form, WINDOW *win_body, Registration &reg, Color &colorScheme);
void exitHandler();

#endif