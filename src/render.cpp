#include "render.hpp"
#include "EventManager.hpp"
#include "UserManager.hpp"

/* 1. HELPERS */

void clearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
};

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

void exitHandler(FORM *form, FIELD **fields, std::vector<WINDOW *> &windows)
{
    // Clean up form
    if (form)
    {
        unpost_form(form);
        free_form(form);
        form = nullptr;
    }

    // Free all fields
    if (fields)
    {
        for (int i = 0; fields[i]; i++)
        {
            free_field(fields[i]);
        }
        fields = nullptr;
    }

    // Clear and delete all windows
    for (WINDOW *win : windows)
    {
        wclear(win);
        wrefresh(win);
        delwin(win);
    }

    // Restore terminal settings to normal mode
    endwin();

    // Exit the program
    EventManager &eventManager = EventManager::getInstance();
    eventManager.exit();
}

void navigationHandler(FORM *form, FIELD **fields, std::vector<WINDOW *> &windows, Screen screen)
{
    // Get the instance of EventManager for screen switching
    EventManager &eventManager = EventManager::getInstance();

    // Clean up the form if it exists
    if (form)
    {
        unpost_form(form);
        free_form(form);
        form = nullptr;
    }

    if (fields)
    {
        for (int i = 0; fields[i]; i++)
        {
            free_field(fields[i]);
        }
        fields = nullptr;
    }

    // Clear and delete each window in the vector
    for (WINDOW *win : windows)
    {
        if (win)
        {
            wclear(win);
            wrefresh(win);
            delwin(win);
        }
    }

    // Clear the vector since all windows have been deleted
    windows.clear();

    // Clear the screen and refresh
    clear();
    refresh();

    // Switch to the next screen
    eventManager.switchScreen(screen);
}

void driver_form(int ch, FORM *form, FIELD **fields, WINDOW *win_form, WINDOW *win_body,
                 std::function<void()> exitHandler, std::function<void()> navigationHandler)
{
    switch (ch)
    {
    case KEY_DOWN:
        // Move to the next field and end the current line
        form_driver(form, REQ_NEXT_FIELD);
        form_driver(form, REQ_END_LINE);
        break;
    case KEY_UP:
        // Move to the previous field and end the current line
        form_driver(form, REQ_PREV_FIELD);
        form_driver(form, REQ_END_LINE);
        break;
    case KEY_LEFT:
        // Move the cursor to the previous character in the current field
        form_driver(form, REQ_PREV_CHAR);
        break;
    case KEY_RIGHT:
        // Move the cursor to the next character in the current field
        form_driver(form, REQ_NEXT_CHAR);
        break;
    case KEY_BACKSPACE:
    case 127:
        // Delete the previous character
        form_driver(form, REQ_DEL_PREV);
        break;
    case 27: // Escape key
        // Call exit handler if provided
        if (exitHandler)
        {
            exitHandler();
        }
        break;
    case 2: // Custom key for "Back"
        // Call navigation handler if provided
        if (navigationHandler)
        {
            navigationHandler();
        }
        return; // Exit early after "Back" key press
    case KEY_DC:
        // Delete the current character
        form_driver(form, REQ_DEL_CHAR);
        break;
    default:
        // Process other keys with form_driver
        form_driver(form, ch);
        break;
    }

    // Refresh the form window to show changes
    wrefresh(win_form);
}

bool validateFields(FIELD **fields)
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

/* 2. COMMON COMPONENTS */

void renderHeader()
{
    Color &colorScheme = Color::getInstance();
    attron(COLOR_PAIR(colorScheme.primary));
    bkgd(COLOR_PAIR(colorScheme.primary));

    int row = 0;
    int col = (COLS - 81) / 2;

    WINDOW *header_win = newwin(10, 82, row, col);
    box(header_win, 0, 0);

    wattron(header_win, A_BOLD);
    mvwprintw(header_win, 1, 1, "  /$$      /$$                 /$$ /$$$$$$$$           /$$                      ");
    mvwprintw(header_win, 2, 1, " | $$$    /$$$                | $$|__  $$__/          | $$               /$$    ");
    mvwprintw(header_win, 3, 1, " | $$$$  /$$$$  /$$$$$$   /$$$$$$$   | $$     /$$$$$$ | $$   /$$        | $$    ");
    mvwprintw(header_win, 4, 1, " | $$ $$/$$ $$ /$$__  $$ /$$__  $$   | $$    /$$__  $$| $$  /$$/      /$$$$$$$$ ");
    mvwprintw(header_win, 5, 1, " | $$  $$$| $$| $$$$$$$$| $$  | $$   | $$   | $$$$$$$$| $$$$$$/      |__  $$__/ ");
    mvwprintw(header_win, 6, 1, " | $$\\  $ | $$| $$_____/| $$  | $$   | $$   | $$_____/| $$_  $$         | $$   ");
    mvwprintw(header_win, 7, 1, " | $$ \\/  | $$|  $$$$$$$|  $$$$$$$   | $$   |  $$$$$$$| $$ \\  $$        |__/  ");
    mvwprintw(header_win, 8, 1, " |__/     |__/ \\_______/ \\_______/   |__/    \\_______/|__/  \\__/            ");
    wattroff(header_win, A_BOLD);

    refresh();
    wbkgd(header_win, COLOR_PAIR(colorScheme.primary));
    wrefresh(header_win);
    delwin(header_win);
}

void renderControlInfo()
{
    Color &colorScheme = Color::getInstance();
    Controls controls;

    int outer_width = 38;                              // Increased width for better readability
    int outer_height = 8 + controls.controlArr.size(); // Dynamic height

    int inner_width = outer_width - 4;
    int inner_height = outer_height - 4;

    int start_y = 0; // Positioning
    int start_x = 2;

    // Create outer window (bordered box)
    WINDOW *win_outer = newwin(outer_height, outer_width, start_y, start_x);
    wbkgd(win_outer, COLOR_PAIR(colorScheme.primary));
    box(win_outer, 0, 0);

    // Print header
    wattron(win_outer, A_BOLD);
    mvwprintw(win_outer, 1, (outer_width - controls.header.length()) / 2, "%s", controls.header.c_str());
    wattroff(win_outer, A_BOLD);
    wrefresh(win_outer);

    // Create inner window (for control descriptions)
    WINDOW *win_inner = derwin(win_outer, inner_height, inner_width, 2, 2);
    wbkgd(win_inner, COLOR_PAIR(colorScheme.primary));
    box(win_inner, 0, 0);

    // Render controls
    int row = 1;
    for (const auto &control : controls.controlArr)
    {
        if (control.find('|') != std::string::npos) // If it's a section title
        {
            wattron(win_inner, A_BOLD | A_UNDERLINE);
            mvwprintw(win_inner, row, (inner_width - control.length()) / 2, "%s", control.c_str());
            wattroff(win_inner, A_BOLD | A_UNDERLINE);
        }
        else
        {
            mvwprintw(win_inner, row, 2, "%s", control.c_str());
        }
        row++;
    }

    wrefresh(win_inner);

    // Wait before closing to avoid flickering (optional)
    napms(100);

    // Cleanup
    delwin(win_inner);
    delwin(win_outer);
}

