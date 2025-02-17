#ifndef ADMIN_H
#define ADMIN_H

#include "User.hpp" // Include base User class for inheritance

// The Admin class inherits from User and represents an administrator account.
class Admin : public User
{
public:
	// Constructor: Initializes an Admin with optional parameters for user details.
	// By default, initializes an empty Admin object.
	Admin(const std::string &username = "",
		  const std::string &password = "",
		  const std::string &fullName = "",
		  const std::string &email = "",
		  const std::string &contactNumber = "")
		: User(username, password, fullName, email, contactNumber, Role::Admin) {}

	// Convert Admin object to JSON format for serialization
	friend void to_json(json &j, const Admin &a)
	{
		j = json{
			{"id", a.id},						 // Unique admin ID
			{"role", a.getRoleToString(a.role)}, // Role as string (should be "Admin")
			{"username", a.username},			 // Admin username
			{"password", a.password},			 // Admin password (Consider encrypting before saving)
			{"fullName", a.fullName},			 // Full name of the admin
			{"email", a.email},					 // Email address
			{"contactNumber", a.contactNumber},	 // Contact number
			{"createdAt", a.getCreatedAt()}		 // Timestamp of account creation
		};
	}

	// Convert JSON object back to an Admin instance (deserialization)
	friend void from_json(const json &j, Admin &a)
	{
		// Extract values from JSON and assign them to the Admin object
		a.id = j.at("id").get<std::string>();
		a.role = a.getRoleToEnum(j.at("role").get<std::string>());
		a.username = j.at("username").get<std::string>();
		a.password = j.at("password").get<std::string>();
		a.fullName = j.at("fullName").get<std::string>();
		a.email = j.at("email").get<std::string>();
		a.contactNumber = j.at("contactNumber").get<std::string>();

		// Deserialize the "createdAt" field (stored as a string) into a time_point object
		std::string createdAtStr = j.at("createdAt").get<std::string>();
		std::tm tm = {};
		std::istringstream ss(createdAtStr);
		ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S"); // Format should match stored timestamps

		// If parsing fails, throw an error
		if (ss.fail())
		{
			throw std::invalid_argument("Invalid date format for createdAt");
		}

		// Convert parsed std::tm to system_clock::time_point
		std::time_t time = std::mktime(&tm);
		a.createdAt = std::chrono::system_clock::from_time_t(time);
	}

	// Save Admin object as a JSON file in the database
	void saveToFile() const
	{
		// Ensure the admin directory exists
		std::filesystem::create_directories("db/admin");

		// Generate file path using the Admin ID
		std::string filePath = "db/admin/" + id + ".json";

		// Serialize Admin object to JSON
		json j = *this;

		// Write JSON data to file
		std::ofstream file(filePath);
		if (file.is_open())
		{
			file << j.dump(4); // Pretty print JSON with 4-space indentation
			file.close();
		}
		else
		{
			// Log an error if the file couldn't be opened
			std::cerr << "Error: Could not open file to save admin data." << std::endl;
		}
	}
};

#endif // ADMIN_H
