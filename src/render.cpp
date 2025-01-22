#include "render.hpp"

struct Color
{
	const char *primary = "\033[38;2;0;255;0m";
	const char *secondary = "\033[32m";
};

void renderHeader()
{
	Color colorMapper;
	std::cout << colorMapper.primary;
	std::cout << R"(
[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
||                                                          ||
||   ███╗   ███╗███████╗██████╗ ████████╗███████╗██╗  ██╗   ||
||   ████╗ ████║██╔════╝██╔══██╗╚══██╔══╝██╔════╝██║ ██╔╝   ||
||   ██╔████╔██║█████╗  ██║  ██║   ██║   █████╗  █████╔╝    ||
||   ██║╚██╔╝██║██╔══╝  ██║  ██║   ██║   ██╔══╝  ██╔═██╗    ||
||   ██║ ╚═╝ ██║███████╗██████╔╝   ██║   ███████╗██║  ██╗   ||
||   ╚═╝     ╚═╝╚══════╝╚═════╝    ╚═╝   ╚══════╝╚═╝  ╚═╝   ||
||                                                          ||
[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
			version 1.0.0
    )";
}

void renderTime(std::time_t time)
{
    // Move to the second line and render the time
    std::cout << "\033[14;1H";  // Move cursor to row 2, column 1 (optional: remove if not using ANSI escape sequences)
    std::cout << "Current Time: " << std::ctime(&time) << std::flush;
}

void clearScreen()
{
    #ifdef _WIN32
        system("cls"); 
    #else
        system("clear");
    #endif
}