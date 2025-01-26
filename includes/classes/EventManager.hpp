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

enum class Screen
{
    Login,
    Dashboard,
    Roster,
    Userbase
};

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
    UserManager& userManager = UserManager::getInstance();

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
        Color colorScheme;
        renderHeader();
        if (screen == Screen::Login)
        {
            bkgd(COLOR_PAIR(colorScheme.primary));

            // Render subheaders
            std::string subHeader1 = "Welcome to MedTek+ CLI";
            std::string subHeader2 = "Please enter your login credentials below";

            int baseline = 11;
            mvprintw(baseline, (COLS - subHeader1.length()) / 2, "%s", subHeader1.c_str());
            mvprintw(baseline + 2, (COLS - subHeader2.length()) / 2, "%s", subHeader2.c_str());

            // Create the main window for the form
            int form_start_y = baseline + 4;
            int form_start_x = (COLS - 50) / 2;
            WINDOW *win_body = newwin(12, 50, form_start_y, form_start_x);

            wbkgd(win_body, COLOR_PAIR(colorScheme.primary));
            box(win_body, 0, 0);
            mvwprintw(win_body, 1, 2, "Login Form");
            refresh();
            wrefresh(win_body);

            // Create a subwindow inside the main window for the form
            WINDOW *win_form = derwin(win_body, 8, 48, 3, 1);
            wbkgd(win_form, COLOR_PAIR(colorScheme.primary));
            box(win_form, 0, 0);
            wrefresh(win_form);

            // Create form fields
            FIELD *fields[5];
            fields[0] = new_field(1, 10, 0, 1, 0, 0);
            fields[1] = new_field(1, 30, 0, 13, 0, 0);
            fields[2] = new_field(1, 10, 2, 1, 0, 0);
            fields[3] = new_field(1, 30, 2, 13, 0, 0);
            fields[4] = NULL;

            assert(fields[0] && fields[1] && fields[2] && fields[3]);

            // Set field buffers
            set_field_buffer(fields[0], 0, "Username:");
            set_field_buffer(fields[1], 0, "");
            set_field_buffer(fields[2], 0, "Password:");
            set_field_buffer(fields[3], 0, "");

            // Set field options
            set_field_opts(fields[0], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
            set_field_opts(fields[1], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
            set_field_opts(fields[2], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
            set_field_opts(fields[3], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);

            // Set field background color
            set_field_back(fields[0], COLOR_PAIR(colorScheme.primary));
            set_field_back(fields[1], COLOR_PAIR(colorScheme.primary));
            set_field_back(fields[2], COLOR_PAIR(colorScheme.primary));
            set_field_back(fields[3], COLOR_PAIR(colorScheme.primary));

            // Create and post form
            FORM *form = new_form(fields);
            assert(form);
            set_form_win(form, win_form);                      // Attach form to the subwindow
            set_form_sub(form, derwin(win_form, 6, 46, 1, 1)); // Form subwindow inside win_form
            post_form(form);

            // Refresh the windows
            wrefresh(win_body);
            wrefresh(win_form);

            // Main input loop
            int ch;
            while ((ch = getch()) != '\n')
            {
                switch (ch)
                {
                case KEY_DOWN:
                    form_driver(form, REQ_NEXT_FIELD);
                    form_driver(form, REQ_END_LINE);
                    break;
                case KEY_UP:
                    form_driver(form, REQ_PREV_FIELD);
                    form_driver(form, REQ_END_LINE);
                    break;
                case KEY_LEFT:
                    form_driver(form, REQ_PREV_CHAR);
                    break;
                case KEY_RIGHT:
                    form_driver(form, REQ_NEXT_CHAR);
                    break;
                case KEY_BACKSPACE:
                case 127:
                    form_driver(form, REQ_DEL_PREV);
                    break;
                case KEY_DC:
                    form_driver(form, REQ_DEL_CHAR);
                    break;
                default:
                    form_driver(form, ch);
                    break;
                }
                wrefresh(win_form);
            }

            form_driver(form, REQ_VALIDATION);

            // Extract form data
            std::string username = trim_whitespaces(field_buffer(fields[1], 0));
            std::string password = trim_whitespaces(field_buffer(fields[3], 0));

            // Validate user credentials
            if (userManager.validateUser(username, password))
            {
                unpost_form(form);
                free_form(form);
                for (int i = 0; fields[i]; ++i)
                    free_field(fields[i]);
                delwin(win_form);
                delwin(win_body);
                switchScreen(Screen::Dashboard);
            }
            else
            {
                attron(COLOR_PAIR(colorScheme.danger));
                mvprintw(LINES - 2, (COLS - 30) / 2, "Invalid credentials, try again.");
                refresh();
                std::this_thread::sleep_for(std::chrono::seconds(2));
                clear();
            }
        }

        else if (screen == Screen::Dashboard)
        {
            bkgd(COLOR_PAIR(colorScheme.primary));

            // Render subheaders
            std::string subHeader1 = "Dashboard";

            int baseline = 11; // Baseline after the header
            mvprintw(baseline, (COLS - subHeader1.length()) / 2, "%s", subHeader1.c_str());
        }
        refresh();
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
    }
};

#endif
