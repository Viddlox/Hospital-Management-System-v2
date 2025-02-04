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
    init_color(4, 1000, 500, 0); // Custom amber color

    // Define color pairs
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_BLACK, 4);
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
    userManager.setCurrentUser(nullptr);

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
    std::string loginForm = "Login Form";
    mvwprintw(win_body, 1, (50 - loginForm.length()) / 2, "%s", loginForm.c_str());

    // Create a subwindow inside the main window for the form
    WINDOW *win_form = derwin(win_body, 8, 48, 3, 1);
    wbkgd(win_form, COLOR_PAIR(colorScheme.primary));
    box(win_form, 0, 0);

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
        mvprintw(LINES - 2, (COLS - 40) / 2, "Invalid credentials, please try again");
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
    std::string enter = "3) Enter (proceed)";

    int outer_width = 24;
    int outer_height = 10;

    int inner_width = 22;
    int inner_height = 6;

    WINDOW *win_outer = newwin(outer_height, outer_width, 0, 1);
    wbkgd(win_outer, COLOR_PAIR(colorScheme.primary));
    box(win_outer, 0, 0);
    mvwprintw(win_outer, 1, (outer_width - header.length()) / 2, "%s", header.c_str());
    wrefresh(win_outer);

    WINDOW *win_inner = derwin(win_outer, inner_height, inner_width, 2, 1);
    wbkgd(win_inner, COLOR_PAIR(colorScheme.primary));
    box(win_inner, 0, 0);
    mvwprintw(win_inner, 1, 1, "%s", back.c_str());
    mvwprintw(win_inner, 2, 1, "%s", exit.c_str());
    mvwprintw(win_inner, 3, 1, "%s", enter.c_str());
    wrefresh(win_inner);

    delwin(win_inner);
    delwin(win_outer);
}

void exitHandler()
{
    EventManager &eventManager = EventManager::getInstance();
    eventManager.exit();
}
void backHandlerRegistration(FORM *form, FIELD **fields, WINDOW *win_form, WINDOW *win_body, Registration &reg, Color &colorScheme)
{
    EventManager &eventManager = EventManager::getInstance();
    Registration::Section section = reg.currentSection;

    if (form)
    {
        unpost_form(form);
        free_form(form);
    }
    if (fields)
    {
        for (int i = 0; fields[i]; i++)
        {
            free_field(fields[i]);
        }
    }
    wclear(win_form);
    wclear(win_body);
    delwin(win_form);
    delwin(win_body);
    clear();
    refresh();

    switch (section)
    {
    case Registration::Section::selection:
        renderRegistrationPersonalSection(reg, colorScheme);
        break;
    case Registration::Section::personal:
        renderRegistrationAccountSection(reg, colorScheme);
        break;
    case Registration::Section::account:
        reg.reset();
        eventManager.switchScreen(Screen::Login);
        break;
    default:
        break;
    }
}

void driver_form(int ch, FORM *form, FIELD **fields, WINDOW *win_form, WINDOW *win_body,
                 std::function<void()> exitHandler, std::function<void()> backHandler)
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
    case 27: // Escape key
        if (exitHandler)
        {
            exitHandler();
        }
        break;
    case 2: // Custom key for "Back"
        if (backHandler)
        {
            backHandler();
        }
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

bool validateFields(FIELD **fields, Color &colorScheme)
{
    for (int i = 0; fields[i]; i++)
    {
        if (i % 2 == 0)
        {
            continue;
        }
        std::string field_value = trim_whitespaces(field_buffer(fields[i], 0));
        if (field_value.empty())
        {
            return false;
        }
    }
    return true;
}