void renderHorizontalMenuStack(WINDOW *win, const std::vector<std::string> &items, const std::string &title, int y_offset, int &selected_index, int start_x)
{
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

/* 3. SCREENS */

void renderLoginScreen()
{
    UserManager &userManager = UserManager::getInstance();
    Color &colorScheme = Color::getInstance();
    userManager.setCurrentUser(nullptr);

    bkgd(COLOR_PAIR(colorScheme.primary));

    // Render subheaders
    std::string subHeader1 = "Welcome to MedTek+ administrator TUI";
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
    wattron(win_body, A_BOLD);
    std::string loginForm = "Login";
    mvwprintw(win_body, 1, (50 - loginForm.length()) / 2, "%s", loginForm.c_str());
    wattroff(win_body, A_BOLD);

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

    // Refresh the windows
    wrefresh(win_body);
    wrefresh(win_form);

    std::vector<WINDOW *> windows = {win_body, win_form};

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
            exitHandler(nullptr, nullptr, windows);
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
        navigationHandler(nullptr, nullptr, windows, Screen::Dashboard);
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

void renderRegistrationAccountPatientScreen()
{
    // Render the header and control information (e.g., buttons, navigation options)
    renderHeader();
    renderControlInfo();

    // Get the singleton instances of color scheme and registration data
    Color &colorScheme = Color::getInstance();
    RegistrationPatient &reg = RegistrationPatient::getInstance();

    // Set the header for the registration screen and center it
    int baseline = 11;
    std::string header = "Register a MedTek+ Patient Account";
    mvprintw(baseline, (COLS - header.length()) / 2, "%s", header.c_str());

    // Set the background color for the screen
    bkgd(COLOR_PAIR(colorScheme.primary));

    // Define the dimensions and position for the outer window
    int outer_height = 16;
    int outer_width = 60;
    int start_y = ((LINES - outer_height) / 2) + 2;
    int start_x = (COLS - outer_width) / 2;

    // Create the window for the registration form
    WINDOW *win_body = newwin(outer_height, outer_width, start_y, start_x);
    wbkgd(win_body, COLOR_PAIR(colorScheme.primary));
    box(win_body, 0, 0); // Draw a border around the window

    // Set the subheader for the account section
    std::string subHeader = "Account Information";
    mvwprintw(win_body, 1, (outer_width - subHeader.length()) / 2, "%s", subHeader.c_str());

    // Define the inner window dimensions for the form fields
    int inner_height = outer_height - 4;
    int inner_width = outer_width - 4;
    WINDOW *win_form = derwin(win_body, inner_height, inner_width, 2, 2); // Create a sub-window for the form
    wbkgd(win_form, COLOR_PAIR(colorScheme.primary));
    box(win_form, 0, 0); // Draw a border around the form window

    // Create the fields for the registration form
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
    fields[10] = NULL;                         // End of fields array

    // Ensure fields were created successfully
    assert(fields[0] && fields[1] && fields[2] && fields[3] && fields[4] &&
           fields[5] && fields[6] && fields[7] && fields[8] && fields[9]);

    // Set initial values for each field based on registration data
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

    // Set options for each field (e.g., visibility, editable, static)
    for (int i = 0; fields[i]; i++)
    {
        if (i % 2 == 0)
        {                                                               // Label fields
            set_field_opts(fields[i], O_VISIBLE | O_PUBLIC | O_STATIC); // Read-only
        }
        else
        { // Input fields
            set_field_opts(fields[i], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE | O_AUTOSKIP);
        }
        set_field_back(fields[i], COLOR_PAIR(colorScheme.primary)); // Set background color for fields
    }

    // Create the form from the fields and post it to the window
    FORM *form = new_form(fields);
    assert(form);
    set_form_win(form, win_form);
    set_form_sub(form, derwin(win_form, inner_height - 2, inner_width - 2, 1, 1));
    post_form(form);

    // Refresh the windows to display the form
    wrefresh(win_body);
    wrefresh(win_form);

    // Create a dedicated window for displaying error messages
    int error_win_height = 1;
    int error_win_width = 40;
    int error_win_y = LINES - 2;
    int error_win_x = (COLS - error_win_width) / 2;
    WINDOW *error_win = newwin(error_win_height, error_win_width, error_win_y, error_win_x);
    wbkgd(error_win, COLOR_PAIR(colorScheme.danger)); // Set background color for error window

    std::vector<WINDOW *> windows = {win_body, win_form, error_win};

    // Input loop for form validation
    bool done = false;
    int ch;

    while (!done)
    {
        while ((ch = getch()) != '\n') // Continue until Enter key is pressed
        {
            driver_form(
                ch,
                form,
                fields,
                win_form,
                win_body,
                [&]()
                { exitHandler(form, fields, windows); }, // Handle exit
                [&]()
                {
                    reg.reset();
                    navigationHandler(form, fields, windows, Screen::Database);
                });
        }

        // Validate the form data
        form_driver(form, REQ_VALIDATION);

        std::string errorMessage;

        // Validate required fields
        if (!validateFields(fields))
        {
            errorMessage = "Please fill all required fields";
        }
        else if (!validateEmail(trim_whitespaces(field_buffer(fields[5], 0))))
        {
            errorMessage = "Please enter a valid email";
        }
        else if (!validateContactNumber(trim_whitespaces(field_buffer(fields[9], 0))))
        {
            errorMessage = "Please enter a valid phone no.";
        }
        // If there's an error, display it
        if (!errorMessage.empty())
        {
            mvwprintw(error_win, 0, 0, "%s", errorMessage.c_str());
            wrefresh(error_win);
            std::this_thread::sleep_for(std::chrono::seconds(2)); // Show error for 2 seconds
            werase(error_win);
            wrefresh(error_win); // Clear error window
        }
        else
        {
            done = true; // Proceed if validation succeeds
        }
    }

    form_driver(form, REQ_VALIDATION); // Final validation

    // Extract the form data into the registration object
    reg.username = trim_whitespaces(field_buffer(fields[1], 0));
    reg.password = trim_whitespaces(field_buffer(fields[3], 0));
    reg.email = trim_whitespaces(field_buffer(fields[5], 0));
    reg.address = trim_whitespaces(field_buffer(fields[7], 0));
    reg.contactNumber = trim_whitespaces(field_buffer(fields[9], 0));

    navigationHandler(form, fields, windows, Screen::RegistrationPersonalPatientScreen);
}

void renderRegistrationPersonalPatientScreen()
{
    // Render header and control info
    renderHeader();
    renderControlInfo();

    // Get the color scheme and registration instance
    Color &colorScheme = Color::getInstance();
    RegistrationPatient &reg = RegistrationPatient::getInstance();

    // Define and print the main header for the screen
    int baseline = 11;
    std::string header = "Register a MedTek+ Patient Account";
    mvprintw(baseline, (COLS - header.length()) / 2, "%s", header.c_str());

    // Set background color to the primary color from color scheme
    bkgd(COLOR_PAIR(colorScheme.primary));

    // Window dimensions and position for outer body
    int outer_height = 20;
    int outer_width = 60;
    int start_y = ((LINES - outer_height) / 2) + 4;
    int start_x = (COLS - outer_width) / 2;

    // Create and configure the outer window (main body)
    WINDOW *win_body = newwin(outer_height, outer_width, start_y, start_x);
    wbkgd(win_body, COLOR_PAIR(colorScheme.primary)); // Set background color for body window
    box(win_body, 0, 0);                              // Draw box around the window

    // Print the sub-header in the body window
    std::string subHeader = "Personal Information";
    mvwprintw(win_body, 1, (outer_width - subHeader.length()) / 2, "%s", subHeader.c_str());

    // Calculate dimensions for the inner form window
    int inner_height = outer_height - 4;
    int inner_width = outer_width - 4;

    // Create and configure the inner form window
    WINDOW *win_form = derwin(win_body, inner_height, inner_width, 2, 2);
    wbkgd(win_form, COLOR_PAIR(colorScheme.primary)); // Set background color for form window
    box(win_form, 0, 0);                              // Draw box around the form window

    // Initialize fields for the form (label and input field pairs)
    FIELD *fields[13];
    fields[0] = new_field(1, 15, 0, 1, 0, 0);    // Label: fullName
    fields[1] = new_field(1, 35, 0, 18, 0, 0);   // Input: fullName
    fields[2] = new_field(1, 32, 2, 1, 0, 0);    // Label: identityCardNumber
    fields[3] = new_field(1, 18, 2, 34, 0, 0);   // Input: identityCardNumber
    fields[4] = new_field(1, 15, 4, 1, 0, 0);    // Label: height
    fields[5] = new_field(1, 35, 4, 18, 0, 0);   // Input: height
    fields[6] = new_field(1, 15, 6, 1, 0, 0);    // Label: weight
    fields[7] = new_field(1, 35, 6, 18, 0, 0);   // Input: weight
    fields[8] = new_field(1, 30, 8, 1, 0, 0);    // Label: emergencyContactNumber
    fields[9] = new_field(1, 22, 8, 28, 0, 0);   // Input: emergencyContactNumber
    fields[10] = new_field(1, 28, 10, 1, 0, 0);  // Label: emergencyContactName
    fields[11] = new_field(1, 22, 10, 28, 0, 0); // Input: emergencyContactName
    fields[12] = NULL;                           // Null-terminate the fields array

    // Ensure all fields were created successfully
    assert(fields[0] && fields[1] && fields[2] && fields[3] && fields[4] &&
           fields[5] && fields[6] && fields[7] && fields[8] && fields[9] && fields[10] && fields[11]);

    // Set initial values for each field buffer
    set_field_buffer(fields[0], 0, "Full Name:");
    set_field_buffer(fields[1], 0, reg.fullName.c_str());
    set_field_buffer(fields[2], 0, "IC number (e.g. 010403141107):");
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
        if (i % 2 == 0) // Labels (read-only)
        {
            set_field_opts(fields[i], O_VISIBLE | O_PUBLIC | O_STATIC);
        }
        else // Input fields (editable)
        {
            set_field_opts(fields[i], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE | O_AUTOSKIP);
        }
        set_field_back(fields[i], COLOR_PAIR(colorScheme.primary)); // Set background color for fields
    }

    // Create the form with the defined fields and configure it
    FORM *form = new_form(fields);
    assert(form); // Ensure form creation was successful
    set_form_win(form, win_form);
    set_form_sub(form, derwin(win_form, inner_height - 2, inner_width - 2, 1, 1));
    post_form(form); // Post the form (make it visible)

    // Refresh the windows to display content
    wrefresh(win_body);
    wrefresh(win_form);

    // Create a dedicated error window for validation messages
    int error_win_height = 1;
    int error_win_width = 40;
    int error_win_y = LINES - 2;
    int error_win_x = (COLS - error_win_width) / 2;

    WINDOW *error_win = newwin(error_win_height, error_win_width, error_win_y, error_win_x);
    wbkgd(error_win, COLOR_PAIR(colorScheme.danger)); // Set background color for error messages

    std::vector<WINDOW *> windows = {win_body, win_form, error_win};

    // Input loop to process the form and handle validation
    bool done = false;
    int ch;

    while (!done)
    {
        // Wait for input until 'Enter' is pressed
        while ((ch = getch()) != '\n')
        {
            // Handle form navigation and other key inputs
            driver_form(
                ch,
                form,
                fields,
                win_form,
                win_body,
                [&]()
                { exitHandler(form, fields, windows); },
                [&]()
                {
                    navigationHandler(form, fields, windows, Screen::RegistrationAccountPatientScreen);
                });
        }

        // Validate the form when 'Enter' is pressed
        form_driver(form, REQ_VALIDATION);

        std::string errorMessage;

        // Validate required fields
        if (!validateFields(fields))
        {
            errorMessage = "Please fill all required fields";
        }
        else if (!validateIdentityCardNumber(trim_whitespaces(field_buffer(fields[3], 0))))
        {
            errorMessage = "Please enter a valid IC number";
        }
        else if (!validateHeightAndWeight(trim_whitespaces(field_buffer(fields[5], 0)), trim_whitespaces(field_buffer(fields[7], 0))))
        {
            errorMessage = "Please enter a valid height/weight";
        }
        else if (!validateContactNumber(trim_whitespaces(field_buffer(fields[9], 0))))
        {
            errorMessage = "Please enter a valid emergency contact no.";
        }
        // If there's an error, display it
        if (!errorMessage.empty())
        {
            mvwprintw(error_win, 0, 0, "%s", errorMessage.c_str());
            wrefresh(error_win);
            std::this_thread::sleep_for(std::chrono::seconds(2)); // Show error for 2 seconds
            werase(error_win);
            wrefresh(error_win); // Clear error window
        }
        else
        {
            done = true; // Proceed if validation succeeds
        }
    }

    // Final form validation
    form_driver(form, REQ_VALIDATION);

    // Extract and clean form data
    reg.fullName = trim_whitespaces(field_buffer(fields[1], 0));
    reg.identityCardNumber = trim_whitespaces(field_buffer(fields[3], 0));
    reg.height = trim_whitespaces(field_buffer(fields[5], 0));
    reg.weight = trim_whitespaces(field_buffer(fields[7], 0));
    reg.emergencyContactNumber = trim_whitespaces(field_buffer(fields[9], 0));
    reg.emergencyContactName = trim_whitespaces(field_buffer(fields[11], 0));

    Admission &a = Admission::getInstance();
    a.prevScreen = Screen::RegistrationSelectionPatientScreen;
    navigationHandler(form, fields, windows, Screen::Admission);
}

void renderRegistrationSelectionPatientScreen()
{
    Color &colorScheme = Color::getInstance();                     // Color scheme instance
    RegistrationPatient &reg = RegistrationPatient::getInstance(); // RegistrationPatient instance
    renderHeader();                                                // Render header
    renderControlInfo();                                           // Render control information

    bkgd(COLOR_PAIR(colorScheme.primary)); // Set background color

    int baseline = 11;
    std::string header = "Register a MedTek+ Patient Account";
    mvprintw(baseline, (COLS - header.length()) / 2, "%s", header.c_str());
    refresh();

    int outer_height = 20;
    int outer_width = 60;

    // Calculate window position
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
    int status_win_width = 34;
    int status_win_y = LINES - 6;
    int status_win_x = (COLS - status_win_width) / 2;

    WINDOW *status_win = newwin(status_win_height, status_win_width, status_win_y, status_win_x);
    box(status_win, 0, 0);

    bool done = false;
    keypad(win_form, TRUE); // Enable keypad for win_form
    curs_set(0);            // Hide cursor

    // Loop for user input
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
            renderHorizontalMenuStack(win_form, reg.menuArrs[i], reg.labelArr[i], y_offset, reg.selectedIndices[i], 2);
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
        case KEY_UP: // Move up to previous menu
            if (reg.currentMenu > 0)
                reg.currentMenu--;
            break;
        case KEY_DOWN: // Move down to next menu
            if (reg.currentMenu < static_cast<int>(reg.menuArrs.size() - 1))
                reg.currentMenu++;
            break;
        case KEY_LEFT: // Move left in current menu
            if (reg.selectedIndices[reg.currentMenu] > 0)
                reg.selectedIndices[reg.currentMenu]--;
            break;
        case KEY_RIGHT: // Move right in current menu
            if (reg.selectedIndices[reg.currentMenu] < static_cast<int>(reg.menuArrs[reg.currentMenu].size() - 1))
                reg.selectedIndices[reg.currentMenu]++;
            break;
        case 10: // Enter key confirms selection and exits
            done = true;
            break;
        case 2: // Custom key for "Back"
            std::vector<WINDOW *> windows = {win_body, win_form};
            navigationHandler(nullptr, nullptr, windows, Screen::RegistrationPersonalPatientScreen);
            break;
        }
    }

    // Handle registration success or failure
    if (submitRegistrationPatient())
    {
        wbkgd(status_win, COLOR_PAIR(colorScheme.primary));
        printCentered(status_win, 1, "Registration SUCCESS! You will be redirected to the database page shortly.");
        wrefresh(status_win);
        std::this_thread::sleep_for(std::chrono::seconds(2));
        werase(status_win);
        wrefresh(status_win);
        reg.reset();
        std::vector<WINDOW *> windows = {win_body, win_form};
        navigationHandler(nullptr, nullptr, windows, Screen::Database);
    }
    else
    {
        wbkgd(status_win, COLOR_PAIR(colorScheme.danger));
        printCentered(status_win, 1, "Registration FAILED! Please try again later.");
        wrefresh(status_win);
        std::this_thread::sleep_for(std::chrono::seconds(2));
        werase(status_win);
        wrefresh(status_win);
    }
}

bool submitRegistrationPatient()
{
    RegistrationPatient &reg = RegistrationPatient::getInstance();
    Admission &a = Admission::getInstance();
    try
    {
        // Accessing UserManager instance and creating a new patient
        UserManager &userManager = UserManager::getInstance();
        userManager.createPatient(
            reg.username,
            reg.password,
            calculateAge(reg.identityCardNumber),
            reg.fullName,
            reg.religion,
            reg.nationality,
            reg.identityCardNumber,
            reg.maritalStatus,
            reg.gender,
            reg.race,
            reg.email,
            reg.contactNumber,
            reg.emergencyContactNumber,
            reg.emergencyContactName,
            reg.address,
            calculateBMI(reg.weight, reg.height),
            reg.height,
            reg.weight,
            a.selectedDepartment);

        return true; // Registration successful
    }
    catch (const std::exception &e)
    {
        return false; // Registration failed due to exception
    }
    return false; // Fallback return (unreachable code)
}

