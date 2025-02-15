#ifndef UTILS_H
#define UTILS_H

#include <chrono>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <cstring>

#include "uuid_v4.h"

std::string formatTimestamp(std::chrono::system_clock::time_point timePoint);
std::string generateUUID();
std::vector<std::string> split(const std::string &s, char delimiter);
std::string trim(const std::string &str);
std::string toLower(const std::string &s);
char *trim_whitespaces(char *str);
int calculateAge(const std::string &identityCardNumber);
double calculateBMI(const std::string &weight, const std::string &height);

#endif