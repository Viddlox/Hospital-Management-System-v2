#include <iostream>
#include <fstream>
#include <string>
#include <functional> // For std::hash
#include <windows.h> // For full screen function

using namespace std;

struct User {
    string username;
    string password;
};

// Function to hash a string
string hashString(const string& input) {
    hash<string> hasher;
    return to_string(hasher(input));
}

// Function to clear the screen
void clearScreen() {
#if defined(_WIN32) || defined(_WIN64)
    system("cls");  // Windows
#else
    system("clear"); // Linux/macOS
#endif
}

// Function to pause until a key is pressed
void pauseScreen() {
#if defined(_WIN32) || defined(_WIN64)
    system("pause");  // Windows-specific pause
#else
    cout << "Press Enter to continue..." << endl;
    cin.ignore(); // Ignore the newline left in the buffer
    cin.get();    // Wait for Enter
#endif
}

// Function to maximize the console window
void maximizeConsoleWindow() {
    HWND consoleWindow = GetConsoleWindow();
    if (consoleWindow) {
        ShowWindow(consoleWindow, SW_MAXIMIZE); // Maximizes the window
    }
}

// Color functions

// Red

string red = "\033[31m";
string endRed = "\033[0m";



// Function to display hospital ASCII art
void printHospitalLogo() {
    cout << red  << R"(
                               
         @@@@@@@@@@@@          
       @@@           @@@      
     @@@               @@@     __        __   _                            _             _    ____   ____   _   _                 _ _        _ 
   @@@       @@@@        @@@   \ \      / /__| | ___ ___  _ __ ___   ___  | |_ ___      / \  | __ ) / ___| | | | | ___  ___ _ __ (_) |_ __ _| |
   @@        @@@@         @@    \ \ /\ / / _ \ |/ __/ _ \| '_ ` _ \ / _ \ | __/ _ \    / _ \ |  _ \| |     | |_| |/ _ \/ __| '_ \| | __/ _` | |   
  @@    @@@@@@@@@@@@@@    @@     \ V  V /  __/ | (_| (_) | | | | | |  __/ | || (_) |  / ___ \| |_) | |___  |  _  | (_) \__ \ |_) | | || (_| | |
  @@    @@@@@@@@@@@@@@    @@      \_/\_/ \___|_|\___\___/|_| |_| |_|\___|  \__\___/  /_/   \_\____/ \____| |_| |_|\___/|___/ .__/|_|\__\__,_|_| 
  @@    @@@@@@@@@@@@@@    @@                                                                                                |_|                 
   @@        @@@@         @@   
   @@@       @@@@        @@    
     @@                @@     
      @@@            @@@       
         @@@@@@@@@@@@          
                               
)" << endRed << endl;
}

// Function to register a new user
void registerUser(const string& filename) {
    User user;
    cout << "Enter a username: ";
    cin >> user.username;
    cout << "Enter a password: ";
    cin >> user.password;

    // Hash the username and password
    string hashedUsername = hashString(user.username);
    string hashedPassword = hashString(user.password);

    ofstream file(filename, ios::app);
    if (!file) {
        cerr << "Error opening file!" << endl;
        return;
    }
    file << hashedUsername << " " << hashedPassword << endl;
    file.close();
    cout << "Registration successful!" << endl;
}


// Main Menu
void userDashboard(const string& username) {
    int choice;
    do {
        cout << "\nWelcome to the User Dashboard, " << username << "!\n";
        cout << "1. View Profile\n";
        cout << "2. Settings\n";
        cout << "3. Logout\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            cout << "\nProfile Details:\n";
            cout << "Username: " << username << endl;
            cout << "Feature coming soon...\n";
            break;
        case 2:
            cout << "\nSettings Page\n";
            cout << "Feature coming soon...\n";
            break;
        case 3:
            cout << "Logging out...\n";
            break;
        default:
            cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 3);
}

int main() {
    User user;
    const string userLoginFile = "users.txt";


    while(true) {
        clearScreen(); // Clear the screen before every login attempt
        printHospitalLogo();
        maximizeConsoleWindow(); 
        cout << "\nPatient Management System V1.0\n";
        cout << "\nABC Hospital\n";
        cout << "Username: ";
        cin >> user.username;
        cout << "Password: ";
        cin >> user.password;

        // Read the user login file
        ifstream userLogin(userLoginFile);
 

        // Hash the input for comparison
        string hashedUsername = hashString(user.username);
        string hashedPassword = hashString(user.password);

        bool isAuthenticated = false;
        string username, password;
        while (userLogin >> username >> password) {
            if (username == hashedUsername && password == hashedPassword) {
                cout << "Login successful! Welcome, " << user.username << "." << endl;
                isAuthenticated = true;
                userLogin.close();
                userDashboard(user.username);
                return 0;
            }
        }

        userLogin.close(); // Close the file after processing

        if (isAuthenticated) {
            break; // Exit the outer while loop once login is successful
        }
        else {
            cout << "Invalid username or password. Please try again." << endl;
            pauseScreen();
        }
    }

    return 0; // Exit successfully
}