bool submitRegistrationAdmin(RegistrationAdmin &reg)
{
    try
    {
        // Accessing UserManager instance and creating a new admin account
        UserManager &userManager = UserManager::getInstance();
        userManager.createAdmin(reg.username, reg.password, reg.fullName, reg.email, reg.contactNumber);
        return true; // Registration successful
    }
    catch (const std::exception &e)
    {
        return false; // Registration failed due to exception
    }
    return false; // Fallback return (unreachable code)
}

void renderRegistrationScreenAdmin()
{
    Color &colorScheme = Color::getInstance();                 // Color scheme instance
    RegistrationAdmin &reg = RegistrationAdmin::getInstance(); // RegistrationAdmin instance
    bkgd(COLOR_PAIR(colorScheme.primary));                     // Set background color

    renderHeader();      // Render header
    renderControlInfo(); // Render control information

    int baseline = 11;
    std::string header = "Register a MedTek+ Admin Account";
    mvprintw(baseline, (COLS - header.length()) / 2, "%s", header.c_str());

    bkgd(COLOR_PAIR(colorScheme.primary));

    int outer_height = 16;
    int outer_width = 60;

    // Calculate window position
    int start_y = ((LINES - outer_height) / 2) + 2;
    int start_x = (COLS - outer_width) / 2;

    // Create the main window (win_body)
    WINDOW *win_body = newwin(outer_height, outer_width, start_y, start_x);
    wbkgd(win_body, COLOR_PAIR(colorScheme.primary));
    box(win_body, 0, 0);

    std::string subHeader = "Account Information";
    mvwprintw(win_body, 1, (outer_width - subHeader.length()) / 2, "%s", subHeader.c_str());

    int inner_height = outer_height - 4;
    int inner_width = outer_width - 4;

    // Create the subwindow (win_form) inside win_body
    WINDOW *win_form = derwin(win_body, inner_height, inner_width, 2, 2);
    wbkgd(win_form, COLOR_PAIR(colorScheme.primary));
    box(win_form, 0, 0);

    // Define form fields
    FIELD *fields[11];
    fields[0] = new_field(1, 10, 0, 1, 0, 0);  // Label: Username
    fields[1] = new_field(1, 30, 0, 13, 0, 0); // Input: Username
    fields[2] = new_field(1, 10, 2, 1, 0, 0);  // Label: Password
    fields[3] = new_field(1, 30, 2, 13, 0, 0); // Input: Password
    fields[4] = new_field(1, 10, 4, 1, 0, 0);  // Label: Full Name
    fields[5] = new_field(1, 30, 4, 13, 0, 0); // Input: Full Name
    fields[6] = new_field(1, 10, 6, 1, 0, 0);  // Label: Email
    fields[7] = new_field(1, 30, 6, 13, 0, 0); // Input: Email
    fields[8] = new_field(1, 16, 8, 1, 0, 0);  // Label: Contact Number
    fields[9] = new_field(1, 30, 8, 20, 0, 0); // Input: Contact Number
    fields[10] = NULL;

    // Assert all fields are initialized properly
    assert(fields[0] && fields[1] && fields[2] && fields[3] && fields[4] &&
           fields[5] && fields[6] && fields[7] && fields[8] && fields[9]);

    // Set the initial values of the fields
    set_field_buffer(fields[0], 0, "Username:");
    set_field_buffer(fields[1], 0, reg.username.c_str());
    set_field_buffer(fields[2], 0, "Password:");
    set_field_buffer(fields[3], 0, reg.password.c_str());
    set_field_buffer(fields[4], 0, "Full Name:");
    set_field_buffer(fields[5], 0, reg.fullName.c_str());
    set_field_buffer(fields[6], 0, "Email:");
    set_field_buffer(fields[7], 0, reg.email.c_str());
    set_field_buffer(fields[8], 0, "Contact Number:");
    set_field_buffer(fields[9], 0, reg.contactNumber.c_str());

    // Set field options: read-only for labels, editable for inputs
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

    // Create and post the form
    FORM *form = new_form(fields);
    assert(form);
    set_form_win(form, win_form);
    set_form_sub(form, derwin(win_form, inner_height - 2, inner_width - 2, 1, 1));
    post_form(form);

    // Refresh windows
    wrefresh(win_body);
    wrefresh(win_form);

    // Create dedicated error and status windows
    int error_win_height = 1;
    int error_win_width = 40;
    int error_win_y = LINES - 2;
    int error_win_x = (COLS - error_win_width) / 2;

    WINDOW *error_win = newwin(error_win_height, error_win_width, error_win_y, error_win_x);
    wbkgd(error_win, COLOR_PAIR(colorScheme.danger));

    // Create status window
    int status_win_height = 5;
    int status_win_width = 34;
    int status_win_y = LINES - 6;
    int status_win_x = (COLS - status_win_width) / 2;

    WINDOW *status_win = newwin(status_win_height, status_win_width, status_win_y, status_win_x);
    box(status_win, 0, 0);

    std::vector<WINDOW *> windows = {win_body, win_form, status_win};

    // Input loop
    bool done = false;
    int ch;

    while (!done)
    {
        while ((ch = getch()) != '\n') // Wait for Enter key to submit form
        {
            driver_form(
                ch,
                form,
                fields,
                win_form,
                win_body,
                [&]()
                { exitHandler(form, fields, windows); },
                [&]()
                {
                    reg.reset(); // Reset the form
                    navigationHandler(form, fields, windows, Screen::Database);
                });
        }
        form_driver(form, REQ_VALIDATION); // Validate the form

        std::string errorMessage;

        // Validate required fields
        if (!validateFields(fields))
        {
            errorMessage = "Please fill all required fields";
        }
        else if (!validateEmail(trim_whitespaces(field_buffer(fields[7], 0))))
        {
            errorMessage = "Please enter a valid email";
        }
        // If there's an error, display it
        if (!errorMessage.empty())
        {
            mvwprintw(error_win, 0, 0, "%s", errorMessage.c_str());
            wrefresh(error_win);
            std::this_thread::sleep_for(std::chrono::seconds(2)); // Show error for 2 seconds
            werase(error_win);
            wrefresh(error_win); // Clear error window
        }
        else
        {
            done = true; // Proceed if validation succeeds
        }
    }

    form_driver(form, REQ_VALIDATION); // Final validation

    // Extract form data and trim whitespaces
    reg.username = trim_whitespaces(field_buffer(fields[1], 0));
    reg.password = trim_whitespaces(field_buffer(fields[3], 0));
    reg.fullName = trim_whitespaces(field_buffer(fields[5], 0));
    reg.email = trim_whitespaces(field_buffer(fields[7], 0));
    reg.contactNumber = trim_whitespaces(field_buffer(fields[9], 0));

    // Submit the registration
    if (submitRegistrationAdmin(reg))
    {
        wbkgd(status_win, COLOR_PAIR(colorScheme.primary));
        printCentered(status_win, 1, "Registration SUCCESS! You will be redirected to the database page shortly.");
        wrefresh(status_win);
        std::this_thread::sleep_for(std::chrono::seconds(3)); // Show status message for 3 seconds
        werase(status_win);
        wrefresh(status_win);
        reg.reset();                                                // Reset the registration data
        navigationHandler(form, fields, windows, Screen::Database); // Redirect to database screen
    }
    else
    {
        wbkgd(status_win, COLOR_PAIR(colorScheme.danger));
        printCentered(status_win, 1, "Registration FAILED! Please try again later.");
        wrefresh(status_win);
        std::this_thread::sleep_for(std::chrono::seconds(3)); // Show status message for 3 seconds
        werase(status_win);
        wrefresh(status_win);
    }
}

// Handles the selection of different options from the dashboard menu.
void handleDashboardOptions()
{
    EventManager &eventManager = EventManager::getInstance(); // Event manager for handling screen transitions.
    UserManager &userManager = UserManager::getInstance();    // User manager to handle user-related actions.
    Profile &p = Profile::getInstance();                      // Profile to manage user profile info.
    Dashboard &dash = Dashboard::getInstance();               // Dashboard to manage dashboard state.

    switch (dash.selectedIndex) // Switch based on selected index from the menu.
    {
    case 0:
        eventManager.switchScreen(Screen::Database); // Navigate to the database screen.
        dash.reset();                                // Reset the dashboard state.
        break;
    case 1:
        p.prevScreen = Screen::Dashboard;           // Save the previous screen.
        p.user = userManager.getCurrentUser();      // Get the current logged-in user.
        eventManager.switchScreen(Screen::Profile); // Switch to the profile screen.
        dash.reset();                               // Reset the dashboard state.
        break;
    case 2:
        eventManager.switchScreen(Screen::Login); // Navigate to the login screen.
        dash.reset();                             // Reset the dashboard state.
        break;
    default:
        break; // Do nothing if the selected index doesn't match any case.
    }
}

// Renders the dashboard screen, handles user input for navigation, and displays the main menu options.
void renderDashboardScreen()
{
    Color &colorScheme = Color::getInstance();             // Get the color scheme for UI elements.
    UserManager &userManager = UserManager::getInstance(); // User manager for getting user data.
    Dashboard &dash = Dashboard::getInstance();            // Dashboard instance to manage the current state.
    auto currentUser = userManager.getCurrentUser();       // Get the current logged-in user.
    bkgd(COLOR_PAIR(colorScheme.primary));                 // Set the background color for the screen.

    std::string header = "Dashboard";                                                    // Set the header text for the screen.
    std::string currentlyLoggedUser = "Currently logged in as " + currentUser->username; // Display the logged-in user's name.

    int baseline = 11;
    mvprintw(baseline, (COLS - header.length()) / 2, "%s", header.c_str());                               // Print header centered on screen.
    mvprintw(baseline + 2, (COLS - currentlyLoggedUser.length()) / 2, "%s", currentlyLoggedUser.c_str()); // Print the logged-in user's name.
    refresh();                                                                                            // Refresh the screen to display the changes.

    // Define the size and position for the main window (win_body) that holds the menu.
    int outer_height = 16;
    int outer_width = 40;
    int start_y = ((LINES - outer_height) / 2) + 4;
    int start_x = (COLS - outer_width) / 2;

    // Create the main window (win_body).
    WINDOW *win_body = newwin(outer_height, outer_width, start_y, start_x);
    wbkgd(win_body, COLOR_PAIR(colorScheme.primary)); // Set the background color for win_body.
    box(win_body, 0, 0);                              // Draw a box around the window.

    // Draw the subheader on the win_body window.
    std::string subHeader = "Main Menu";
    wattron(win_body, A_BOLD);                                                               // Turn on bold text for the subheader.
    mvwprintw(win_body, 1, (outer_width - subHeader.length()) / 2, "%s", subHeader.c_str()); // Print subheader centered on win_body.
    wattroff(win_body, A_BOLD);                                                              // Turn off bold text.
    wrefresh(win_body);                                                                      // Refresh the window to show the subheader.

    // Define the size of the inner window (win_form) where the menu options will be displayed.
    int inner_height = outer_height - 4;
    int inner_width = outer_width - 4;

    // Create the subwindow (win_form) inside win_body for displaying the menu options.
    WINDOW *win_form = derwin(win_body, inner_height, inner_width, 2, 2);
    wbkgd(win_form, COLOR_PAIR(colorScheme.primary)); // Set the background color for win_form.
    box(win_form, 0, 0);                              // Draw a box around the inner window.
    wrefresh(win_form);                               // Refresh win_form to show the box.

    std::vector<WINDOW *> windows = {win_body, win_form};

    bool done = false;      // Loop control flag.
    keypad(win_form, TRUE); // Enable keypad input for win_form.
    curs_set(0);            // Hide the cursor.

    while (!done)
    {
        // Clear the previous menu options to update them.
        werase(win_form);
        box(win_form, 0, 0);

        // Render the menu options inside win_form.
        for (int i = 0; i < static_cast<int>(dash.optionsArr.size()); ++i)
        {
            // Calculate the horizontal position to center each menu option.
            int optionLength = dash.optionsArr[i].length();
            int xPos = (inner_width - optionLength) / 2;

            if (i == dash.selectedIndex) // If this option is selected, highlight it.
            {
                wattron(win_form, A_REVERSE); // Turn on reverse video for highlighting.
            }

            mvwprintw(win_form, 2 + (i * 2), xPos, "%s", dash.optionsArr[i].c_str()); // Print each option.

            if (i == dash.selectedIndex) // If this option was highlighted, turn off the highlight.
            {
                wattroff(win_form, A_REVERSE);
            }
        }

        wrefresh(win_form); // Refresh win_form to show the updated menu options.

        // Handle user input.
        int ch = wgetch(win_form); // Get user input.
        switch (ch)
        {
        case KEY_UP: // If the user presses the UP key, move the selection up.
            dash.selectedIndex--;
            if (dash.selectedIndex < 0) // If the index goes out of bounds, wrap around to the last option.
            {
                dash.selectedIndex = dash.optionsArr.size() - 1;
            }
            break;
        case KEY_DOWN: // If the user presses the DOWN key, move the selection down.
            dash.selectedIndex++;
            if (dash.selectedIndex >= static_cast<int>(dash.optionsArr.size())) // If the index goes out of bounds, wrap around to the first option.
            {
                dash.selectedIndex = 0;
            }
            break;
        case 10: // If the user presses Enter, exit the loop and handle the selection.
            done = true;
            break;
        case 27: // If the user presses ESC, exit the application.
            exitHandler(nullptr, nullptr, windows);
            break;
        case 2:
            navigationHandler(nullptr, nullptr, windows, Screen::Login);
            break;
        }
    }

    // Clean up windows after the loop finishes.
    wclear(win_form);
    wclear(win_body);
    delwin(win_form);
    delwin(win_body);
    clear();                  // Clear the screen.
    refresh();                // Refresh the screen.
    handleDashboardOptions(); // Call the function to handle the action for the selected option.
}