void renderRegistrationAccountSection(Registration &reg, Color &colorScheme)
{
    renderHeader();
    renderControlInfo();

    int baseline = 11;
    std::string header = "Register a MedTek+ Account";
    mvprintw(baseline, (COLS - header.length()) / 2, "%s", header.c_str());

    reg.currentSection = Registration::Section::account;
    bkgd(COLOR_PAIR(colorScheme.primary));

    int outer_height = 16;
    int outer_width = 60;

    int start_y = ((LINES - outer_height) / 2) + 2;
    int start_x = (COLS - outer_width) / 2;

    WINDOW *win_body = newwin(outer_height, outer_width, start_y, start_x);
    wbkgd(win_body, COLOR_PAIR(colorScheme.primary));
    box(win_body, 0, 0);

    std::string subHeader = "Account Information";
    mvwprintw(win_body, 1, (outer_width - subHeader.length()) / 2, "%s", subHeader.c_str());

    int inner_height = outer_height - 4;
    int inner_width = outer_width - 4;

    WINDOW *win_form = derwin(win_body, inner_height, inner_width, 2, 2);
    wbkgd(win_form, COLOR_PAIR(colorScheme.primary));
    box(win_form, 0, 0);

    FIELD *fields[11];
    fields[0] = new_field(1, 10, 0, 1, 0, 0);  // Label: Username
    fields[1] = new_field(1, 30, 0, 13, 0, 0); // Input: Username
    fields[2] = new_field(1, 10, 2, 1, 0, 0);  // Label: Password
    fields[3] = new_field(1, 30, 2, 13, 0, 0); // Input: Password
    fields[4] = new_field(1, 10, 4, 1, 0, 0);  // Label: Email
    fields[5] = new_field(1, 30, 4, 13, 0, 0); // Input: Email
    fields[6] = new_field(1, 10, 6, 1, 0, 0);  // Label: Address
    fields[7] = new_field(1, 30, 6, 13, 0, 0); // Input: Address
    fields[8] = new_field(1, 16, 8, 1, 0, 0);  // Label: Contact Number
    fields[9] = new_field(1, 30, 8, 20, 0, 0); // Input: Contact Number
    fields[10] = NULL;

    assert(fields[0] && fields[1] && fields[2] && fields[3] && fields[4] &&
           fields[5] && fields[6] && fields[7] && fields[8] && fields[9]);

    set_field_buffer(fields[0], 0, "Username:");
    set_field_buffer(fields[1], 0, reg.username.c_str());
    set_field_buffer(fields[2], 0, "Password:");
    set_field_buffer(fields[3], 0, reg.password.c_str());
    set_field_buffer(fields[4], 0, "Email:");
    set_field_buffer(fields[5], 0, reg.email.c_str());
    set_field_buffer(fields[6], 0, "Address:");
    set_field_buffer(fields[7], 0, reg.address.c_str());
    set_field_buffer(fields[8], 0, "Contact Number:");
    set_field_buffer(fields[9], 0, reg.contactNumber.c_str());

    // Set field options
    for (int i = 0; fields[i]; i++)
    {
        if (i % 2 == 0)
        {                                                               // Labels
            set_field_opts(fields[i], O_VISIBLE | O_PUBLIC | O_STATIC); // Read-only
        }
        else
        { // Input fields
            set_field_opts(fields[i], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE | O_AUTOSKIP);
        }
        set_field_back(fields[i], COLOR_PAIR(colorScheme.primary)); // Set background color
    }

    // Create and post form
    FORM *form = new_form(fields);
    assert(form);
    set_form_win(form, win_form);
    set_form_sub(form, derwin(win_form, inner_height - 2, inner_width - 2, 1, 1));
    post_form(form);

    // Refresh windows
    wrefresh(win_body);
    wrefresh(win_form);

    // Create a dedicated error window
    int error_win_height = 1;
    int error_win_width = 32;
    int error_win_y = LINES - 2;
    int error_win_x = (COLS - error_win_width) / 2;

    WINDOW *error_win = newwin(error_win_height, error_win_width, error_win_y, error_win_x);
    wbkgd(error_win, COLOR_PAIR(colorScheme.danger));

    // Input loop
    bool done = false;
    int ch;

    while (!done)
    {
        while ((ch = getch()) != '\n')
        {
            driver_form(
                ch,
                form,
                fields,
                win_form,
                win_body,
                [&]()
                { exitHandler(); },
                [&]()
                { backHandlerRegistration(form, fields, win_form, win_body, reg, colorScheme); });
        }
        form_driver(form, REQ_VALIDATION);
        if (!validateFields(fields, colorScheme))
        {
            mvwprintw(error_win, 0, 0, "Please fill all required fields");
            wrefresh(error_win);
            std::this_thread::sleep_for(std::chrono::seconds(2));
            werase(error_win);
            wrefresh(error_win);
        }
        else
        {
            done = true;
        }
    }

    form_driver(form, REQ_VALIDATION);

    // Extract form data
    reg.username = trim_whitespaces(field_buffer(fields[1], 0));
    reg.password = trim_whitespaces(field_buffer(fields[3], 0));
    reg.email = trim_whitespaces(field_buffer(fields[5], 0));
    reg.address = trim_whitespaces(field_buffer(fields[7], 0));
    reg.contactNumber = trim_whitespaces(field_buffer(fields[9], 0));

    // Clean up
    unpost_form(form);
    free_form(form);
    for (int i = 0; fields[i]; i++)
    {
        free_field(fields[i]);
    }
    wclear(win_form);
    wclear(win_body);
    delwin(win_form);
    delwin(win_body);
    delwin(error_win);
    clear();
    refresh();

    renderRegistrationPersonalSection(reg, colorScheme);
}

