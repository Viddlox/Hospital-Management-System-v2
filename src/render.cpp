#include "render.hpp"
#include "EventManager.hpp"
#include "UserManager.hpp"

struct Color
{
    int primary = 1;
    int secondary = 2;
    int danger = 3;
};

void initializeColors()
{
    start_color();

    // Define custom colors
    init_color(COLOR_GREEN, 0, 1000, 0);
    init_color(COLOR_BLACK, 0, 0, 0);
    init_color(COLOR_RED, 1000, 0, 0);

    // Define color pairs
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);
}

void renderHeader()
{
    Color colorScheme;
    attron(COLOR_PAIR(colorScheme.primary));
    bkgd(COLOR_PAIR(colorScheme.primary));

    int row = 0;
    int col = (COLS - 81) / 2;

    WINDOW *header_win = newwin(10, 82, row, col);
    box(header_win, 0, 0);

    mvwprintw(header_win, 1, 1, "  /$$      /$$                 /$$ /$$$$$$$$           /$$                      ");
    mvwprintw(header_win, 2, 1, " | $$$    /$$$                | $$|__  $$__/          | $$               /$$    ");
    mvwprintw(header_win, 3, 1, " | $$$$  /$$$$  /$$$$$$   /$$$$$$$   | $$     /$$$$$$ | $$   /$$        | $$    ");
    mvwprintw(header_win, 4, 1, " | $$ $$/$$ $$ /$$__  $$ /$$__  $$   | $$    /$$__  $$| $$  /$$/      /$$$$$$$$ ");
    mvwprintw(header_win, 5, 1, " | $$  $$$| $$| $$$$$$$$| $$  | $$   | $$   | $$$$$$$$| $$$$$$/      |__  $$__/ ");
    mvwprintw(header_win, 6, 1, " | $$\\  $ | $$| $$_____/| $$  | $$   | $$   | $$_____/| $$_  $$         | $$   ");
    mvwprintw(header_win, 7, 1, " | $$ \\/  | $$|  $$$$$$$|  $$$$$$$   | $$   |  $$$$$$$| $$ \\  $$        |__/  ");
    mvwprintw(header_win, 8, 1, " |__/     |__/ \\_______/ \\_______/   |__/    \\_______/|__/  \\__/            ");

    refresh();
    wbkgd(header_win, COLOR_PAIR(colorScheme.primary));
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

std::vector<std::string> split_string(std::string toSplit, int maxLength)
{
    std::vector<std::string> result;
    std::string line;
    std::istringstream stream(toSplit);
    std::string word;

    while (stream >> word)
    {
        if (line.length() + word.length() + 1 <= static_cast<size_t>(maxLength))
        {
            if (line.empty())
                line = word;
            else
                line += " " + word;
        }
        else
        {
            result.push_back(line);
            line = word;
        }
    }
    if (!line.empty())
    {
        result.push_back(line);
    }
    return result;
}

void printCentered(WINDOW *win, int startRow, const std::string &text)
{
    int centerCol = getmaxx(win) / 2;
    std::vector<std::string> lines = split_string(text, getmaxx(win));

    for (size_t i = 0; i < lines.size(); i++)
    {
        int halfLength = lines[i].length() / 2;
        int adjustedCol = centerCol - halfLength;
        mvwprintw(win, startRow + i, adjustedCol, "%s", lines[i].c_str());
    }
}

void renderLoginScreen()
{
    EventManager &eventManager = EventManager::getInstance();
    UserManager &userManager = UserManager::getInstance();
    Color colorScheme;

    bkgd(COLOR_PAIR(colorScheme.primary));

    // Render subheaders
    std::string subHeader1 = "Welcome to MedTek+ TUI";
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

    int footer_start_y = form_start_y + 12;
    int footer_start_x = (COLS - 33) / 2;
    std::string footer = "Don't have an existing account? Press Ctrl+r to register now";
    WINDOW *win_footer = newwin(4, 33, footer_start_y, footer_start_x);
    wbkgd(win_footer, COLOR_PAIR(colorScheme.primary));
    box(win_footer, 0, 0);
    printCentered(win_footer, 1, footer);

    // Refresh the windows
    wrefresh(win_body);
    wrefresh(win_form);
    wrefresh(win_footer);

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
        case 27:
            eventManager.exit();
            break;
        case 18:
            unpost_form(form);
            free_form(form);
            for (int i = 0; fields[i]; ++i)
                free_field(fields[i]);
            delwin(win_form);
            delwin(win_body);
            delwin(win_footer);
            eventManager.switchScreen(Screen::Register);
            return;
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
        delwin(win_footer);
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

void renderControlInfo()
{
    Color colorScheme;

    std::string header = "Controls";
    std::string back = "1) Ctrl+b (back)";
    std::string exit = "2) Esc/Ctrl+c (exit)";

    int outer_width = 24;
    int outer_height = 8;

    int inner_width = 22;
    int inner_height = 4;

    WINDOW *win_outer = newwin(outer_height, outer_width, 1, 1);
    wbkgd(win_outer, COLOR_PAIR(colorScheme.primary));
    box(win_outer, 0, 0);
    mvwprintw(win_outer, 1, (outer_width - header.length()) / 2, "%s", header.c_str());
    wrefresh(win_outer);

    WINDOW *win_inner = derwin(win_outer, inner_height, inner_width, 2, 1);
    wbkgd(win_inner, COLOR_PAIR(colorScheme.primary));
    box(win_inner, 0, 0);
    mvwprintw(win_inner, 1, 1, "%s", back.c_str());
    mvwprintw(win_inner, 2, 1, "%s", exit.c_str());
    wrefresh(win_inner);

    delwin(win_inner);
    delwin(win_outer);
}

void renderRegistrationScreen()
{
    // Set up the screen and cursor
    Color colorScheme;
    bkgd(COLOR_PAIR(colorScheme.primary));

    // Header text
    std::string header = "Register a MedTek+ User Account";

    // Outer window dimensions
    int outer_height = 16; // Height of the main window
    int outer_width = 80;  // Width of the main window

    // Calculate the center position for the outer window
    int start_y = (LINES - outer_height) / 2; // Center vertically
    int start_x = (COLS - outer_width) / 2;   // Center horizontally

    // Create the outer window
    WINDOW *win_outer = newwin(outer_height, outer_width, start_y, start_x);
    wbkgd(win_outer, COLOR_PAIR(colorScheme.primary));
    box(win_outer, 0, 0);
    mvwprintw(win_outer, 1, (outer_width - header.length()) / 2, "%s", header.c_str());
    wrefresh(win_outer);

    // Inner window dimensions (for form placement)
    int inner_height = outer_height - 4; // Slightly smaller than the outer window
    int inner_width = outer_width - 4;   // Slightly smaller than the outer window

    // Create the inner window centered inside the outer window
    WINDOW *win_inner = derwin(win_outer, inner_height, inner_width, 2, 2);
    wbkgd(win_inner, COLOR_PAIR(colorScheme.primary));
    box(win_inner, 0, 0);
    wrefresh(win_inner);

    // Cleanup: Delete windows
    delwin(win_inner);
    delwin(win_outer);
}

void renderDashboardScreen()
{
    Color colorScheme;
    bkgd(COLOR_PAIR(colorScheme.primary));

    std::string subHeader = "Dashboard";

    int baseline = 11;
    mvprintw(baseline, (COLS - subHeader.length()) / 2, "%s", subHeader.c_str());
}