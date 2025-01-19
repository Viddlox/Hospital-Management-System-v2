#ifndef ADMIN_H
#define ADMIN_H

#include "User.hpp"

class Admin : public User
{
public:
	Admin(const std::string &username = "", const std::string &password = "")
		: User(username, password, Role::Admin) {}

	// Serialize admin object to JSON
	friend void to_json(json &j, const Admin &a)
	{
		j = json{
			{"id", a.id},
			{"role", a.getRoleToString(a.role)},
			{"username", a.username},
			{"password", a.password},
			{"createdAt", a.getCreatedAt()}};
	}

	// Deserialize admin object from JSON
	friend void from_json(const json &j, Admin &a)
	{
		a.id = j.at("id").get<std::string>();
		a.role = a.getRoleToEnum(j.at("role").get<std::string>());
		a.username = j.at("username").get<std::string>();
		a.password = j.at("password").get<std::string>();

		// Deserialize createdAt as a string and convert it to time_point
		std::string createdAtStr = j.at("createdAt").get<std::string>();
		std::tm tm = {};
		std::istringstream ss(createdAtStr);
		ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S"); // Match the format of your timestamp

		// If parsing is successful, convert tm to time_point
		if (ss.fail())
		{
			throw std::invalid_argument("Invalid date format for createdAt");
		}

		// Convert the std::tm to system_clock::time_point
		std::time_t time = std::mktime(&tm);
		a.createdAt = std::chrono::system_clock::from_time_t(time);
	}
	void saveToFile() const
	{
		// Ensure the folder exists
		std::filesystem::create_directories("db/admin");

		// Generate the file name using the patient ID
		std::string filePath = "db/admin/" + id + ".json";

		// Serialize the object to JSON
		json j = *this;

		// Write to the file
		std::ofstream file(filePath);
		if (file.is_open())
		{
			file << j.dump(4); // Pretty print with 4 spaces
			file.close();
			std::cout << "Admin saved to file: " << filePath << std::endl;
		}
		else
		{
			std::cerr << "Error: Could not open file to save admin data." << std::endl;
		}
	}
};

#endif