void renderRegistrationPersonalSection(Registration &reg, Color &colorScheme)
{
    renderHeader();
    renderControlInfo();

    int baseline = 11;
    std::string header = "Register a MedTek+ Account";
    mvprintw(baseline, (COLS - header.length()) / 2, "%s", header.c_str());

    reg.currentSection = Registration::Section::personal;
    bkgd(COLOR_PAIR(colorScheme.primary));

    int outer_height = 20;
    int outer_width = 60;

    int start_y = ((LINES - outer_height) / 2) + 4;
    int start_x = (COLS - outer_width) / 2;

    WINDOW *win_body = newwin(outer_height, outer_width, start_y, start_x);
    wbkgd(win_body, COLOR_PAIR(colorScheme.primary));
    box(win_body, 0, 0);

    std::string subHeader = "Personal Information";
    mvwprintw(win_body, 1, (outer_width - subHeader.length()) / 2, "%s", subHeader.c_str());

    int inner_height = outer_height - 4;
    int inner_width = outer_width - 4;

    WINDOW *win_form = derwin(win_body, inner_height, inner_width, 2, 2);
    wbkgd(win_form, COLOR_PAIR(colorScheme.primary));
    box(win_form, 0, 0);

    FIELD *fields[13];
    fields[0] = new_field(1, 15, 0, 1, 0, 0);    // Label: fullName
    fields[1] = new_field(1, 35, 0, 18, 0, 0);   // Input: fullName
    fields[2] = new_field(1, 15, 2, 1, 0, 0);    // Label: identityCardNumber
    fields[3] = new_field(1, 35, 2, 18, 0, 0);   // Input: identityCardNumber
    fields[4] = new_field(1, 15, 4, 1, 0, 0);    // Label: height
    fields[5] = new_field(1, 35, 4, 18, 0, 0);   // Input: height
    fields[6] = new_field(1, 15, 6, 1, 0, 0);    // Label: weight
    fields[7] = new_field(1, 35, 6, 18, 0, 0);   // Input: weight
    fields[8] = new_field(1, 30, 8, 1, 0, 0);    // Label: emergencyContactNumber
    fields[9] = new_field(1, 22, 8, 28, 0, 0);   // Input: emergencyContactNumber
    fields[10] = new_field(1, 28, 10, 1, 0, 0);  // Label: emergencyContactName
    fields[11] = new_field(1, 22, 10, 28, 0, 0); // Input: emergencyContactName
    fields[12] = NULL;

    assert(fields[0] && fields[1] && fields[2] && fields[3] && fields[4] &&
           fields[5] && fields[6] && fields[7] && fields[8] && fields[9] && fields[10] && fields[11]);

    set_field_buffer(fields[0], 0, "Full Name:");
    set_field_buffer(fields[1], 0, reg.fullName.c_str());
    set_field_buffer(fields[2], 0, "IC number:");
    set_field_buffer(fields[3], 0, reg.identityCardNumber.c_str());
    set_field_buffer(fields[4], 0, "Height (cm):");
    set_field_buffer(fields[5], 0, reg.height.c_str());
    set_field_buffer(fields[6], 0, "Weight (kg):");
    set_field_buffer(fields[7], 0, reg.weight.c_str());
    set_field_buffer(fields[8], 0, "Emergency Contact Number:");
    set_field_buffer(fields[9], 0, reg.emergencyContactNumber.c_str());
    set_field_buffer(fields[10], 0, "Emergency Contact Name:");
    set_field_buffer(fields[11], 0, reg.emergencyContactName.c_str());

    // Set field options
    for (int i = 0; fields[i]; i++)
    {
        if (i % 2 == 0) // Labels
        {
            set_field_opts(fields[i], O_VISIBLE | O_PUBLIC | O_STATIC); // Read-only
        }
        else // Input fields
        {
            set_field_opts(fields[i], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE | O_AUTOSKIP);
        }
        set_field_back(fields[i], COLOR_PAIR(colorScheme.primary)); // Set background color
    }

    // Create and post form
    FORM *form = new_form(fields);
    assert(form);
    set_form_win(form, win_form);
    set_form_sub(form, derwin(win_form, inner_height - 2, inner_width - 2, 1, 1));
    post_form(form);

    // Refresh windows
    wrefresh(win_body);
    wrefresh(win_form);

    // Create a dedicated error window
    int error_win_height = 1;
    int error_win_width = 32;
    int error_win_y = LINES - 2;
    int error_win_x = (COLS - error_win_width) / 2;

    WINDOW *error_win = newwin(error_win_height, error_win_width, error_win_y, error_win_x);
    wbkgd(error_win, COLOR_PAIR(colorScheme.danger));

    // Input loop
    bool done = false;
    int ch;

    while (!done)
    {
        while ((ch = getch()) != '\n')
        {
            driver_form(
                ch,
                form,
                fields,
                win_form,
                win_body,
                [&]()
                { exitHandler(); },
                [&]()
                { backHandlerRegistration(form, fields, win_form, win_body, reg, colorScheme); });
        }
        form_driver(form, REQ_VALIDATION);
        if (!validateFields(fields, colorScheme))
        {
            mvwprintw(error_win, 0, 0, "Please fill all required fields");
            wrefresh(error_win);
            std::this_thread::sleep_for(std::chrono::seconds(2));
            werase(error_win);
            wrefresh(error_win);
        }
        else
        {
            done = true;
        }
    }

    form_driver(form, REQ_VALIDATION);

    // Extract form data
    reg.fullName = trim_whitespaces(field_buffer(fields[1], 0));
    reg.identityCardNumber = trim_whitespaces(field_buffer(fields[3], 0));
    reg.height = trim_whitespaces(field_buffer(fields[5], 0));
    reg.weight = trim_whitespaces(field_buffer(fields[7], 0));
    reg.emergencyContactNumber = trim_whitespaces(field_buffer(fields[9], 0));
    reg.emergencyContactName = trim_whitespaces(field_buffer(fields[11], 0));

    // Clean up
    unpost_form(form);
    free_form(form);
    for (int i = 0; fields[i]; i++)
    {
        free_field(fields[i]);
    }
    wclear(win_form);
    wclear(win_body);
    delwin(win_form);
    delwin(win_body);
    delwin(error_win);
    clear();
    refresh();

    renderRegistrationSelectionSection(reg, colorScheme);
}