void handleDatabaseControls(WINDOW *win_form, WINDOW *win_body)
{
    // Retrieve instances of essential objects for handling the database logic.
    UserManager &userManager = UserManager::getInstance(); // Manages user data and actions.
    Profile &p = Profile::getInstance();                   // Profile management for individual users.
    Database &db = Database::getInstance();                // Database management for record retrieval and filtering.
    UpdatePatient &up = UpdatePatient::getInstance();      // Handles updating patient data.
    UpdateAdmin &ua = UpdateAdmin::getInstance();          // Handles updating admin data.
    std::vector<WINDOW *> windows = {win_body, win_form};

    // If there are no records to interact with, exit early.
    if (db.listMatrixCurrent.empty())
        return;

    // Switch between different actions based on the selected column in the current row.
    switch (db.selectedCol)
    {
    case 1: // Profile action (View Profile)
        // Get the user ID from the selected record and fetch the corresponding user details.
        p.user = userManager.getUserById(db.listMatrixCurrent[db.selectedRow][db.listMatrixCurrent[db.selectedRow].size() - 1]);

        // Store the previous screen (database screen) for future navigation.
        p.prevScreen = Screen::Database;

        // Switch to the Profile screen for viewing detailed user information.
        navigationHandler(nullptr, nullptr, windows, Screen::Profile);
        break;

    case 2: // Update action (Edit Profile)
        // Handle updates for patient and admin records differently.
        if (db.currentFilter == Database::Filter::patient)
        {
            // Populate the update form for the selected patient user.
            up.user = userManager.getUserById(db.listMatrixCurrent[db.selectedRow][db.listMatrixCurrent[db.selectedRow].size() - 1]);
            up.populateValues(std::dynamic_pointer_cast<Patient>(up.user));

            // Switch to the screen for updating patient information.
            navigationHandler(nullptr, nullptr, windows, Screen::UpdateAccountPatientScreen);
        }
        else
        {
            // Populate the update form for the selected admin user.
            ua.user = userManager.getUserById(db.listMatrixCurrent[db.selectedRow][db.listMatrixCurrent[db.selectedRow].size() - 1]);
            ua.populateValues(std::dynamic_pointer_cast<Admin>(ua.user));

            // Switch to the screen for updating admin information.
            navigationHandler(nullptr, nullptr, windows, Screen::UpdateAdminScreen);
        }
        break;

    case 3: // Delete action (Delete User)
        // Get the ID of the selected user and delete them from the database.
        userManager.deleteUserById(db.listMatrixCurrent[db.selectedRow][db.listMatrixCurrent[db.selectedRow].size() - 1]);

        // Refresh the list of records after deletion to reflect the changes.
        if (db.currentFilter == Database::Filter::patient)
        {
            db.patientRecords = userManager.getPatients(db.searchQuery);
            db.generateListMatrixPatient(db.patientRecords);
        }
        else
        {
            db.adminRecords = userManager.getAdmins(db.searchQuery);
            db.generateListMatrixAdmin(db.adminRecords);
        }

        // Update the list displayed on the current page after the deletion.
        db.listMatrixCurrent = db.currentFilter == Database::Filter::patient ? db.getCurrentPagePatient() : db.getCurrentPageAdmin();

        // If the current page is empty after deletion, move to the previous page.
        while (db.listMatrixCurrent.empty() && db.currentPage > 0)
        {
            db.currentPage--;
            db.listMatrixCurrent = db.currentFilter == Database::Filter::patient ? db.getCurrentPagePatient() : db.getCurrentPageAdmin();
        }

        // Adjust the selected row to avoid it going out of bounds after deletion.
        if (db.selectedRow >= static_cast<int>(db.listMatrixCurrent.size()))
        {
            // Ensure that selectedRow stays within the range of the available records.
            db.selectedRow = std::max(0, static_cast<int>(db.listMatrixCurrent.size()) - 1);
        }
        break;

    default:
        break; // Default case if no action is taken.
    }
}

// Renders the database screen, displays records, handles search, and navigation.
void renderDatabaseScreen()
{
    Color &colorScheme = Color::getInstance();             // Get the color scheme for UI elements.
    UserManager &userManager = UserManager::getInstance(); // User manager to handle users and their records.
    Database &db = Database::getInstance();                // Database instance to manage and display records.

    renderHeader();      // Render the screen header.
    renderControlInfo(); // Render control information (help, instructions, etc.).

    bkgd(COLOR_PAIR(colorScheme.primary)); // Set the background color for the screen.

    // Define header for the screen.
    int baseline = 11;
    std::string header = "MedTek+ User Database";
    mvprintw(baseline, (COLS - header.length()) / 2, "%s", header.c_str()); // Display the header in the center.
    refresh();                                                              // Ensure header is displayed.

    // Set the size and position for the main window (win_body).
    int outer_height = 28;
    int outer_width = 82;
    int start_y = ((LINES - outer_height) / 2) + 5;
    int start_x = (COLS - outer_width) / 2;

    // Create and initialize win_body.
    WINDOW *win_body = newwin(outer_height, outer_width, start_y, start_x);
    wbkgd(win_body, COLOR_PAIR(colorScheme.primary)); // Set the background color for win_body.
    box(win_body, 0, 0);                              // Draw a box around win_body.
    wrefresh(win_body);                               // Refresh win_body.

    // Define inner window size (win_form) and create it inside win_body.
    int inner_height = outer_height - 4;
    int inner_width = outer_width - 4;
    WINDOW *win_form = derwin(win_body, inner_height, inner_width, 2, 2);
    wbkgd(win_form, COLOR_PAIR(colorScheme.primary)); // Set background for win_form.
    box(win_form, 0, 0);                              // Draw a box around win_form.
    wrefresh(win_form);                               // Refresh win_form.

    // Fetch patient and admin records based on the search query.
    db.patientRecords = userManager.getPatients(db.searchQuery);
    db.adminRecords = userManager.getAdmins(db.searchQuery);

    // Generate the list matrix for the current records.
    db.generateListMatrixPatient(db.patientRecords);
    db.generateListMatrixAdmin(db.adminRecords);

    // Set the list to display based on the current filter (patient/admin).
    db.listMatrixCurrent = db.currentFilter == Database::Filter::patient ? db.getCurrentPagePatient() : db.getCurrentPageAdmin();

    std::vector<WINDOW *> windows = {win_body, win_form};

    bool done = false;      // Flag to control the loop.
    keypad(win_form, TRUE); // Enable keypad input for win_form.
    curs_set(0);            // Hide cursor initially.
    int ch;

    while (!done)
    {
        // Render the subheader based on the current filter (Patient Records or Admin Records).
        werase(win_body);
        box(win_body, 0, 0);
        std::string subHeader = db.currentFilter == Database::Filter::patient ? db.subheaderArr[0] : db.subheaderArr[1];
        mvwprintw(win_body, 1, (outer_width - subHeader.length()) / 2, "%s", subHeader.c_str()); // Print subheader centered.
        wrefresh(win_body);

        // Clear and prepare the form window for rendering the content.
        werase(win_form);
        box(win_form, 0, 0);

        mvwhline(win_form, 2, 2, ACS_HLINE, inner_width - 4); // Draw a horizontal line under the header.

        // Render the search bar at the top.
        std::string searchText = "Search: " + db.searchQuery;
        mvwprintw(win_form, 1, 2, "%s", searchText.c_str());

        // Render the records list for the current page.
        if (db.listMatrixCurrent.empty())
        {
            std::string emptyText = "No records found.";
            mvwprintw(win_form, 4, (inner_width - emptyText.length()) / 2, "%s", emptyText.c_str()); // If no records, display a message.
        }
        else
        {
            // Loop through and display records in a matrix format.
            for (int i = 0; i < static_cast<int>(db.listMatrixCurrent.size()); i++)
            {
                int xPos = 2;
                int yPos = 3 + (i * 2);

                // Render each field of a record.
                for (int j = 0; j < static_cast<int>(db.listMatrixCurrent[i].size() - 1); j++)
                {
                    if (j == 1) // Adjust spacing for buttons.
                        xPos += 5;

                    // Highlight the selected record.
                    if (i == db.selectedRow && j == db.selectedCol)
                        wattron(win_form, A_REVERSE);

                    mvwprintw(win_form, yPos, xPos, "%s", db.listMatrixCurrent[i][j].c_str());

                    // Remove highlighting after the selected item is printed.
                    if (i == db.selectedRow && j == db.selectedCol)
                        wattroff(win_form, A_REVERSE);

                    xPos += inner_width / db.listMatrixCurrent[i].size(); // Adjust spacing between columns.
                }
            }
        }

        // Show cursor in the search bar if no row is selected, else hide it.
        if (db.selectedRow == -1)
        {
            curs_set(1);                                 // Show cursor.
            wmove(win_form, 1, 2 + searchText.length()); // Move cursor to the end of the search text.
        }
        else
        {
            curs_set(0); // Hide cursor.
        }

        wrefresh(win_form); // Refresh the form window.

        ch = wgetch(win_form); // Get user input.

        if (db.selectedRow == -1) // If no row is selected, handle the search input.
        {
            if (ch == KEY_BACKSPACE || ch == 127) // Handle backspace for search query.
            {
                if (!db.searchQuery.empty())
                    db.searchQuery.pop_back();
            }
            else if (ch >= 32 && ch <= 126) // Allow printable characters.
            {
                db.currentPage = 0;                      // Reset to the first page.
                db.searchQuery += static_cast<char>(ch); // Add the character to the search query.
            }

            // Regenerate list based on the updated search query.
            if (db.currentFilter == Database::Filter::patient)
            {
                db.patientRecords = userManager.getPatients(db.searchQuery);
                db.generateListMatrixPatient(db.patientRecords);
                db.listMatrixCurrent = db.getCurrentPagePatient();
            }
            else
            {
                db.adminRecords = userManager.getAdmins(db.searchQuery);
                db.generateListMatrixAdmin(db.adminRecords);
                db.listMatrixCurrent = db.getCurrentPageAdmin();
            }
        }

        // Handle input for navigation and switching filters.
        switch (ch)
        {
        case '\n': // If Enter is pressed, handle the selected record.
            handleDatabaseControls(win_form, win_body);
            break;
        case '+': // Exit loop if '+' is pressed.
            done = true;
            break;
        case KEY_DOWN: // Move selection down.
            if (db.selectedRow < static_cast<int>(db.listMatrixCurrent.size() - 1))
                db.selectedRow++;
            break;
        case KEY_UP: // Move selection up.
            if (db.selectedRow > -1)
                db.selectedRow--;
            break;
        case KEY_LEFT: // Move left across the columns.
            if (db.selectedRow > -1 && db.selectedCol > 1)
                db.selectedCol--;
            break;
        case KEY_RIGHT: // Move right across the columns.
            if (db.selectedRow > -1 && db.selectedCol < static_cast<int>(db.listMatrixCurrent[db.selectedRow].size() - 2))
                db.selectedCol++;
            break;
        case 9: // If Tab is pressed, switch between patient/admin filter.
            db.currentFilter = (db.currentFilter == Database::Filter::patient) ? Database::Filter::admin : Database::Filter::patient;
            db.currentPage = 0;     // Reset page when switching filters.
            db.searchQuery.clear(); // Clear the search query.
            db.patientRecords = userManager.getPatients("");
            db.adminRecords = userManager.getAdmins("");
            db.generateListMatrixPatient(db.patientRecords);
            db.generateListMatrixAdmin(db.adminRecords);
            db.listMatrixCurrent = db.currentFilter == Database::Filter::patient ? db.getCurrentPagePatient() : db.getCurrentPageAdmin();
            db.selectedRow = -1; // No row selected initially.
            db.selectedCol = 1;  // Default column selection.
            break;
        case KEY_PPAGE: // Handle page up action.
            if (db.currentPage > 0)
            {
                db.currentPage--;
                db.listMatrixCurrent = db.currentFilter == Database::Filter::patient ? db.getCurrentPagePatient() : db.getCurrentPageAdmin();
            }
            break;
        case KEY_NPAGE: // Handle page down action.
            if (db.currentFilter == Database::Filter::patient && db.currentPage + 1 < db.totalPagesPatient)
            {
                db.currentPage++;
                db.listMatrixCurrent = db.getCurrentPagePatient();
            }
            else if (db.currentFilter == Database::Filter::admin && db.currentPage + 1 < db.totalPagesAdmin)
            {
                db.currentPage++;
                db.listMatrixCurrent = db.getCurrentPageAdmin();
            }
            break;
        case 27: // If ESC is pressed, exit.
            exitHandler(nullptr, nullptr, windows);
            done = true;
            break;
        case 2: // Custom key for "Back" action.
            done = true;
            break;
        }
    }

    // Switch to the appropriate screen based on the action.
    Screen screen = db.currentFilter == Database::Filter::patient ? Screen::RegistrationAccountPatientScreen : Screen::RegisterAdmin;
    switch (ch)
    {
    case '+':
        navigationHandler(nullptr, nullptr, windows, screen);
        break;
    default:
        db.reset();
        navigationHandler(nullptr, nullptr, windows, Screen::Dashboard);
        break;
    }
}

