#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include <iostream> // Provides basic input/output functionality (std::cout, std::cerr)
#include <ctime>    // Used to get the current system time (std::time_t)
#include <atomic>   // Ensures thread-safe access to `isRunning`
#include <csignal>  // Allows handling of system signals (e.g., SIGINT for safe termination)

#include "render.hpp"      // Handles UI rendering functions for different screens
#include "UserManager.hpp" // Manages user authentication, role-based access, and user records

// The EventManager class is responsible for managing the UI screens, handling user interactions,
// and managing the event loop for the application.
class EventManager
{
private:
    Screen screen;                                         // Tracks the current screen state
    UserManager &userManager = UserManager::getInstance(); // Singleton reference to user management system
    bool isRunning = false;                                // Flag to control the event loop

    // Private constructor to enforce singleton pattern
    EventManager() : screen(Screen::Login) {}
    ~EventManager() {}

    // Delete copy constructor and assignment operator to prevent copying
    EventManager(const EventManager &) = delete;
    EventManager &operator=(const EventManager &) = delete;

    // Function to render the appropriate UI layout based on the current screen
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
            renderDashboardScreen();
            wrefresh(stdscr);
            break;
        case Screen::RegistrationAccountPatientScreen:
            curs_set(1);
            renderRegistrationAccountPatientScreen();
            wrefresh(stdscr);
            break;
        case Screen::RegistrationPersonalPatientScreen:
            curs_set(1);
            renderRegistrationPersonalPatientScreen();
            wrefresh(stdscr);
            break;
        case Screen::RegistrationSelectionPatientScreen:
            renderRegistrationSelectionPatientScreen();
            wrefresh(stdscr);
            break;
        case Screen::RegisterAdmin:
            curs_set(1);
            renderRegistrationScreenAdmin();
            wrefresh(stdscr);
            break;
        case Screen::Database:
            renderDatabaseScreen();
            wrefresh(stdscr);
            break;
        case Screen::Profile:
            curs_set(0);
            renderProfileScreen();
            wrefresh(stdscr);
            break;
        case Screen::ProfileAdmissions:
            renderProfileAdmissionsScreen();
            wrefresh(stdscr);
            break;
        case Screen::Admission:
            renderAdmissionScreen();
            wrefresh(stdscr);
            break;
        case Screen::UpdateAccountPatientScreen:
            curs_set(1);
            renderUpdateAccountPatientScreen();
            wrefresh(stdscr);
            break;
        case Screen::UpdatePersonalPatientScreen:
            curs_set(1);
            renderUpdatePersonalPatientScreen();
            wrefresh(stdscr);
            break;
        case Screen::UpdateSelectionPatientScreen:
            renderUpdateSelectionPatientScreen();
            wrefresh(stdscr);
            break;
        case Screen::UpdateAdminScreen:
            curs_set(1);
            renderUpdateAdminScreen();
            wrefresh(stdscr);
            break;
        default:
            break;
        }
    }

    // Initialize terminal settings and color configurations
    void initScreen()
    {
        initscr();               // Initialize the ncurses screen
        cbreak();                // Disable line buffering
        noecho();                // Disable automatic echo of input
        keypad(stdscr, TRUE);    // Enable special keys (e.g., arrow keys)
        scrollok(stdscr, FALSE); // Disable scrolling
        initializeColors();      // Set up terminal colors
    }

public:
    // Singleton instance getter
    static EventManager &getInstance()
    {
        static EventManager instance;
        return instance;
    }

    // Switch the active screen and trigger a re-render
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

    // Start the event loop, initialize UI, and handle user sessions
    void start()
    {
        clearScreen();
        isRunning = true;
        initScreen();

        // Ensure at least one admin and patient exist in the system for testing/demo purposes
        if (userManager.getAdminCount() < 1)
        {
            userManager.createAdmin("admin" + std::to_string(1), "1234",
                                    "Michael Cheng" + std::to_string(1), "lol@gmail.com", "0123917125");
        }
        // Main event loop to render UI updates
        try
        {
            while (isRunning)
            {
                renderLayout();
                std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Reduce CPU usage
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "Exception: " << e.what() << std::endl;
            isRunning = false;
        }
        exit();
    }

    // Safely stop the event loop and clean up terminal state
    void exit()
    {
        isRunning = false;
        clear();
        refresh();
        endwin(); // Restore terminal settings
        std::exit(0);
    }
};

#endif // EVENT_MANAGER_H
