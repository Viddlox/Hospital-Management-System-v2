#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include <iostream>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <ctime>
#include <atomic>
#include <csignal>
#include "render.hpp"
#include "UserManager.hpp"

class EventManager
{
private:
    std::mutex mtx;
    std::atomic<bool> isRunning{false};
    std::condition_variable cv;
    std::queue<std::string> notifications;
    std::thread notificationThread;
    std::thread timeThread;
    Screen screen;
    std::atomic<std::time_t> time;
    UserManager &userManager = UserManager::getInstance();

    EventManager() : screen(Screen::Login), time(getCurrentTime()) {}
    ~EventManager()
    {
        stopThreads();
    }
    EventManager(const EventManager &) = delete;
    EventManager &operator=(const EventManager &) = delete;

    // Notification handling logic
    void handleNotifications()
    {
        while (isRunning.load())
        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [this]
                    { return !notifications.empty() || !isRunning.load(); });

            while (!notifications.empty())
            {
                std::cout << "[Notification] " << notifications.front() << std::endl;
                notifications.pop();
                std::this_thread::sleep_for(std::chrono::seconds(2));
            }
        }
    }

    // time handling logic
    void handleTime()
    {
        while (isRunning.load())
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            time.store(getCurrentTime(), std::memory_order_relaxed); // Using memory_order_relaxed for simple updates
        }
    }

    // Render the UI layout
    void renderLayout()
    {
        refresh();
        switch (screen)
        {
        case Screen::Login:
            renderHeader();
            renderControlInfo();
            renderLoginScreen();
            break;
        case Screen::Dashboard:
            renderHeader();
            renderControlInfo();
            renderDashboardScreen();
            break;
        case Screen::Register:
            renderControlInfo();
            renderRegistrationScreen();
            break;
        case Screen::Roster:
            break;
        case Screen::Userbase:
            break;
        case Screen::Appointment:
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

    // Stop the notification thread safely
    void stopThreads()
    {
        if (notificationThread.joinable())
        {
            notificationThread.join();
        }
        if (timeThread.joinable())
        {
            timeThread.join();
        }
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
        isRunning.store(true);
        notificationThread = std::thread(&EventManager::handleNotifications, this);
        timeThread = std::thread(&EventManager::handleTime, this);
        if (userManager.getAdminCount() < 1)
            userManager.createAdmin("admin", "1234");
        initScreen();
        try
        {
            while (isRunning.load())
            {
                renderLayout();
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "Exception: " << e.what() << std::endl;
            isRunning.store(false);
        }
        exit();
    }

    // Stop the event manager safely
    void exit()
    {
        {
            std::lock_guard<std::mutex> lock(mtx);
            isRunning.store(false);
        }
        cv.notify_all();
        stopThreads();
        clear();
        refresh();
        endwin();
        std::exit(0);
    }
};

#endif