void renderProfileAdmissionsScreen()
{
    // Get instances for managing colors, events, and profile data
    Color &colorScheme = Color::getInstance();
    Profile &p = Profile::getInstance();

    // Render the header and control info at the top of the screen
    renderHeader();
    renderControlInfo();

    // Get the current patient object
    auto patient = std::dynamic_pointer_cast<Patient>(p.user);

    // Set the header text for the admissions screen
    std::string header = "Viewing " + patient->fullName + "'s admissions";
    attron(A_BOLD); // Make the header bold
    mvprintw(11, (COLS - header.length()) / 2, "%s", header.c_str());
    attroff(A_BOLD); // End bold text
    refresh();

    // Define the dimensions and starting position of the outer and inner windows
    int outer_height = 28;
    int outer_width = 82;
    int start_y = ((LINES - outer_height) / 2) + 5;
    int start_x = (COLS - outer_width) / 2;

    // Create an outer window for the body of the screen and set its background color
    WINDOW *win_body = newwin(outer_height, outer_width, start_y, start_x);
    wbkgd(win_body, COLOR_PAIR(colorScheme.primary));
    box(win_body, 0, 0); // Draw a border around the window
    wrefresh(win_body);

    // Define the dimensions for the inner window (form)
    int inner_height = outer_height - 4;
    int inner_width = outer_width - 4;

    // Create the inner window (form) and set its background color
    WINDOW *win_form = derwin(win_body, inner_height, inner_width, 2, 2);
    wbkgd(win_form, COLOR_PAIR(colorScheme.primary));
    box(win_form, 0, 0); // Draw a border for the inner window
    wrefresh(win_form);

    bool done = false;      // Flag to control the loop
    keypad(win_form, TRUE); // Enable special keys (e.g., arrow keys, Enter, etc.)
    curs_set(0);            // Hide the cursor

    // Generate the list of admissions for the patient based on the current search query
    p.generateListMatrix(p.search(p.searchQuery, patient->admissions));
    p.listMatrix = p.getCurrentPage();

    std::vector<WINDOW *> windows = {win_body, win_form};

    int ch; // Variable to store the user input

    while (!done)
    {
        // Clear the body window and redraw the border
        werase(win_body);
        box(win_body, 0, 0);

        // Render the subheader (e.g., "Patient Records")
        std::string subHeader = p.patientSubheaderArr[1];
        mvwprintw(win_body, 1, (outer_width - subHeader.length()) / 2, "%s", subHeader.c_str());
        wrefresh(win_body);

        // Clear the form window and redraw the border
        werase(win_form);
        box(win_form, 0, 0);

        // Draw a horizontal line for separation
        mvwhline(win_form, 2, 2, ACS_HLINE, inner_width - 4);

        // Render the search bar at the top of the form window
        std::string searchText = "Search: " + p.searchQuery;
        mvwprintw(win_form, 1, 2, "%s", searchText.c_str());

        // Render the list of admissions for the current page
        if (p.listMatrix.empty())
        {
            std::string emptyText = "No records found.";
            mvwprintw(win_form, 4, (inner_width - emptyText.length()) / 2, "%s", emptyText.c_str());
        }
        else
        {
            for (int i = 0; i < static_cast<int>(p.listMatrix.size()); i++)
            {
                int xPos = 2;
                int yPos = 3 + (i * 2);

                for (int j = 0; j < static_cast<int>(p.listMatrix[i].size()); j++)
                {
                    if (j == 2)
                    {
                        xPos += 5; // Adjust position for the third column
                    }

                    // Highlight the selected item
                    if (i == p.selectedRow && j == p.selectedCol)
                        wattron(win_form, A_REVERSE);

                    mvwprintw(win_form, yPos, xPos, "%s", p.listMatrix[i][j].c_str());

                    // Remove highlight after rendering
                    if (i == p.selectedRow && j == p.selectedCol)
                        wattroff(win_form, A_REVERSE);

                    xPos += inner_width / p.listMatrix[i].size();
                }
            }
        }

        // If no row is selected, show the cursor in the search bar
        if (p.selectedRow == -1)
        {
            curs_set(1);                                 // Show the cursor
            wmove(win_form, 1, 2 + searchText.length()); // Move cursor to the end of the search query
        }
        else
        {
            curs_set(0); // Hide the cursor
        }

        wrefresh(win_form);

        // Get the user input (key press)
        ch = wgetch(win_form);

        // Handle input for search query (backspace and character input)
        if (p.selectedRow == -1)
        {
            if (ch == KEY_BACKSPACE || ch == 127) // Handle backspace
            {
                if (!p.searchQuery.empty())
                    p.searchQuery.pop_back();
            }
            else if (ch >= 32 && ch <= 126) // Handle printable characters
            {
                p.currentPage = 0;                      // Reset to the first page
                p.searchQuery += static_cast<char>(ch); // Add the character to the search query
            }
            // Regenerate the list based on the updated search query
            p.generateListMatrix(p.search(p.searchQuery, patient->admissions));
            p.listMatrix = p.getCurrentPage();
        }

        // Handle input for navigation and actions (enter, arrow keys, etc.)
        switch (ch)
        {
        case '+': // Exit the loop and return to the previous screen
            done = true;
            break;
        case '\n': // Enter key to delete an admission
            if (p.listMatrix.empty())
                break;
            // Delete the selected admission
            patient->deleteAdmission(Admissions::stringToDepartment(p.listMatrix[p.selectedRow][0]), p.listMatrix[p.selectedRow][1]);
            // Regenerate the list after deletion
            p.generateListMatrix(p.search(p.searchQuery, patient->admissions));
            p.listMatrix = p.getCurrentPage();
            while (p.listMatrix.empty() && p.currentPage > 0)
            {
                p.currentPage--;
                p.listMatrix = p.getCurrentPage();
            }
            // Adjust selected row if necessary
            if (p.selectedRow >= static_cast<int>(p.listMatrix.size()))
            {
                p.selectedRow = std::max(0, static_cast<int>(p.listMatrix.size()) - 1);
            }
            break;
        case KEY_DOWN: // Down arrow key for navigation
            if (p.selectedRow < static_cast<int>(p.listMatrix.size() - 1))
                p.selectedRow++;
            break;
        case KEY_UP: // Up arrow key for navigation
            if (p.selectedRow > -1)
                p.selectedRow--;
            break;
        case 9: // Tab key (custom action)
            done = true;
            break;
        case KEY_PPAGE: // Page Up key
            if (p.currentPage > 0)
            {
                p.currentPage--;
                p.generateListMatrix(p.search(p.searchQuery, patient->admissions));
                p.listMatrix = p.getCurrentPage();
            }
            break;
        case KEY_NPAGE: // Page Down key
            if (p.currentPage + 1 < p.totalPages)
            {
                p.currentPage++;
                p.generateListMatrix(p.search(p.searchQuery, patient->admissions));
                p.listMatrix = p.getCurrentPage();
            }
            break;
        case 27: // ESC to exit
            exitHandler(nullptr, nullptr, windows);
            done = true;
            break;
        case 2: // Custom key for "Back"
            done = true;
            break;
        }
    }

    // Reset profile data and handle screen transition
    p.reset();
    if (ch == '+')
    {
        Admission &a = Admission::getInstance();
        a.prevScreen = Screen::ProfileAdmissions;
        navigationHandler(nullptr, nullptr, windows, Screen::Admission);
    }
    else
    {
        navigationHandler(nullptr, nullptr, windows, Screen::Profile);
    }
}