// Function to render a custom menu
void renderHorizontalMenuStack(WINDOW *win, const std::vector<std::string> &items, const std::string &title, int y_offset, int &selected_index)
{
    int start_x = 2; // Start rendering menu items from column 2
    int start_y = y_offset;

    // Display the menu title
    mvwprintw(win, start_y - 1, start_x, "%s:", title.c_str());

    // Display menu items
    for (int i = 0; i < static_cast<int>(items.size()); ++i)
    {
        if (i == selected_index)
        {
            wattron(win, A_REVERSE); // Turn off highlighting
        }
        mvwprintw(win, start_y, start_x, "%s", items[i].c_str());
        if (i == selected_index)
        {
            wattroff(win, A_REVERSE); // Turn off highlighting
        }
        start_x += items[i].length() + 2; // Add spacing between items
    }
}

int calculateAge(const std::string &identityCardNumber)
{
    // Extract year, month, and day
    int year = std::stoi(identityCardNumber.substr(0, 2));
    int month = std::stoi(identityCardNumber.substr(2, 2));
    int day = std::stoi(identityCardNumber.substr(4, 2));

    // Get the current date
    time_t now = time(0);
    tm *localTime = localtime(&now);
    int currentYear = 1900 + localTime->tm_year;
    int currentMonth = 1 + localTime->tm_mon;
    int currentDay = localTime->tm_mday;

    // Determine the full year (assuming IC numbers use 1900s and 2000s)
    if (year >= 0 && year <= 24)
    { // Adjust based on reasonable birth years
        year += 2000;
    }
    else
    {
        year += 1900;
    }

    // Calculate age
    int age = currentYear - year;
    if (currentMonth < month || (currentMonth == month && currentDay < day))
    {
        age--; // Adjust if birthday hasn't occurred yet this year
    }

    return age;
}

