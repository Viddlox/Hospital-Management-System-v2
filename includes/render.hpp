
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

void renderHeader();
void renderTime(std::time_t time);
void clearScreen();
void initializeColors();
void clearScrollbackBuffer();
char* trim_whitespaces(char *str);
void driver(int ch, FORM *form, WINDOW *win_form, FIELD **fields);
void renderLoginScreen();
void renderDashboardScreen();

#endif