void renderProfileScreen()
{
    // Accessing singleton instances of Color, EventManager, and Profile
    Color &colorScheme = Color::getInstance();
    Profile &p = Profile::getInstance();

    // Render the header and control information (e.g., navigation tips)
    renderHeader();
    renderControlInfo();

    // Set background color based on the primary color scheme
    bkgd(COLOR_PAIR(colorScheme.primary));
    int ch;

    // Check if the current user is a Patient or an Admin
    if (p.user->getRole() == Role::Patient)
    {
        // Cast the user to a Patient type
        auto patient = std::dynamic_pointer_cast<Patient>(p.user);
        int baseline = 11;

        // Display patient header with their full name
        attron(A_BOLD);
        std::string header = "Viewing " + patient->fullName + "'s account";
        mvprintw(baseline, (COLS - header.length()) / 2, "%s", header.c_str());
        attroff(A_BOLD);

        // Define window dimensions and positions
        int outer_height = 26;
        int outer_width = 60;
        int start_y = ((LINES - outer_height) / 2) + 4;
        int start_x = (COLS - outer_width) / 2;

        // Create window for the profile information
        WINDOW *win_body = newwin(outer_height, outer_width, start_y, start_x);
        wbkgd(win_body, COLOR_PAIR(colorScheme.primary));
        box(win_body, 0, 0);

        // Render subheader (e.g., "Patient Information")
        mvwprintw(win_body, 1, (outer_width - p.patientSubheaderArr[0].length()) / 2, "%s", p.patientSubheaderArr[0].c_str());

        // Define inner window dimensions
        int inner_height = outer_height - 4;
        int inner_width = outer_width - 4;

        // Create inner window for displaying field values
        WINDOW *win_form = derwin(win_body, inner_height, inner_width, 2, 2);
        wbkgd(win_form, COLOR_PAIR(colorScheme.primary));
        box(win_form, 0, 0);

        // Populate the patient's field values (ID, Age, Email, etc.)
        p.fieldValues = {
            {"ID: ", patient->getId()},
            {"Created at: ", patient->getCreatedAt()},
            {"Full Name: ", patient->getFullName()},
            {"Username: ", patient->getUsername()},
            {"Age: ", std::to_string(patient->age)},
            {"Gender: ", patient->gender},
            {"Marital Status: ", patient->maritalStatus},
            {"Religion: ", patient->religion},
            {"Nationality: ", patient->nationality},
            {"Race: ", patient->race},
            {"IC Number: ", patient->identityCardNumber},
            {"Email: ", patient->email},
            {"Password: ", patient->password},
            {"Contact No.: ", patient->contactNumber},
            {"Emergency Contact Name: ", patient->emergencyContactName},
            {"Emergency Contact No.: ", patient->emergencyContactNumber},
            {"BMI: ", std::to_string(patient->bmi)},
            {"Height: ", patient->height},
            {"Weight: ", patient->weight},
            {"Address: ", patient->address},
        };

        // Loop through the field values and print them on the form
        int row = 1;
        for (const auto &field : p.fieldValues)
        {
            mvwprintw(win_form, row, 2, "%s%s", field.first.c_str(), field.second.c_str());
            row++;
        }

        // Refresh the form and body windows
        wrefresh(win_form);
        wrefresh(win_body);

        std::vector<WINDOW *> windows = {win_body, win_form};

        // Enter the input loop
        while (true)
        {
            ch = getch(); // Get user input

            switch (ch)
            {
            case 2: // Handle 'Back' key (Ctrl+B)
                p.reset();
                // Navigate back to the previous screen
                navigationHandler(nullptr, nullptr, windows, p.prevScreen);
                return;
            case 27: // Handle ESC key
                // Exit the program or screen
                exitHandler(nullptr, nullptr, windows);
                return;
            case 9: // Handle TAB key
                // Clear current window and navigate to the profile admissions screen
                navigationHandler(nullptr, nullptr, windows, Screen::ProfileAdmissions);
                return;
            }
        }
    }
    else
    {
        // If the user is an admin, follow similar steps for rendering the admin profile
        auto admin = std::dynamic_pointer_cast<Admin>(p.user);

        int baseline = 11;
        std::string header = "Viewing " + admin->fullName + "'s account";
        mvprintw(baseline, (COLS - header.length()) / 2, "%s", header.c_str());

        // Set background color for the screen
        bkgd(COLOR_PAIR(colorScheme.primary));

        // Define window dimensions for admin profile
        int outer_height = 16;
        int outer_width = 60;
        int start_y = ((LINES - outer_height) / 2) + 2;
        int start_x = (COLS - outer_width) / 2;

        // Create window for the profile information
        WINDOW *win_body = newwin(outer_height, outer_width, start_y, start_x);
        wbkgd(win_body, COLOR_PAIR(colorScheme.primary));
        box(win_body, 0, 0);

        // Render subheader for admin profile (e.g., "Account Information")
        std::string subHeader = "Account Information";
        mvwprintw(win_body, 1, (outer_width - subHeader.length()) / 2, "%s", subHeader.c_str());

        // Define inner window dimensions for field values
        int inner_height = outer_height - 4;
        int inner_width = outer_width - 4;

        // Create inner window for displaying admin details
        WINDOW *win_form = derwin(win_body, inner_height, inner_width, 2, 2);
        wbkgd(win_form, COLOR_PAIR(colorScheme.primary));
        box(win_form, 0, 0);

        // Populate the admin's field values (ID, Username, Email, etc.)
        p.fieldValues = {
            {"ID: ", admin->getId()},
            {"Created at: ", admin->getCreatedAt()},
            {"Full Name: ", admin->getFullName()},
            {"Username: ", admin->getUsername()},
            {"Email: ", admin->email},
            {"Password: ", admin->password},
            {"Contact No.: ", admin->contactNumber}};

        // Loop through the admin field values and print them on the form
        int row = 1;
        for (const auto &field : p.fieldValues)
        {
            mvwprintw(win_form, row, 2, "%s%s", field.first.c_str(), field.second.c_str());
            row++;
        }

        // Refresh the form and body windows
        wrefresh(win_form);
        wrefresh(win_body);

        std::vector<WINDOW *> windows = {win_body, win_form};

        // Enter the input loop for admin profile
        while (true)
        {
            ch = getch(); // Get user input

            if (ch == 2) // Handle 'Back' key (Ctrl+B)
            {
                p.reset();
                // Navigate back to the previous screen
                navigationHandler(nullptr, nullptr, windows, p.prevScreen);
                break;
            }
            else if (ch == 27) // Handle ESC key
            {
                // Exit the program or screen
                exitHandler(nullptr, nullptr, windows);
            }
            else
            {
                continue;
            }
        }
    }
}

void renderAdmissionScreen()
{
    // Accessing singleton instances of Color, EventManager, and Admission
    Color &colorScheme = Color::getInstance();
    Admission &a = Admission::getInstance();

    // Render the header and control information (e.g., navigation tips)
    renderHeader();
    renderControlInfo();

    // Display the title "Admission" at the center of the screen
    std::string header = "Admission";
    attron(A_BOLD);
    mvprintw(11, (COLS - header.length()) / 2, "%s", header.c_str());
    attroff(A_BOLD);
    refresh();

    // Define window dimensions and positions for the main body
    int outer_height = 28;
    int outer_width = 82;
    int start_y = ((LINES - outer_height) / 2) + 5;
    int start_x = (COLS - outer_width) / 2;

    // Create the window for the main body and set background color
    WINDOW *win_body = newwin(outer_height, outer_width, start_y, start_x);
    wbkgd(win_body, COLOR_PAIR(colorScheme.primary));
    box(win_body, 0, 0);
    wrefresh(win_body);

    // Define inner window dimensions for the form
    int inner_height = outer_height - 4;
    int inner_width = outer_width - 4;

    // Create the inner window for user interaction (form window)
    WINDOW *win_form = derwin(win_body, inner_height, inner_width, 2, 2);
    wbkgd(win_form, COLOR_PAIR(colorScheme.primary));
    box(win_form, 0, 0);
    wrefresh(win_form);

    // Flag to control the main loop
    bool done = false;

    // Enable keypad input and hide the cursor
    keypad(win_form, TRUE);
    curs_set(0);
    int ch;

    // Initialize the list of departments (search query and pagination)
    a.generateList(a.search(a.searchQuery));
    a.list = a.getCurrentPage();

    std::vector<WINDOW *> windows = {win_body, win_form};

    // Main loop for rendering the admission screen
    while (!done)
    {
        // Clear and redraw the main body window with a new subheader
        werase(win_body);
        box(win_body, 0, 0);
        std::string subHeader = "Select a department";
        mvwprintw(win_body, 1, (outer_width - subHeader.length()) / 2, "%s", subHeader.c_str());
        wrefresh(win_body);

        // Clear and redraw the form window
        werase(win_form);
        box(win_form, 0, 0);

        // Draw horizontal line for search bar separation
        mvwhline(win_form, 2, 2, ACS_HLINE, inner_width - 4);

        // Display search query in the search bar
        std::string searchText = "Search: " + a.searchQuery;
        mvwprintw(win_form, 1, 2, "%s", searchText.c_str());

        // Render the department list or show "No records found" if the list is empty
        if (a.list.empty())
        {
            std::string emptyText = "No records found.";
            mvwprintw(win_form, 4, (inner_width - emptyText.length()) / 2, "%s", emptyText.c_str());
        }
        else
        {
            // Render each department in the list with selectable rows
            for (int i = 0; i < static_cast<int>(a.list.size()); i++)
            {
                int yPos = 3 + (i * 2);
                int xPos = 2;

                // Highlight the selected row with reverse video
                if (i == a.selectedRow)
                    wattron(win_form, A_REVERSE);

                mvwprintw(win_form, yPos, xPos, "%s", a.list[i].second.c_str());

                if (i == a.selectedRow)
                    wattroff(win_form, A_REVERSE);
            }
        }

        // Manage cursor visibility and position based on selection
        if (a.selectedRow == -1)
        {
            curs_set(1);                                 // Show cursor
            wmove(win_form, 1, 2 + searchText.length()); // Move cursor to the end of the search text
        }
        else
        {
            curs_set(0); // Hide cursor when a row is selected
        }

        // Refresh the form window to reflect updates
        wrefresh(win_form);

        // Capture user input
        ch = wgetch(win_form);

        // Handle input for search query updates (backspace and character entry)
        if (a.selectedRow == -1)
        {
            if (ch == KEY_BACKSPACE || ch == 127) // Handle backspace
            {
                if (!a.searchQuery.empty())
                    a.searchQuery.pop_back();
            }
            else if (ch >= 32 && ch <= 126) // Allow printable characters for search query
            {
                a.currentPage = 0;
                a.searchQuery += static_cast<char>(ch);
            }
            // Regenerate list dynamically based on updated search query
            a.generateList(a.search(a.searchQuery));
            a.list = a.getCurrentPage();
        }

        // Handle input for navigation and interaction with the list
        switch (ch)
        {
        case '\n': // Handle "Enter" key (select a department)
            if (a.selectedRow > -1 && a.selectedRow < static_cast<int>(a.list.size()))
            {
                // Set the selected department and exit loop
                a.selectedDepartment = a.list[a.selectedRow].first;
                done = true;
            }
            break;
        case KEY_DOWN: // Handle "Down" key (move selection down)
            if (a.selectedRow < static_cast<int>(a.list.size() - 1))
                a.selectedRow++;
            break;
        case KEY_UP: // Handle "Up" key (move selection up)
            if (a.selectedRow > -1)
                a.selectedRow--;
            break;
        case 9: // Handle "Tab" key (exit loop)
            done = true;
            break;
        case KEY_PPAGE: // Handle "Page Up" key (navigate up a page)
            if (a.currentPage > 0)
            {
                a.currentPage--;
                a.generateList(a.search(a.searchQuery)); // Ensure list is refreshed
                a.list = a.getCurrentPage();
            }
            break;
        case KEY_NPAGE: // Handle "Page Down" key (navigate down a page)
            if (a.currentPage + 1 < a.totalPages)
            {
                a.currentPage++;
                a.generateList(a.search(a.searchQuery)); // Ensure list is refreshed
                a.list = a.getCurrentPage();
            }
            break;
        case 27: // Handle "ESC" key to exit
            exitHandler(nullptr, nullptr, windows);
            done = true;
            break;
        case 2: // Custom key for "Back"
            done = true;
            break;
        }
    }

    // If we are returning to the Profile screen, add the selected department to the patient's admissions
    if (a.prevScreen == Screen::ProfileAdmissions)
    {
        Profile &p = Profile::getInstance();
        auto patient = std::dynamic_pointer_cast<Patient>(p.user);
        patient->addAdmission(a.selectedDepartment);
    }

    // Reset the admission state and navigate back to the previous screen
    a.reset();
    navigationHandler(nullptr, nullptr, windows, a.prevScreen);
}