double calculateBMI(const std::string &weight, const std::string &height)
{
    return std::stod(weight) / pow((std::stod(height) / 100.0), 2);
}

bool submitRegistration(Registration &reg, Color &colorScheme)
{
    try
    {
        UserManager &userManager = UserManager::getInstance();
        userManager.createPatient(reg.username, reg.password, calculateAge(reg.identityCardNumber), reg.fullName,
                                  reg.religion, reg.nationality, reg.identityCardNumber, reg.maritalStatus,
                                  reg.gender, reg.race, reg.email, reg.contactNumber, reg.emergencyContactNumber,
                                  reg.emergencyContactName, reg.address, calculateBMI(reg.weight, reg.height), reg.height, reg.weight);
        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error registering new patient" << e.what() << std::endl;
        return false;
    }
    return false;
}

void renderRegistrationSelectionSection(Registration &reg, Color &colorScheme)
{
    renderHeader();
    renderControlInfo();
    reg.currentSection = Registration::Section::selection;

    bkgd(COLOR_PAIR(colorScheme.primary));

    int baseline = 11;
    std::string header = "Register a MedTek+ Account";
    mvprintw(baseline, (COLS - header.length()) / 2, "%s", header.c_str());
    refresh();

    int outer_height = 20;
    int outer_width = 60;

    int start_y = ((LINES - outer_height) / 2) + 4;
    int start_x = (COLS - outer_width) / 2;

    // Create the main window (win_body)
    WINDOW *win_body = newwin(outer_height, outer_width, start_y, start_x);
    wbkgd(win_body, COLOR_PAIR(colorScheme.primary));
    box(win_body, 0, 0);

    // Draw the subheader on win_body
    std::string subHeader = "Personal Information (Continued)";
    mvwprintw(win_body, 1, (outer_width - subHeader.length()) / 2, "%s", subHeader.c_str());
    wrefresh(win_body); // Refresh win_body to show the subheader

    int inner_height = outer_height - 4;
    int inner_width = outer_width - 4;

    // Create the subwindow (win_form) inside win_body
    WINDOW *win_form = derwin(win_body, inner_height, inner_width, 2, 2);
    wbkgd(win_form, COLOR_PAIR(colorScheme.primary));
    box(win_form, 0, 0);
    wrefresh(win_form); // Refresh win_form to show the box

    // Create a dedicated status window
    int status_win_height = 5;
    int status_win_width = 33;
    int status_win_y = LINES - 6;
    int status_win_x = (COLS - status_win_width) / 2;

    WINDOW *status_win = newwin(status_win_height, status_win_width, status_win_y, status_win_x);
    box(status_win, 0, 0);

    bool done = false;
    keypad(win_form, TRUE);
    curs_set(0);

    while (!done)
    {
        // Clear win_form and redraw its border
        werase(win_form);
        box(win_form, 0, 0);

        // Update registration data with selected values
        reg.gender = reg.menuArrs[0][reg.selectedIndices[0]];
        reg.religion = reg.menuArrs[1][reg.selectedIndices[1]];
        reg.race = reg.menuArrs[2][reg.selectedIndices[2]];
        reg.maritalStatus = reg.menuArrs[3][reg.selectedIndices[3]];
        reg.nationality = reg.menuArrs[4][reg.selectedIndices[4]];

        int y_offset = 2;

        // Render all menus
        for (int i = 0; i < static_cast<int>(reg.menuArrs.size()); ++i)
        {
            if (i == reg.currentMenu)
            {
                wattron(win_form, A_BOLD); // Highlight the current menu
            }
            renderHorizontalMenuStack(win_form, reg.menuArrs[i], reg.labelArr[i], y_offset, reg.selectedIndices[i]);
            if (i == reg.currentMenu)
            {
                wattroff(win_form, A_BOLD); // Turn off highlighting
            }
            y_offset += 3; // Move down for the next menu
        }

        wrefresh(win_form); // Refresh win_form to show the menus

        // Handle user input
        int ch = wgetch(win_form);
        switch (ch)
        {
        case KEY_UP:
            if (reg.currentMenu > 0)
                reg.currentMenu--; // Move up to previous menu
            break;
        case KEY_DOWN:
            if (reg.currentMenu < static_cast<int>(reg.menuArrs.size() - 1))
                reg.currentMenu++; // Move down to next menu
            break;
        case KEY_LEFT:
            if (reg.selectedIndices[reg.currentMenu] > 0)
                reg.selectedIndices[reg.currentMenu]--; // Move left in current menu
            break;
        case KEY_RIGHT:
            if (reg.selectedIndices[reg.currentMenu] < static_cast<int>(reg.menuArrs[reg.currentMenu].size() - 1))
                reg.selectedIndices[reg.currentMenu]++; // Move right in current menu
            break;
        case 10: // Enter key confirms selection and exits
            done = true;
            break;
        case 2: // Custom key for "Back"
            backHandlerRegistration(nullptr, nullptr, win_form, win_body, reg, colorScheme);
            break;
        }
    }

    if (submitRegistration(reg, colorScheme))
    {
        wbkgd(status_win, COLOR_PAIR(colorScheme.primary));
        printCentered(status_win, 1, "Registration SUCCESS! You will be redirected to the login page shortly.");
        wrefresh(status_win);
        std::this_thread::sleep_for(std::chrono::seconds(5));
        werase(status_win);
        wrefresh(status_win);
    }
    else
    {
        wbkgd(status_win, COLOR_PAIR(colorScheme.danger));
        printCentered(status_win, 1, "Registration FAILED! Please try again later.");
        wrefresh(status_win);
        std::this_thread::sleep_for(std::chrono::seconds(5));
        werase(status_win);
        wrefresh(status_win);
    }
    // Clean up
    delwin(win_form);
    delwin(win_body);
    delwin(status_win);
    clear();
    refresh();
    reg.reset();
    EventManager &eventManager = EventManager::getInstance();
    eventManager.switchScreen(Screen::Login);
}

