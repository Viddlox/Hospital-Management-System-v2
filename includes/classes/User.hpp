#ifndef USER_H
#define USER_H

// Standard library headers
#include <string>     // Provides std::string for user attributes
#include <vector>     // Allows storage of multiple items in a dynamic array
#include <fstream>    // Enables reading and writing user data to files
#include <filesystem> // Supports file and directory operations

// Project-specific headers
#include "utils.hpp" // Provides utility functions such as UUID generation and timestamp formatting
#include "json.hpp"  // Enables JSON serialization/deserialization

// Using the nlohmann JSON library
using json = nlohmann::json;

// Using filesystem namespace to simplify path management
namespace fs = std::filesystem;

// Enum class defining user roles
enum class Role
{
    Admin,   // Represents administrative users
    Patient, // Represents patients in the system
    User     // Default role, used as a fallback
};

// Base class representing a generic user
class User
{
protected:
    std::string id; // Unique identifier for the user

public:
    // Common user attributes
    std::chrono::system_clock::time_point createdAt; // Timestamp when the user was created
    std::string username;                            // Unique username for authentication
    std::string password;                            // User password (should be stored securely)
    std::string fullName;                            // User's full legal name
    std::string email;                               // Contact email address
    std::string contactNumber;                       // Contact phone number
    Role role;                                       // User's role in the system

    // Default constructor
    User()
        : id(generateId()),                            // Assign a unique ID
          createdAt(std::chrono::system_clock::now()), // Set account creation timestamp
          username(""),
          password(""),
          fullName(""),
          email(""),
          contactNumber(""),
          role(Role::User)
    {
    } // Default role is 'User'

    // Parameterized constructor
    User(const std::string &username, const std::string &password, const std::string &fullName,
         const std::string &email, const std::string &contactNumber, Role role)
        : id(generateId()),                            // Generate a new user ID
          createdAt(std::chrono::system_clock::now()), // Store account creation time
          username(username),
          password(password),
          fullName(fullName),
          email(email),
          contactNumber(contactNumber),
          role(role)
    {
    }

    virtual ~User() = default; // Virtual destructor ensures proper cleanup in derived classes

    // 🔹 Generates a unique ID for the user
    static std::string generateId()
    {
        return generateUUID(); // Calls utility function to generate UUID
    }

    // 🔹 Returns the account creation timestamp as a formatted string
    std::string getCreatedAt() const
    {
        return formatTimestamp(createdAt); // Converts timestamp to readable format
    }

    // 🔹 Converts a Role enum to its string representation
    static std::string getRoleToString(Role role)
    {
        switch (role)
        {
        case Role::Admin:
            return "admin";
        case Role::Patient:
            return "patient";
        default:
            return "user";
        }
    }

    // 🔹 Converts a string to its corresponding Role enum
    static Role getRoleToEnum(const std::string &role)
    {
        if (role == "admin")
            return Role::Admin;
        else if (role == "patient")
            return Role::Patient;
        else
            throw std::invalid_argument("Unknown role: " + role); // Throw exception if role is invalid
    }

    // 🔹 Getter functions for accessing private attributes
    const std::string &getId() const { return id; }
    const std::string &getUsername() const { return username; }
    const std::string &getFullName() const { return fullName; }
    const std::string &getPassword() const { return password; }
    Role getRole() const { return role; }
};

#endif // USER_H
