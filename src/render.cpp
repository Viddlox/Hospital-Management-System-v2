#include "render.hpp"
#include "EventManager.hpp"
#include "UserManager.hpp"

void initializeColors()
{
    start_color();

    // Define custom colors
    init_color(COLOR_GREEN, 0, 1000, 0);
    init_color(COLOR_BLACK, 0, 0, 0);
    init_color(COLOR_RED, 1000, 0, 0);

    // Define color pairs
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_GREEN);
    init_pair(3, COLOR_RED, COLOR_BLACK);
}

void renderHeader()
{
    Color colorScheme;
    attron(COLOR_PAIR(colorScheme.primary)); // Set the color for the text and box
    bkgd(COLOR_PAIR(colorScheme.primary));

    int row = 0;
    int col = (COLS - 81) / 2;

    // Create a window to contain the header and the box around it
    WINDOW *header_win = newwin(10, 82, row, col); // Set appropriate window size
    box(header_win, 0, 0);                         // Draw a box around the window

    mvwprintw(header_win, 1, 1, "  /$$      /$$                 /$$ /$$$$$$$$           /$$                      ");
    mvwprintw(header_win, 2, 1, " | $$$    /$$$                | $$|__  $$__/          | $$               /$$    ");
    mvwprintw(header_win, 3, 1, " | $$$$  /$$$$  /$$$$$$   /$$$$$$$   | $$     /$$$$$$ | $$   /$$        | $$    ");
    mvwprintw(header_win, 4, 1, " | $$ $$/$$ $$ /$$__  $$ /$$__  $$   | $$    /$$__  $$| $$  /$$/      /$$$$$$$$ ");
    mvwprintw(header_win, 5, 1, " | $$  $$$| $$| $$$$$$$$| $$  | $$   | $$   | $$$$$$$$| $$$$$$/      |__  $$__/ ");
    mvwprintw(header_win, 6, 1, " | $$\\  $ | $$| $$_____/| $$  | $$   | $$   | $$_____/| $$_  $$         | $$   ");
    mvwprintw(header_win, 7, 1, " | $$ \\/  | $$|  $$$$$$$|  $$$$$$$   | $$   |  $$$$$$$| $$ \\  $$        |__/  ");
    mvwprintw(header_win, 8, 1, " |__/     |__/ \\_______/ \\_______/   |__/    \\_______/|__/  \\__/            ");

    // Make sure the header content is displayed within the window

    wbkgd(header_win, COLOR_PAIR(colorScheme.primary));
    refresh();
    wrefresh(header_win);
    delwin(header_win);
}

void renderTime(std::time_t time)
{
    std::string timeStr = std::ctime(&time);
    timeStr.pop_back();

    Color colorScheme;
    attron(COLOR_PAIR(colorScheme.primary));

    int row = 11;
    int col = (COLS - 81) / 2;

    mvprintw(row, col, "%s", timeStr.c_str());
    refresh();
}

void clearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Experimental

void clearScrollbackBuffer()
{
    std::cout << "\033[3J" << std::flush;
}

char *trim_whitespaces(char *str)
{
    char *end;

    // trim leading space
    while (isspace(*str))
        str++;

    if (*str == 0) // all spaces?
        return str;

    // trim trailing space
    end = str + strnlen(str, 128) - 1;

    while (end > str && isspace(*end))
        end--;

    // write new null terminator
    *(end + 1) = '\0';

    return str;
}

void driver(int ch, FORM *form, WINDOW *win_form, FIELD **fields)
{
    switch (ch)
    {
    case KEY_F(2):
        // Sync the field buffer with what's displayed
        form_driver(form, REQ_NEXT_FIELD);
        form_driver(form, REQ_PREV_FIELD);

        // Debug: Print field values
        move(LINES - 3, 2);
        for (int i = 0; fields[i]; i++)
        {
            printw("%s", trim_whitespaces(field_buffer(fields[i], 0)));
            if (field_opts(fields[i]) & O_ACTIVE)
                printw("\"\t");
            else
                printw(": \"");
        }
        refresh();
        break;

    case KEY_DOWN:
        form_driver(form, REQ_NEXT_FIELD);
        break;

    case KEY_UP:
        form_driver(form, REQ_PREV_FIELD);
        break;

    case KEY_LEFT:
        form_driver(form, REQ_PREV_CHAR);
        break;

    case KEY_RIGHT:
        form_driver(form, REQ_NEXT_CHAR);
        break;

    // Delete the char before cursor
    case KEY_BACKSPACE:
    case 127:
        form_driver(form, REQ_DEL_PREV);
        break;

    // Delete the char under the cursor
    case KEY_DC:
        form_driver(form, REQ_DEL_CHAR);
        break;

    default:
        // Send other characters to the form driver
        form_driver(form, ch);
        break;
    }

    // Ensure cursor and window are updated
    pos_form_cursor(form);
    wrefresh(win_form);
}

void renderLoginScreen()
{
    EventManager &eventManager = EventManager::getInstance();
    UserManager &userManager = UserManager::getInstance();
    Color colorScheme;

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
        eventManager.switchScreen(Screen::Dashboard);
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

void renderDashboardScreen()
{
    Color colorScheme;
    bkgd(COLOR_PAIR(colorScheme.primary));

    std::string subHeader1 = "Dashboard";

    int baseline = 11;
    mvprintw(baseline, (COLS - subHeader1.length()) / 2, "%s", subHeader1.c_str());
}