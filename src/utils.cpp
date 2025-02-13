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