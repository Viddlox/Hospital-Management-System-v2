#include "render.hpp"

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
    attron(COLOR_PAIR(colorScheme.primary));

    int row = 0;
    int col = (COLS - 81) / 2;

    mvprintw(row++, col, "================================================================================");
    mvprintw(row++, col, "  /$$      /$$                 /$$ /$$$$$$$$           /$$                      ");
    mvprintw(row++, col, " | $$$    /$$$                | $$|__  $$__/          | $$                /$$   ");
    mvprintw(row++, col, " | $$$$  /$$$$  /$$$$$$   /$$$$$$$   | $$     /$$$$$$ | $$   /$$         | $$   ");
    mvprintw(row++, col, " | $$ $$/$$ $$ /$$__  $$ /$$__  $$   | $$    /$$__  $$| $$  /$$/       /$$$$$$$$");
    mvprintw(row++, col, " | $$  $$$| $$| $$$$$$$$| $$  | $$   | $$   | $$$$$$$$| $$$$$$/       |__  $$__/");
    mvprintw(row++, col, " | $$\\  $ | $$| $$_____/| $$  | $$   | $$   | $$_____/| $$_  $$          | $$  ");
    mvprintw(row++, col, " | $$ \\/  | $$|  $$$$$$$|  $$$$$$$   | $$   |  $$$$$$$| $$ \\  $$         |__/ ");
    mvprintw(row++, col, " |__/     |__/ \\_______/ \\_______/   |__/    \\_______/|__/  \\__/            ");
    mvprintw(row++, col, "================================================================================");
    attroff(COLOR_PAIR(colorScheme.primary));
    refresh();
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

char* trim_whitespaces(char *str)
{
	char *end;

	// trim leading space
	while(isspace(*str))
		str++;

	if(*str == 0) // all spaces?
		return str;

	// trim trailing space
	end = str + strnlen(str, 128) - 1;

	while(end > str && isspace(*end))
		end--;

	// write new null terminator
	*(end+1) = '\0';

	return str;
}

void driver(int ch, FORM *form, WINDOW *win_form, FIELD **fields)
{
    switch (ch) {
        case KEY_F(2):
            // Sync the field buffer with what's displayed
            form_driver(form, REQ_NEXT_FIELD);
            form_driver(form, REQ_PREV_FIELD);

            // Debug: Print field values
            move(LINES - 3, 2);
            for (int i = 0; fields[i]; i++) {
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
