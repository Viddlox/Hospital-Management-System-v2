#include "Utils.hpp"

std::string generateUUID()
{
    UUIDv4::UUIDGenerator<std::mt19937_64> uuidGenerator;
    UUIDv4::UUID uuid = uuidGenerator.getUUID();
    return std::string(uuid.str());
};

std::string formatTimestamp(std::chrono::system_clock::time_point timePoint)
{
    std::time_t t = std::chrono::system_clock::to_time_t(timePoint);
    std::tm tm = *std::localtime(&t);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::vector<std::string> split(const std::string &s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}

std::string trim(const std::string &str)
{
    size_t start = str.find_first_not_of(" \t\n\r\f\v");
    size_t end = str.find_last_not_of(" \t\n\r\f\v");

    return (start == std::string::npos || end == std::string::npos)
               ? ""
               : str.substr(start, end - start + 1);
}

std::string toLower(const std::string &s)
{
    std::string lowerStr = s;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    return lowerStr;
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

bool validateContactNumber(const std::string &contactNumber)
{
    // Define a regex pattern for a valid contact number
    std::regex phonePattern(R"(^\+?\d{10,15}$)");

    // Check if the input matches the pattern
    return std::regex_match(contactNumber, phonePattern);
}

bool validateIdentityCardNumber(const std::string &identityCardNumber)
{
    try
    {
        int age = calculateAge(identityCardNumber);
        return (age >= 0 && age <= 130);
    }
    catch (...)
    {
        return false;
    }
}

bool validateHeightAndWeight(const std::string &height, const std::string &weight)
{
    try
    {
        int bmi = calculateBMI(weight, height);
        return (bmi >= 10.0 && bmi <= 50.0);
    }
    catch (...)
    {
        return false;
    } 
}

bool validateEmail(const std::string &email)
{
    // Define a regex pattern for validating email
    const std::regex emailPattern(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");

    // Return true if email matches the pattern, otherwise false
    return std::regex_match(email, emailPattern);
}