void renderUpdateAccountPatientScreen()
{
    // Render the header and control information (e.g., navigation tips)
    renderHeader();
    renderControlInfo();

    // Access singleton instances of Color and UpdatePatient
    Color &colorScheme = Color::getInstance();
    UpdatePatient &u = UpdatePatient::getInstance();

    // Access the patient object (casting from base User class)
    auto patient = std::dynamic_pointer_cast<Patient>(u.user);

    // Display the screen header with patient's full name
    int baseline = 11;
    std::string header = "Updating " + patient->fullName + "'s account";
    mvprintw(baseline, (COLS - header.length()) / 2, "%s", header.c_str());

    // Set background color for the screen
    bkgd(COLOR_PAIR(colorScheme.primary));

    // Define window size and position for the form
    int outer_height = 16;
    int outer_width = 60;
    int start_y = ((LINES - outer_height) / 2) + 2;
    int start_x = (COLS - outer_width) / 2;

    // Create the window for the form body and set background color
    WINDOW *win_body = newwin(outer_height, outer_width, start_y, start_x);
    wbkgd(win_body, COLOR_PAIR(colorScheme.primary));
    box(win_body, 0, 0);

    // Render the subheader in the form window (Account Information)
    std::string subHeader = "Account Information";
    mvwprintw(win_body, 1, (outer_width - subHeader.length()) / 2, "%s", subHeader.c_str());

    // Define the inner dimensions for the editable form
    int inner_height = outer_height - 4;
    int inner_width = outer_width - 4;

    // Create the inner form window for user input
    WINDOW *win_form = derwin(win_body, inner_height, inner_width, 2, 2);
    wbkgd(win_form, COLOR_PAIR(colorScheme.primary));
    box(win_form, 0, 0);

    // Define and initialize form fields (labels and input fields)
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
    fields[10] = NULL;                         // End of fields array

    // Ensure all fields are created successfully
    assert(fields[0] && fields[1] && fields[2] && fields[3] && fields[4] &&
           fields[5] && fields[6] && fields[7] && fields[8] && fields[9]);

    // Set field values (from UpdatePatient instance)
    set_field_buffer(fields[0], 0, "Username:");
    set_field_buffer(fields[1], 0, u.fieldValues["username"].second.c_str());
    set_field_buffer(fields[2], 0, "Password:");
    set_field_buffer(fields[3], 0, u.fieldValues["password"].second.c_str());
    set_field_buffer(fields[4], 0, "Email:");
    set_field_buffer(fields[5], 0, u.fieldValues["email"].second.c_str());
    set_field_buffer(fields[6], 0, "Address:");
    set_field_buffer(fields[7], 0, u.fieldValues["address"].second.c_str());
    set_field_buffer(fields[8], 0, "Contact Number:");
    set_field_buffer(fields[9], 0, u.fieldValues["contactNumber"].second.c_str());

    // Set field options (visibility, editability, etc.)
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
        set_field_back(fields[i], COLOR_PAIR(colorScheme.primary)); // Set background color for each field
    }

    // Create the form and attach it to the form window
    FORM *form = new_form(fields);
    assert(form);
    set_form_win(form, win_form);
    set_form_sub(form, derwin(win_form, inner_height - 2, inner_width - 2, 1, 1));
    post_form(form);

    // Refresh the windows to display the form
    wrefresh(win_body);
    wrefresh(win_form);

    // Create an error window for form validation feedback
    int error_win_height = 1;
    int error_win_width = 40;
    int error_win_y = LINES - 2;
    int error_win_x = (COLS - error_win_width) / 2;
    WINDOW *error_win = newwin(error_win_height, error_win_width, error_win_y, error_win_x);
    wbkgd(error_win, COLOR_PAIR(colorScheme.danger));

    std::vector<WINDOW *> windows = {win_body, win_form, error_win};

    // Input loop for form submission
    bool done = false;
    int ch;

    while (!done)
    {
        // Wait for user input until Enter key is pressed
        while ((ch = getch()) != '\n')
        {
            driver_form(
                ch,
                form,
                fields,
                win_form,
                win_body,
                [&]()
                { exitHandler(form, fields, windows); },
                [&]()
                {
                    u.reset();
                    navigationHandler(form, fields, windows, Screen::Database);
                });
        }

        // Validate the form data when Enter key is pressed
        form_driver(form, REQ_VALIDATION);

        std::string errorMessage;

        // Validate required fields
        if (!validateFields(fields))
        {
            errorMessage = "Please fill all required fields";
        }
        else if (!validateEmail(trim_whitespaces(field_buffer(fields[5], 0))))
        {
            errorMessage = "Please enter a valid email";
        }
        else if (!validateContactNumber(trim_whitespaces(field_buffer(fields[9], 0))))
        {
            errorMessage = "Please enter a valid phone no.";
        }
        // If there's an error, display it
        if (!errorMessage.empty())
        {
            mvwprintw(error_win, 0, 0, "%s", errorMessage.c_str());
            wrefresh(error_win);
            std::this_thread::sleep_for(std::chrono::seconds(2)); // Show error for 2 seconds
            werase(error_win);
            wrefresh(error_win); // Clear error window
        }
        else
        {
            done = true; // Proceed if validation succeeds
        }
    }

    // Final validation to ensure fields are valid before processing
    form_driver(form, REQ_VALIDATION);

    // Extract the form data and update the field values
    u.fieldValues["username"].second = trim_whitespaces(field_buffer(fields[1], 0));
    u.fieldValues["password"].second = trim_whitespaces(field_buffer(fields[3], 0));
    u.fieldValues["email"].second = trim_whitespaces(field_buffer(fields[5], 0));
    u.fieldValues["address"].second = trim_whitespaces(field_buffer(fields[7], 0));
    u.fieldValues["contactNumber"].second = trim_whitespaces(field_buffer(fields[9], 0));

    navigationHandler(form, fields, windows, Screen::UpdatePersonalPatientScreen);
}

void renderUpdatePersonalPatientScreen()
{
    // Render the header and control information
    renderHeader();
    renderControlInfo();

    // Get the color scheme instance for styling
    Color &colorScheme = Color::getInstance();

    // Accessing the UpdatePatient instance for patient data
    UpdatePatient &u = UpdatePatient::getInstance();

    // Casting user data to Patient type
    auto patient = std::dynamic_pointer_cast<Patient>(u.user);

    // Define the position for the header and render it at the top of the screen
    int baseline = 11;
    std::string header = "Updating " + patient->fullName + "'s account";
    mvprintw(baseline, (COLS - header.length()) / 2, "%s", header.c_str());

    // Set the background color for the whole screen
    bkgd(COLOR_PAIR(colorScheme.primary));

    // Define the outer window size and position
    int outer_height = 20;
    int outer_width = 60;
    int start_y = ((LINES - outer_height) / 2) + 4;
    int start_x = (COLS - outer_width) / 2;

    // Create an outer window (bordered container for content)
    WINDOW *win_body = newwin(outer_height, outer_width, start_y, start_x);
    wbkgd(win_body, COLOR_PAIR(colorScheme.primary)); // Set background color for window
    box(win_body, 0, 0);                              // Add a border around the window

    // Add the sub-header for the personal information section
    std::string subHeader = "Personal Information";
    mvwprintw(win_body, 1, (outer_width - subHeader.length()) / 2, "%s", subHeader.c_str());

    // Define the inner window size and position (form content area)
    int inner_height = outer_height - 4;
    int inner_width = outer_width - 4;

    // Create a new window inside the outer window to display the form
    WINDOW *win_form = derwin(win_body, inner_height, inner_width, 2, 2);
    wbkgd(win_form, COLOR_PAIR(colorScheme.primary)); // Set background color for form window
    box(win_form, 0, 0);                              // Add a border around the form window

    // Define the fields for the form (each label and input field)
    FIELD *fields[13];
    fields[0] = new_field(1, 15, 0, 1, 0, 0);    // Label: fullName
    fields[1] = new_field(1, 35, 0, 18, 0, 0);   // Input: fullName
    fields[2] = new_field(1, 32, 2, 1, 0, 0);    // Label: identityCardNumber
    fields[3] = new_field(1, 18, 2, 34, 0, 0);   // Input: identityCardNumber
    fields[4] = new_field(1, 15, 4, 1, 0, 0);    // Label: height
    fields[5] = new_field(1, 35, 4, 18, 0, 0);   // Input: height
    fields[6] = new_field(1, 15, 6, 1, 0, 0);    // Label: weight
    fields[7] = new_field(1, 35, 6, 18, 0, 0);   // Input: weight
    fields[8] = new_field(1, 30, 8, 1, 0, 0);    // Label: emergencyContactNumber
    fields[9] = new_field(1, 22, 8, 28, 0, 0);   // Input: emergencyContactNumber
    fields[10] = new_field(1, 28, 10, 1, 0, 0);  // Label: emergencyContactName
    fields[11] = new_field(1, 22, 10, 28, 0, 0); // Input: emergencyContactName
    fields[12] = NULL;                           // End of field array

    // Ensure all fields are created successfully
    assert(fields[0] && fields[1] && fields[2] && fields[3] && fields[4] &&
           fields[5] && fields[6] && fields[7] && fields[8] && fields[9] && fields[10] && fields[11]);

    // Set initial values for the input fields (from the registration instance)
    set_field_buffer(fields[0], 0, "Full Name:");
    set_field_buffer(fields[1], 0, u.fieldValues["fullName"].second.c_str());
    set_field_buffer(fields[2], 0, "IC number (e.g. 010403141107):");
    set_field_buffer(fields[3], 0, u.fieldValues["identityCardNumber"].second.c_str());
    set_field_buffer(fields[4], 0, "Height (cm):");
    set_field_buffer(fields[5], 0, u.fieldValues["height"].second.c_str());
    set_field_buffer(fields[6], 0, "Weight (kg):");
    set_field_buffer(fields[7], 0, u.fieldValues["weight"].second.c_str());
    set_field_buffer(fields[8], 0, "Emergency Contact Number:");
    set_field_buffer(fields[9], 0, u.fieldValues["emergencyContactNumber"].second.c_str());
    set_field_buffer(fields[10], 0, "Emergency Contact Name:");
    set_field_buffer(fields[11], 0, u.fieldValues["emergencyContactName"].second.c_str());

    // Set field options: defining which fields are editable or static
    for (int i = 0; fields[i]; i++)
    {
        if (i % 2 == 0) // Labels are static, non-editable
        {
            set_field_opts(fields[i], O_VISIBLE | O_PUBLIC | O_STATIC); // Read-only
        }
        else // Input fields are editable
        {
            set_field_opts(fields[i], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE | O_AUTOSKIP);
        }
        set_field_back(fields[i], COLOR_PAIR(colorScheme.primary)); // Set background color for fields
    }

    // Create and post the form to the window
    FORM *form = new_form(fields);
    assert(form);
    set_form_win(form, win_form);                                                  // Assign window to the form
    set_form_sub(form, derwin(win_form, inner_height - 2, inner_width - 2, 1, 1)); // Assign sub-window for form
    post_form(form);                                                               // Display the form on the screen

    // Refresh both the outer window and the form window to render them
    wrefresh(win_body);
    wrefresh(win_form);

    // Create a dedicated window to show error messages
    int error_win_height = 1;
    int error_win_width = 40;
    int error_win_y = LINES - 2;
    int error_win_x = (COLS - error_win_width) / 2;
    WINDOW *error_win = newwin(error_win_height, error_win_width, error_win_y, error_win_x);
    wbkgd(error_win, COLOR_PAIR(colorScheme.danger)); // Set background color for error messages

    std::vector<WINDOW *> windows = {win_body, win_form, error_win};

    // Input handling loop: validate input until all fields are filled
    bool done = false;
    int ch;

    while (!done)
    {
        while ((ch = getch()) != '\n')
        {
            driver_form(
                ch,       // Capture key press
                form,     // Pass the form for updating
                fields,   // List of fields
                win_form, // Window where form is displayed
                win_body, // Outer window
                [&]()
                { exitHandler(form, fields, windows); }, // Exit handler
                [&]()
                {
                    navigationHandler(form, fields, windows, Screen::UpdateAccountPatientScreen);
                }); // Navigation handler
        }
        form_driver(form, REQ_VALIDATION); // Validate the form on pressing Enter

        std::string errorMessage;

        // Validate required fields
        if (!validateFields(fields))
        {
            errorMessage = "Please fill all required fields";
        }
        else if (!validateIdentityCardNumber(trim_whitespaces(field_buffer(fields[3], 0))))
        {
            errorMessage = "Please enter a valid IC number";
        }
        else if (!validateHeightAndWeight(trim_whitespaces(field_buffer(fields[5], 0)), trim_whitespaces(field_buffer(fields[7], 0))))
        {
            errorMessage = "Please enter a valid height/weight";
        }
        else if (!validateContactNumber(trim_whitespaces(field_buffer(fields[9], 0))))
        {
            errorMessage = "Please enter a valid emergency contact no.";
        }
        // If there's an error, display it
        if (!errorMessage.empty())
        {
            mvwprintw(error_win, 0, 0, "%s", errorMessage.c_str());
            wrefresh(error_win);
            std::this_thread::sleep_for(std::chrono::seconds(2)); // Show error for 2 seconds
            werase(error_win);
            wrefresh(error_win); // Clear error window
        }
        else
        {
            done = true; // Proceed if validation succeeds
        }
    }

    form_driver(form, REQ_VALIDATION); // Final validation before saving data

    // Extract data from the form fields and update the field values
    u.fieldValues["fullName"].second = trim_whitespaces(field_buffer(fields[1], 0));
    u.fieldValues["identityCardNumber"].second = trim_whitespaces(field_buffer(fields[3], 0));
    u.fieldValues["height"].second = trim_whitespaces(field_buffer(fields[5], 0));
    u.fieldValues["weight"].second = trim_whitespaces(field_buffer(fields[7], 0));
    u.fieldValues["emergencyContactNumber"].second = trim_whitespaces(field_buffer(fields[9], 0));
    u.fieldValues["emergencyContactName"].second = trim_whitespaces(field_buffer(fields[11], 0));

    navigationHandler(form, fields, windows, Screen::UpdateSelectionPatientScreen);
}

