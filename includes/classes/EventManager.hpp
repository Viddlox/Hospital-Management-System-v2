#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include <iostream>
#include <ctime>
#include <atomic>
#include <csignal>
#include "render.hpp"
#include "UserManager.hpp"

class EventManager
{
private:
    Screen screen;
    UserManager &userManager = UserManager::getInstance();
    RegistrationPatient reg;
    RegistrationAdmin regA;
    Dashboard dash;
    Database db;
    Profile p;
    Admission a;
    bool isRunning = false;

    EventManager() : screen(Screen::Login) {}
    ~EventManager()
    {
    }
    EventManager(const EventManager &) = delete;
    EventManager &operator=(const EventManager &) = delete;

    // Render the UI layout
    void renderLayout()
    {
        switch (screen)
        {
        case Screen::Login:
            curs_set(1);
            renderHeader();
            renderControlInfo();
            renderLoginScreen();
            wrefresh(stdscr);
            break;
        case Screen::Dashboard:
            curs_set(0);
            renderHeader();
            renderControlInfo();
            renderDashboardScreen(dash, p);
            wrefresh(stdscr);
            break;
        case Screen::RegisterPatient:
            curs_set(1);
            renderRegistrationScreenPatient(reg);
            wrefresh(stdscr);
            break;
        case Screen::RegisterAdmin:
            curs_set(1);
            renderRegistrationScreenAdmin(regA);
            wrefresh(stdscr);
            break;
        case Screen::Database:
            renderDatabaseScreen(db, p);
            wrefresh(stdscr);
            break;
        case Screen::Profile:
            curs_set(0);
            renderProfileScreen(p);
            wrefresh(stdscr);
            break;
        case Screen::ProfileAdmissions:
            renderProfileAdmissionsScreen(p);
            wrefresh(stdscr);
            break;
        case Screen::Admission:
            renderAdmissionScreen(a);
            wrefresh(stdscr);
            break;
        default:
            break;
        }
    }

    // Initialize terminal settings and colors
    void initScreen()
    {
        initscr();
        cbreak();
        noecho();
        keypad(stdscr, TRUE);
        scrollok(stdscr, FALSE);
        initializeColors();
    }

public:
    // get singleton instance
    static EventManager &getInstance()
    {
        static EventManager instance;
        return instance;
    }

    // Switch the current screen
    void switchScreen(Screen newScreen)
    {
        clear();
        screen = newScreen;
        renderLayout();
    }

    // Get the current system time
    std::time_t getCurrentTime()
    {
        return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    }

    // Start the event manager
    void start()
    {
        clearScreen();
        isRunning = true;
        initScreen();

        if (userManager.getAdminCount() < 1)
        {
            for (int i = 1; i < 31; i++)
                userManager.createAdmin("admin" + std::to_string(i), "1234",
                                        "Michael Cheng" + std::to_string(i), "nigga@gmail.com", "0123917125");
        }
        if (userManager.getPatientCount() < 1)
        {
            for (int i = 1; i < 31; i++)
                userManager.createPatient("user" + std::to_string(i), "123", 42,
                                          "Peter Griffin" + std::to_string(i), "Christianity", "Malaysian", "010403141107",
                                          "single", "male", "Chinese", "mikeypeter37@gmail.com", "0123917125", "0123197125",
                                          "Lois Griffin", "Quahog", 20, "180", "70", Admissions::Department::Surgery);
        }
        try
        {
            while (isRunning)
            {
                renderLayout();
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "Exception: " << e.what() << std::endl;
            isRunning = false;
        }
        exit();
    }

    // Stop the event manager safely
    void exit()
    {
        isRunning = false;
        clear();
        refresh();
        endwin();
        std::exit(0);
    }
};

#endif
