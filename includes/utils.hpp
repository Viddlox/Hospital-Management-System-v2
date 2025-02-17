#ifndef UTILS_H
#define UTILS_H

// Standard library includes
#include <chrono>     // For working with timestamps and time points
#include <iostream>   // For input/output operations
#include <iomanip>    // For formatting output
#include <algorithm>  // For string manipulation (e.g., trimming, case conversion)
#include <cctype>     // For character classification (e.g., checking whitespace)
#include <cstring>    // For C-style string manipulation

#include "uuid_v4.h"  // Include for generating unique UUIDs

// Function prototypes

/**
 * @brief Converts a time_point into a formatted timestamp string.
 * @param timePoint The time_point to format.
 * @return A string representing the formatted timestamp.
 */
std::string formatTimestamp(std::chrono::system_clock::time_point timePoint);

/**
 * @brief Generates a universally unique identifier (UUID).
 * @return A string representing the generated UUID.
 */
std::string generateUUID();

/**
 * @brief Splits a string into substrings based on a given delimiter.
 * @param s The input string.
 * @param delimiter The character used to split the string.
 * @return A vector containing the split substrings.
 */
std::vector<std::string> split(const std::string &s, char delimiter);

/**
 * @brief Trims leading and trailing whitespace from a string.
 * @param str The input string.
 * @return A new string with whitespace removed.
 */
std::string trim(const std::string &str);

/**
 * @brief Converts a string to lowercase.
 * @param s The input string.
 * @return A new string in lowercase.
 */
std::string toLower(const std::string &s);

/**
 * @brief Trims leading and trailing whitespace from a C-style string.
 * @param str The input C-string.
 * @return A pointer to the trimmed C-string.
 */
char *trim_whitespaces(char *str);

/**
 * @brief Calculates a person's age based on their identity card number.
 * @param identityCardNumber The identity card number (assumed to contain birth info).
 * @return The calculated age.
 */
int calculateAge(const std::string &identityCardNumber);

/**
 * @brief Calculates the Body Mass Index (BMI) based on weight and height.
 * @param weight The weight as a string (assumed to be in kg).
 * @param height The height as a string (assumed to be in meters).
 * @return The calculated BMI value.
 */
double calculateBMI(const std::string &weight, const std::string &height);

#endif // UTILS_H