void renderRegistrationScreen(Registration &reg)
{
    Color colorScheme;
    bkgd(COLOR_PAIR(colorScheme.primary));
    renderRegistrationAccountSection(reg, colorScheme);
}

void handleDashboardOptions(Dashboard &dash, std::string &roleStr)
{
    EventManager &eventManager = EventManager::getInstance();
    switch (dash.selectedIndex)
    {
    case 0:
        eventManager.switchScreen(Screen::Roster);
        break;
    case 1:
        eventManager.switchScreen(roleStr == "patient" ? Screen::Appointments : Screen::Userbase);
        break;
    case 2:
        eventManager.switchScreen(Screen::Profile);
        break;
    default:
        break;
    }
}

void renderTime(std::time_t time)
{
    int row = 11;
    int col = 1;

    std::string timeStr = std::ctime(&time);
    timeStr.pop_back();

    Color colorScheme;
    attron(COLOR_PAIR(colorScheme.primary));
    mvprintw(row, col, "%s", timeStr.c_str());
    attroff(COLOR_PAIR(colorScheme.primary));
    refresh();
}


void renderDashboardScreen(Dashboard &dash)
{
    Color colorScheme;
    EventManager &eventManager = EventManager::getInstance();
    UserManager &userManager = UserManager::getInstance();
    auto currentUser = userManager.getCurrentUser();
    bkgd(COLOR_PAIR(colorScheme.primary));

    std::string header = "Dashboard";
    std::string currentlyLoggedUser = "Currently logged in as " + currentUser->username;

    int baseline = 11;
    mvprintw(baseline, (COLS - header.length()) / 2, "%s", header.c_str());
    mvprintw(baseline + 2, (COLS - currentlyLoggedUser.length()) / 2, "%s", currentlyLoggedUser.c_str());
    refresh();

    int outer_height = 16;
    int outer_width = 40;

    int start_y = ((LINES - outer_height) / 2) + 4;
    int start_x = (COLS - outer_width) / 2;

    // Create the main window (win_body)
    WINDOW *win_body = newwin(outer_height, outer_width, start_y, start_x);
    wbkgd(win_body, COLOR_PAIR(colorScheme.primary));
    box(win_body, 0, 0);

    // Draw the subheader on win_body
    std::string subHeader = "Main Menu";
    mvwprintw(win_body, 1, (outer_width - subHeader.length()) / 2, "%s", subHeader.c_str());
    wrefresh(win_body); // Refresh win_body to show the subheader

    int inner_height = outer_height - 4;
    int inner_width = outer_width - 4;

    // Create the subwindow (win_form) inside win_body
    WINDOW *win_form = derwin(win_body, inner_height, inner_width, 2, 2);
    wbkgd(win_form, COLOR_PAIR(colorScheme.primary));
    box(win_form, 0, 0);
    wrefresh(win_form); // Refresh win_form to show the box

    bool done = false;
    keypad(win_form, TRUE);
    curs_set(0);

    while (!done)
    {
        // Clear the previous menu options
        werase(win_form);
        box(win_form, 0, 0);

        std::string roleStr = currentUser->getRoleToString(currentUser->role);
        std::vector<std::string> menuOptions = roleStr == "patient" ? dash.patientOptionsArr : dash.adminOptionsArr;

        // Render menu options
        for (int i = 0; i < static_cast<int>(menuOptions.size()); ++i)
        {
            // Calculate the horizontal position to center the option
            int optionLength = menuOptions[i].length();
            int xPos = (inner_width - optionLength) / 2;

            if (i == dash.selectedIndex)
            {
                wattron(win_form, A_REVERSE); // Highlight the selected option
            }

            mvwprintw(win_form, 2 + (i * 2), xPos, "%s", menuOptions[i].c_str());

            if (i == dash.selectedIndex)
            {
                wattroff(win_form, A_REVERSE); // Turn off highlighting
            }
        }

        wrefresh(win_form); // Refresh win_form to show the updated menu

        // Handle user input
        int ch = wgetch(win_form);
        switch (ch)
        {
        case KEY_UP:
            dash.selectedIndex--;
            if (dash.selectedIndex < 0)
            {
                dash.selectedIndex = menuOptions.size() - 1; // Wrap around to the last option
            }
            break;
        case KEY_DOWN:
            dash.selectedIndex++;
            if (dash.selectedIndex >= static_cast<int>(menuOptions.size()))
            {
                dash.selectedIndex = 0; // Wrap around to the first option
            }
            break;
        case 10: // Enter key confirms selection and exits
            handleDashboardOptions(dash, roleStr);
            done = true;
            break;
        case 27:
            exitHandler();
            break;
        case 2:
            done = true;
            break;
        }
    }

    // Clean up windows
    wclear(win_form);
    wclear(win_body);
    delwin(win_form);
    delwin(win_body);
    clear();
    refresh();
    dash.reset();
    eventManager.switchScreen(Screen::Login);
}