void renderUpdateSelectionPatientScreen()
{
    renderHeader();      // Renders the header of the screen
    renderControlInfo(); // Renders control-related information

    Color &colorScheme = Color::getInstance();
    // We're using registration variables here for the menu
    RegistrationPatient &reg = RegistrationPatient::getInstance();
    UpdatePatient &u = UpdatePatient::getInstance();

    auto patient = std::dynamic_pointer_cast<Patient>(u.user); // Get patient details

    int baseline = 11;
    std::string header = "Updating " + patient->fullName + "'s account";
    mvprintw(baseline, (COLS - header.length()) / 2, "%s", header.c_str());

    bkgd(COLOR_PAIR(colorScheme.primary)); // Set background color

    int outer_height = 20;
    int outer_width = 60;

    int start_y = ((LINES - outer_height) / 2) + 4;
    int start_x = (COLS - outer_width) / 2;

    // Create the main window (win_body) to contain other elements
    WINDOW *win_body = newwin(outer_height, outer_width, start_y, start_x);
    wbkgd(win_body, COLOR_PAIR(colorScheme.primary));
    box(win_body, 0, 0);

    // Draw the subheader on win_body to display the continuation message
    std::string subHeader = "Personal Information (Continued)";
    mvwprintw(win_body, 1, (outer_width - subHeader.length()) / 2, "%s", subHeader.c_str());
    wrefresh(win_body); // Refresh win_body to show the subheader

    int inner_height = outer_height - 4;
    int inner_width = outer_width - 4;

    // Create the subwindow (win_form) inside win_body for displaying form
    WINDOW *win_form = derwin(win_body, inner_height, inner_width, 2, 2);
    wbkgd(win_form, COLOR_PAIR(colorScheme.primary));
    box(win_form, 0, 0);
    wrefresh(win_form); // Refresh win_form to show the box

    bool done = false;
    keypad(win_form, TRUE); // Enable keypad input
    curs_set(0);            // Hide cursor

    // Menu items are stored here for gender, religion, race, maritalStatus, nationality
    std::string *fields[] = {
        &patient->gender,
        &patient->religion,
        &patient->race,
        &patient->maritalStatus,
        &patient->nationality};

    // Initialize selected menu indices based on current patient information
    for (int i = 0; i < static_cast<int>(reg.menuArrs.size()); i++)
    {
        auto &menu = reg.menuArrs[i];
        auto it = std::find(menu.begin(), menu.end(), *fields[i]);

        if (it != menu.end())
        {
            reg.selectedIndices[i] = std::distance(menu.begin(), it);
        }
        else
        {
            reg.selectedIndices[i] = 0; // Default to the first menu item
        }
    }

    std::vector<WINDOW *> windows = {win_body, win_form};

    while (!done)
    {
        // Clear win_form and redraw its border each loop iteration
        werase(win_form);
        box(win_form, 0, 0);

        // Update registration data with selected values for each field
        u.fieldValues["gender"].second = reg.menuArrs[0][reg.selectedIndices[0]];
        u.fieldValues["religion"].second = reg.menuArrs[1][reg.selectedIndices[1]];
        u.fieldValues["race"].second = reg.menuArrs[2][reg.selectedIndices[2]];
        u.fieldValues["maritalStatus"].second = reg.menuArrs[3][reg.selectedIndices[3]];
        u.fieldValues["nationality"].second = reg.menuArrs[4][reg.selectedIndices[4]];

        int y_offset = 2; // Start from the second line in the form

        // Render all menus (gender, religion, race, etc.)
        for (int i = 0; i < static_cast<int>(reg.menuArrs.size()); ++i)
        {
            if (i == reg.currentMenu) // Highlight the current menu
            {
                wattron(win_form, A_BOLD);
            }
            renderHorizontalMenuStack(win_form, reg.menuArrs[i], reg.labelArr[i], y_offset, reg.selectedIndices[i], 2);
            if (i == reg.currentMenu) // Turn off highlighting after rendering
            {
                wattroff(win_form, A_BOLD);
            }
            y_offset += 3; // Move down for the next menu
        }

        wrefresh(win_form); // Refresh win_form to display the updated menu

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
            navigationHandler(nullptr, nullptr, windows, Screen::UpdatePersonalPatientScreen);
            break;
        }
    }

    u.handleUpdatePatient(patient->getId()); // Handle patient update with ID
    reg.reset();                             // Reset the registration process
    u.reset();                               // Reset the update process

    navigationHandler(nullptr, nullptr, windows, Screen::Database); // Navigate to Database screen
}

void renderUpdateAdminScreen()
{
    // Get instances of the color scheme and update logic for admin accounts
    Color &colorScheme = Color::getInstance();   // Get color scheme for UI design
    UpdateAdmin &u = UpdateAdmin::getInstance(); // Get instance of UpdateAdmin to handle admin updates
    bkgd(COLOR_PAIR(colorScheme.primary));       // Set background color for the screen using primary color scheme

    // Render the header and control information
    renderHeader();      // Render the page header (e.g., title or navigation bar)
    renderControlInfo(); // Render information about controls (e.g., instructions for user)

    // Cast the user instance to an Admin type to access admin-specific data
    auto admin = std::dynamic_pointer_cast<Admin>(u.user);

    // Setup for the header text showing the admin being updated
    int baseline = 11; // Position for header
    std::string header = "Updating " + admin->fullName + "'s account";
    mvprintw(baseline, (COLS - header.length()) / 2, "%s", header.c_str()); // Center the header

    // Set the background color again for any changes to layout
    bkgd(COLOR_PAIR(colorScheme.primary));

    // Define the dimensions for the outer window where the form will reside
    int outer_height = 16;
    int outer_width = 60;
    int start_y = ((LINES - outer_height) / 2) + 2; // Center the window vertically
    int start_x = (COLS - outer_width) / 2;         // Center the window horizontally

    // Create the outer window where form elements will be placed
    WINDOW *win_body = newwin(outer_height, outer_width, start_y, start_x);
    wbkgd(win_body, COLOR_PAIR(colorScheme.primary)); // Set background color for window body
    box(win_body, 0, 0);                              // Draw a border around the window

    // Render a subheader indicating the section
    std::string subHeader = "Account Information";
    mvwprintw(win_body, 1, (outer_width - subHeader.length()) / 2, "%s", subHeader.c_str()); // Center subheader

    // Define dimensions for the inner window, which will hold the form fields
    int inner_height = outer_height - 4;
    int inner_width = outer_width - 4;

    // Create the inner window for the form input area
    WINDOW *win_form = derwin(win_body, inner_height, inner_width, 2, 2); // Derivative window inside win_body
    wbkgd(win_form, COLOR_PAIR(colorScheme.primary));                     // Set background color
    box(win_form, 0, 0);                                                  // Draw a border around the form

    // Define form fields for admin account updates (username, password, full name, etc.)
    FIELD *fields[11];
    fields[0] = new_field(1, 10, 0, 1, 0, 0);  // Label: Username
    fields[1] = new_field(1, 30, 0, 13, 0, 0); // Input: Username
    fields[2] = new_field(1, 10, 2, 1, 0, 0);  // Label: Password
    fields[3] = new_field(1, 30, 2, 13, 0, 0); // Input: Password
    fields[4] = new_field(1, 10, 4, 1, 0, 0);  // Label: Full Name
    fields[5] = new_field(1, 30, 4, 13, 0, 0); // Input: Full Name
    fields[6] = new_field(1, 10, 6, 1, 0, 0);  // Label: Email
    fields[7] = new_field(1, 30, 6, 13, 0, 0); // Input: Email
    fields[8] = new_field(1, 16, 8, 1, 0, 0);  // Label: Contact Number
    fields[9] = new_field(1, 30, 8, 20, 0, 0); // Input: Contact Number
    fields[10] = NULL;                         // End of field array

    // Ensure all fields are created properly
    assert(fields[0] && fields[1] && fields[2] && fields[3] && fields[4] &&
           fields[5] && fields[6] && fields[7] && fields[8] && fields[9]);

    // Set default values for each field (e.g., populate with existing admin data)
    set_field_buffer(fields[0], 0, "Username:");
    set_field_buffer(fields[1], 0, u.fieldValues["username"].second.c_str());
    set_field_buffer(fields[2], 0, "Password:");
    set_field_buffer(fields[3], 0, u.fieldValues["password"].second.c_str());
    set_field_buffer(fields[4], 0, "Full Name:");
    set_field_buffer(fields[5], 0, u.fieldValues["fullName"].second.c_str());
    set_field_buffer(fields[6], 0, "Email:");
    set_field_buffer(fields[7], 0, u.fieldValues["email"].second.c_str());
    set_field_buffer(fields[8], 0, "Contact Number:");
    set_field_buffer(fields[9], 0, u.fieldValues["contactNumber"].second.c_str());

    // Set field options (labels as read-only and input fields as editable)
    for (int i = 0; fields[i]; i++)
    {
        if (i % 2 == 0) // Labels
        {
            set_field_opts(fields[i], O_VISIBLE | O_PUBLIC | O_STATIC); // Read-only for labels
        }
        else // Input fields
        {
            set_field_opts(fields[i], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE | O_AUTOSKIP); // Editable fields
        }
        set_field_back(fields[i], COLOR_PAIR(colorScheme.primary)); // Set background color for fields
    }

    // Create and display the form on the window
    FORM *form = new_form(fields);
    assert(form);
    set_form_win(form, win_form);                                                  // Set the window for the form
    set_form_sub(form, derwin(win_form, inner_height - 2, inner_width - 2, 1, 1)); // Subwindow for form fields
    post_form(form);                                                               // Display the form

    // Refresh the window to show updated UI
    wrefresh(win_body);
    wrefresh(win_form);

    // Create an error window for displaying validation messages
    int error_win_height = 1;
    int error_win_width = 40;
    int error_win_y = LINES - 2;
    int error_win_x = (COLS - error_win_width) / 2;
    WINDOW *error_win = newwin(error_win_height, error_win_width, error_win_y, error_win_x);
    wbkgd(error_win, COLOR_PAIR(colorScheme.danger)); // Set error window background color

    // Create a status window to show success or status messages
    int status_win_height = 5;
    int status_win_width = 34;
    int status_win_y = LINES - 6;
    int status_win_x = (COLS - status_win_width) / 2;
    WINDOW *status_win = newwin(status_win_height, status_win_width, status_win_y, status_win_x);
    box(status_win, 0, 0); // Draw a border for the status window

    // Input loop for the form
    bool done = false;
    int ch;

    std::vector<WINDOW *> windows = {win_body, win_form};

    // Loop to handle user input
    while (!done)
    {
        while ((ch = getch()) != '\n') // Wait for 'Enter' key to submit
        {
            driver_form(ch, form, fields, win_form, win_body, [&]()
                        { exitHandler(form, fields, windows); }, [&]()
                        { u.reset(); navigationHandler(form, fields, windows, Screen::Database); });
        }

        form_driver(form, REQ_VALIDATION); // Validate form fields

        std::string errorMessage;

        // Validate required fields
        if (!validateFields(fields))
        {
            errorMessage = "Please fill all required fields";
        }
        else if (!validateEmail(trim_whitespaces(field_buffer(fields[7], 0))))
        {
            errorMessage = "Please enter a valid email";
        }
        // If there's an error, display it
        if (!errorMessage.empty())
        {
            mvwprintw(error_win, 0, 0, "%s", errorMessage.c_str());
            wrefresh(error_win);
            std::this_thread::sleep_for(std::chrono::seconds(2)); // Show error for 2 seconds
            werase(error_win);
            wrefresh(error_win); // Clear error window
        }
        else
        {
            done = true; // Proceed if validation succeeds
        }
    }

    form_driver(form, REQ_VALIDATION); // Final validation after submission

    // Extract and update admin data
    u.fieldValues["username"].second = trim_whitespaces(field_buffer(fields[1], 0));
    u.fieldValues["password"].second = trim_whitespaces(field_buffer(fields[3], 0));
    u.fieldValues["fullName"].second = trim_whitespaces(field_buffer(fields[5], 0));
    u.fieldValues["email"].second = trim_whitespaces(field_buffer(fields[7], 0));
    u.fieldValues["contactNumber"].second = trim_whitespaces(field_buffer(fields[9], 0));

    // Perform the update action with the extracted data
    u.handleUpdateAdmin(admin->getId());

    u.reset();                                                      // Reset any temporary data
    navigationHandler(nullptr, nullptr, windows, Screen::Database); // Navigate back to the database screen
}
