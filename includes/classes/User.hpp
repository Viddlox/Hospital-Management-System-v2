#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
#include <fstream>
#include <filesystem>

#include "Utils.hpp"
#include "json.hpp"

using json = nlohmann::json;

namespace fs = std::filesystem;

enum class Role
{
	Admin,
	Patient,
	User
};

class User
{
protected:
    std::string id;
    std::chrono::system_clock::time_point createdAt;

public:
    std::string username;
    std::string password;
    std::string fullName;
    Role role;

    User()
        : id(generateId()),
          createdAt(std::chrono::system_clock::now()),
          username(""), 
          password(""),
          fullName(""),
          role(Role::User) {}

    User(const std::string &username, const std::string &password, const std::string &fullName, Role role)
        : id(generateId()),
          createdAt(std::chrono::system_clock::now()),
          username(username), 
          password(password),
          fullName(fullName),
          role(role) {}

    virtual ~User() = default; // Virtual destructor for proper cleanup of derived classes

    // methods
    static std::string generateId()
    {
        return generateUUID();
    }

    std::string getCreatedAt() const
    {
        return formatTimestamp(createdAt);
    }
    
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

    static Role getRoleToEnum(const std::string &role)
    {
        if (role == "admin")
            return Role::Admin;
        else if (role == "patient")
            return Role::Patient;
        else
            throw std::invalid_argument("Unknown role: " + role);
    }

    const std::string &getId() const { return id; }
    const std::string &getUsername() const { return username; }
    const std::string &getFullName() const { return fullName; }
    const std::string &getPassword() const { return password; }
    Role getRole() const { return role; }
};


#endif
