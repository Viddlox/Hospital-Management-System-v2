#ifndef USER_MANAGER_H
#define USER_MANAGER_H

#include <unordered_map> // Used for storing and managing user data efficiently
#include <memory>		 // Enables the use of smart pointers (std::shared_ptr, std::unique_ptr)
#include <functional>	 // Provides std::function for storing and invoking update functions

// User-related class headers
#include "User.hpp"	   // Base class for different user roles (Admin, Patient)
#include "Admin.hpp"   // Derived class representing Admin users
#include "Patient.hpp" // Derived class representing Patient users

// Utility functions
#include "utils.hpp" // Helper functions (e.g., string manipulation, validation)

// Admissions management
#include "admissions.hpp" // Manages patient admissions and related operations


// The UserManager class is responsible for managing the CRUD operations of user-related objects
class UserManager
{
private:
	// In-memory storage of users as shared pointers for fast lookups
	std::unordered_map<std::string, std::shared_ptr<User>> userMap;
	std::shared_ptr<User> currentUser; // Currently logged-in user

	// Singleton constructor: private to prevent direct instantiation
	UserManager()
	{
		populateUserMap(); // Load all users into memory
	}
	~UserManager() = default;
	UserManager(const UserManager &) = delete;
	UserManager &operator=(const UserManager &) = delete;

	// Load a user from a file given their user ID and role
	std::shared_ptr<User> getUserFromFile(const std::string &userId, const std::string &role)
	{
		std::shared_ptr<User> user = nullptr;
		std::string filePath = "db/" + role + "/" + userId + ".json";

		// Check if the file exists before attempting to read
		if (std::filesystem::exists(filePath))
		{
			std::ifstream file(filePath);
			if (file.is_open())
			{
				nlohmann::json j;
				file >> j;
				file.close();

				// Deserialize the JSON data into the appropriate user object
				if (role == "admin")
				{
					user = std::make_shared<Admin>();
					from_json(j, *std::dynamic_pointer_cast<Admin>(user));
				}
				else if (role == "patient")
				{
					user = std::make_shared<Patient>();
					from_json(j, *std::dynamic_pointer_cast<Patient>(user));
				}
				userMap[userId] = user; // Cache the user in memory
			}
		}
		return user;
	}

	// Delete a user file based on their ID
	bool deleteUserFromFile(const std::string &userId)
	{
		std::vector<Role> roles = {Role::Admin, Role::Patient, Role::User};
		for (const auto &role : roles)
		{
			std::string roleStr = User::getRoleToString(role);
			std::string filePath = "db/" + roleStr + "/" + userId + ".json";

			if (fs::exists(filePath))
			{
				try
				{
					fs::remove(filePath);
					return true;
				}
				catch (const std::exception &e)
				{
					std::cerr << "Error deleting file " << filePath << ": " << e.what() << std::endl;
					return false;
				}
			}
		}
		return false;
	}

	// Load all user records from the database into memory
	void populateUserMap()
	{
		std::vector<Role> roles = {Role::Admin, Role::Patient, Role::User};
		for (const auto &role : roles)
		{
			std::string roleStr = User::getRoleToString(role);
			std::string filePath = "db/" + roleStr + "/";

			if (!std::filesystem::exists(filePath))
			{
				std::cerr << "Directory not found: " << filePath << std::endl;
				continue;
			}

			for (const auto &entry : std::filesystem::directory_iterator(filePath))
			{
				if (entry.path().extension() == ".json")
				{
					std::ifstream file(entry.path());
					if (file.is_open())
					{
						nlohmann::json j;
						file >> j;
						file.close();
						std::shared_ptr<User> user = nullptr;

						if (roleStr == "admin")
						{
							user = std::make_shared<Admin>();
							from_json(j, *std::dynamic_pointer_cast<Admin>(user));
						}
						else if (roleStr == "patient")
						{
							user = std::make_shared<Patient>();
							from_json(j, *std::dynamic_pointer_cast<Patient>(user));
						}

						userMap[j["id"]] = user;
					}
				}
			}
		}
	}

public:
	// Get the singleton instance of UserManager
	static UserManager &getInstance()
	{
		static UserManager instance;
		return instance;
	}

	// Create a new patient record and store it in the user map and file system
	void createPatient(const std::string &username, const std::string &password, int age, const std::string &fullName,
					   const std::string &religion, const std::string &nationality,
					   const std::string &identityCardNumber, const std::string &maritalStatus, const std::string &gender,
					   const std::string &race, const std::string &email, const std::string &contactNumber,
					   const std::string &emergencyContactNumber, const std::string &emergencyContactName, const std::string &address,
					   double bmi, const std::string &height, const std::string &weight, Admissions::Department dept)
	{
		// Create a new Patient object
		std::shared_ptr<Patient> newPatient = std::make_shared<Patient>(
			username, password, age, fullName, religion, nationality, identityCardNumber,
			maritalStatus, gender, race, email, contactNumber, emergencyContactNumber, emergencyContactName,
			address, bmi, height, weight, dept);

		// Try inserting the patient into the user map
		auto result = userMap.insert({newPatient->getId(), newPatient});
		if (!result.second)
		{
			// User already exists, print an error message
			std::cout << "Patient with ID " << newPatient->getId() << " already exists.\n";
			std::cout << "Patient with username " << newPatient->getUsername() << " already exists.\n";
			return;
		}

		// Save patient details to a file for persistence
		newPatient->saveToFile();
	}

	// Create a new admin record and store it in the user map and file system
	void createAdmin(const std::string &username, const std::string &password, const std::string &fullName, const std::string &email, const std::string &contactNumber)
	{
		// Create a new Admin object
		std::shared_ptr<Admin> newAdmin = std::make_shared<Admin>(username, password, fullName, email, contactNumber);

		// Try inserting the admin into the user map
		auto result = userMap.insert({newAdmin->getId(), newAdmin});
		if (!result.second)
		{
			// User already exists, print an error message
			std::cout << "Admin with ID " << newAdmin->getId() << " already exists.\n";
			std::cout << "Admin with username " << newAdmin->getUsername() << " already exists.\n";
			return;
		}

		// Save admin details to a file for persistence
		newAdmin->saveToFile();
	}

	// Retrieve a user record by ID (either from memory or file system)
	std::shared_ptr<User> getUserById(const std::string &userId)
	{
		// Check if the user is already stored in memory
		auto it = userMap.find(userId);
		if (it != userMap.end())
		{
			return it->second;
		}

		// If not found in memory, check in the file system for admin users
		std::shared_ptr<User> user = getUserFromFile(userId, "admin");
		if (user)
		{
			userMap[userId] = user;
			return user;
		}

		// Check in the file system for patient users
		user = getUserFromFile(userId, "patient");
		if (user)
		{
			userMap[userId] = user;
			return user;
		}

		// If user is not found, return nullptr
		return nullptr;
	}

	// Retrieve a user record by username (searches both memory and file system)
	std::shared_ptr<User> getUserByUsername(const std::string &username)
	{
		// Normalize the input username by trimming and converting to lowercase
		std::string trimmedUsername = toLower(trim(username));

		// Search in-memory storage first
		for (const auto &pair : userMap)
		{
			if (toLower(trim(pair.second->getUsername())) == trimmedUsername)
			{
				return pair.second;
			}
		}

		// If not found in memory, check in the file system across all roles
		std::vector<Role> roles = {Role::Admin, Role::Patient, Role::User};
		for (const auto &role : roles)
		{
			std::string roleStr = User::getRoleToString(role);
			std::string filePath = "db/" + roleStr + "/";

			try
			{
				// Iterate over user files in the role directory
				for (const auto &entry : std::filesystem::directory_iterator(filePath))
				{
					if (entry.path().extension() == ".json")
					{
						// Read the user file
						std::ifstream file(entry.path());
						if (file.is_open())
						{
							nlohmann::json j;
							file >> j;
							file.close();

							// If username matches, return the user object
							if (j.contains("username") && toLower(trim(j["username"].get<std::string>())) == trimmedUsername)
							{
								auto user = getUserFromFile(j["id"], roleStr);
								if (user)
								{
									return user;
								}
							}
						}
					}
				}
			}
			catch (const std::exception &e)
			{
				return nullptr;
			}
		}

		// If user is not found, return nullptr
		return nullptr;
	}

	// Retrieve a user record by full name (searches both memory and file system)
	std::shared_ptr<User> getUserByName(const std::string &fullName)
	{
		// Normalize the input name by trimming and converting to lowercase
		std::string trimmedName = toLower(trim(fullName));

		// Search in-memory storage first
		for (const auto &pair : userMap)
		{
			if (toLower(trim(pair.second->getFullName())) == trimmedName)
			{
				return pair.second;
			}
		}

		// If not found in memory, check in the file system across all roles
		std::vector<Role> roles = {Role::Admin, Role::Patient, Role::User};
		for (const auto &role : roles)
		{
			std::string roleStr = User::getRoleToString(role);
			std::string filePath = "db/" + roleStr + "/";

			try
			{
				// Iterate over user files in the role directory
				for (const auto &entry : std::filesystem::directory_iterator(filePath))
				{
					if (entry.path().extension() == ".json")
					{
						// Read the user file
						std::ifstream file(entry.path());
						if (file.is_open())
						{
							nlohmann::json j;
							file >> j;
							file.close();

							// If full name matches, return the user object
							if (j.contains("fullName") && toLower(trim(j["fullName"])) == trimmedName)
							{
								auto user = getUserFromFile(j["id"], roleStr);
								if (user)
								{
									return user;
								}
							}
						}
					}
				}
			}
			catch (const std::exception &e)
			{
				return nullptr;
			}
		}

		// If user is not found, return nullptr
		return nullptr;
	}

	// Delete a user record by ID (removes from memory and file system)
	void deleteUserById(const std::string &userId)
	{
		// Check if the user exists in memory
		auto it = userMap.find(userId);
		if (it != userMap.end())
		{
			// Remove user from memory storage
			userMap.erase(it);
		}

		// Attempt to delete user from file system
		bool isDeleted = deleteUserFromFile(userId);
		if (!isDeleted)
		{
			std::cout << "User with ID " << userId << " not found.\n";
		}
	}

	// Update a user's record based on user ID, field name, and new value
	void updateUser(const std::string &userId, const std::string &fieldName, const std::string &newValue)
	{
		// Retrieve user object from ID
		auto user = getUserById(userId);
		if (!user)
		{
			std::cerr << "User with ID " << userId << " not found.\n";
			return;
		}

		Role role = user->role;

		// Handle Admin user updates
		if (role == Role::Admin)
		{
			auto admin = std::dynamic_pointer_cast<Admin>(user);
			if (!admin)
			{
				std::cerr << "Failed to cast User to Admin for user ID " << userId << ".\n";
				return;
			}

			// Define allowed fields for Admin updates and corresponding update logic
			std::unordered_map<std::string, std::function<void(const std::string &)>> adminUpdates = {
				{"username", [&admin](const std::string &value)
				 { admin->username = value; }},
				{"password", [&admin](const std::string &value)
				 { admin->password = value; }},
				{"fullName", [&admin](const std::string &value)
				 { admin->fullName = value; }},
				{"email", [&admin](const std::string &value)
				 { admin->email = value; }},
				{"contactNumber", [&admin](const std::string &value)
				 { admin->contactNumber = value; }},
			};

			// Apply update if field is valid
			auto it = adminUpdates.find(fieldName);
			if (it != adminUpdates.end())
			{
				it->second(newValue);
				admin->saveToFile(); // Save changes to file storage
			}
			else
			{
				std::cerr << "Field '" << fieldName << "' is not valid for Admin.\n";
			}
			return;
		}

		// Handle Patient user updates
		if (role == Role::Patient)
		{
			auto patient = std::dynamic_pointer_cast<Patient>(user);
			if (!patient)
			{
				std::cerr << "Failed to cast User to Patient for user ID " << userId << ".\n";
				return;
			}

			// Define allowed fields for Patient updates and corresponding update logic
			std::unordered_map<std::string, std::function<void(const std::string &)>> patientUpdates = {
				// Personal Information
				{"age", [&patient](const std::string &value)
				 { patient->age = std::stoi(value); }},
				{"fullName", [&patient](const std::string &value)
				 { patient->fullName = value; }},
				{"religion", [&patient](const std::string &value)
				 { patient->religion = value; }},
				{"nationality", [&patient](const std::string &value)
				 { patient->nationality = value; }},
				{"identityCardNumber", [&patient](const std::string &value)
				 { patient->identityCardNumber = value; }},
				{"maritalStatus", [&patient](const std::string &value)
				 { patient->maritalStatus = value; }},
				{"gender", [&patient](const std::string &value)
				 { patient->gender = value; }},
				{"race", [&patient](const std::string &value)
				 { patient->race = value; }},

				// Contact Information
				{"email", [&patient](const std::string &value)
				 { patient->email = value; }},
				{"contactNumber", [&patient](const std::string &value)
				 { patient->contactNumber = value; }},
				{"emergencyContactNumber", [&patient](const std::string &value)
				 { patient->emergencyContactNumber = value; }},
				{"emergencyContactName", [&patient](const std::string &value)
				 { patient->emergencyContactName = value; }},
				{"address", [&patient](const std::string &value)
				 { patient->address = value; }},
				{"username", [&patient](const std::string &value)
				 { patient->username = value; }},
				{"password", [&patient](const std::string &value)
				 { patient->password = value; }},
				{"bmi", [&patient](const std::string &value)
				 { patient->bmi = std::stod(value); }},

				// Medical Information
				{"height", [&patient](const std::string &value)
				 { patient->height = value; }},
				{"weight", [&patient](const std::string &value)
				 { patient->weight = value; }},
			};

			// Apply update if field is valid
			auto it = patientUpdates.find(fieldName);
			if (it != patientUpdates.end())
			{
				it->second(newValue);
				patient->saveToFile(); // Save changes to file storage
			}
			else
			{
				std::cerr << "Field '" << fieldName << "' is not valid for Patient.\n";
			}
			return;
		}

		// Handle unknown roles
		std::cerr << "Unknown role for user ID " << userId << ".\n";
	}
	// Validate user credentials and check if the user is an Admin
	bool validateUser(const std::string &username, const std::string &password)
	{
		auto user = getUserByUsername(username);

		// Ensure the user exists, is an Admin, and the password matches
		if (user && user->getRole() == Role::Admin && user->getPassword() == password)
		{
			setCurrentUser(user); // Set the current user
			return true;
		}
		return false;
	}

	// Count the number of Admin users by checking files in the admin directory
	int getAdminCount()
	{
		std::string filePath = "db/admin/";
		int count = 0;

		// Check if the directory exists and is valid
		if (std::filesystem::exists(filePath) && std::filesystem::is_directory(filePath))
		{
			// Iterate through files in the directory
			for (const auto &entry : std::filesystem::directory_iterator(filePath))
			{
				if (std::filesystem::is_regular_file(entry))
				{
					++count;
				}
			}
		}
		return count;
	}

	// Count the number of Patient users by checking files in the patient directory
	int getPatientCount()
	{
		std::string filePath = "db/patient/";
		int count = 0;

		// Check if the directory exists and is valid
		if (std::filesystem::exists(filePath) && std::filesystem::is_directory(filePath))
		{
			// Iterate through files in the directory
			for (const auto &entry : std::filesystem::directory_iterator(filePath))
			{
				if (std::filesystem::is_regular_file(entry))
				{
					++count;
				}
			}
		}
		return count;
	}

	// Set the current user
	void setCurrentUser(std::shared_ptr<User> user)
	{
		currentUser = user;
	}

	// Get the current user
	const std::shared_ptr<User> &getCurrentUser() const
	{
		return currentUser;
	}

	// Retrieve a list of Admins based on a search query
	std::vector<std::pair<std::string, std::string>> getAdmins(const std::string &query)
	{
		std::vector<std::pair<std::shared_ptr<User>, std::string>> tempRes;
		std::string filteredQuery = query.empty() ? "" : toLower(trim(query));

		// Iterate through user records and filter for Admins
		for (const auto &pair : userMap)
		{
			const std::shared_ptr<User> &userPtr = pair.second;
			if (userPtr && userPtr->role == Role::Admin)
			{
				// Check if the query matches full name, ID, or username
				if (filteredQuery.empty() ||
					toLower(userPtr->getFullName()).find(filteredQuery) != std::string::npos ||
					toLower(userPtr->getId()).find(filteredQuery) != std::string::npos ||
					toLower(userPtr->getUsername()).find(filteredQuery) != std::string::npos)
				{
					tempRes.push_back({userPtr, userPtr->getId()});
				}
			}
		}

		// Sort by createdAt in descending order (newest first)
		std::sort(tempRes.begin(), tempRes.end(), [](const auto &a, const auto &b)
				  { return a.first->createdAt > b.first->createdAt; });

		// Store results as pairs of (fullName, userId)
		std::vector<std::pair<std::string, std::string>> res;
		for (const auto &entry : tempRes)
		{
			res.push_back({entry.first->getFullName(), entry.second});
		}
		return res;
	}

	// Retrieve a list of Patients based on a search query
	std::vector<std::pair<std::string, std::string>> getPatients(const std::string &query)
	{
		std::vector<std::pair<std::shared_ptr<User>, std::string>> tempRes;
		std::string filteredQuery = query.empty() ? "" : toLower(trim(query));

		// Iterate through user records and filter for Patients
		for (const auto &pair : userMap)
		{
			const std::shared_ptr<User> &userPtr = pair.second;
			if (userPtr && userPtr->role == Role::Patient)
			{
				// Check if the query matches full name, ID, or username
				if (filteredQuery.empty() ||
					toLower(userPtr->getFullName()).find(filteredQuery) != std::string::npos ||
					toLower(userPtr->getId()).find(filteredQuery) != std::string::npos ||
					toLower(userPtr->getUsername()).find(filteredQuery) != std::string::npos)
				{
					tempRes.push_back({userPtr, userPtr->getId()});
				}
			}
		}

		// Sort by createdAt in descending order (newest first)
		std::sort(tempRes.begin(), tempRes.end(), [](const auto &a, const auto &b)
				  { return a.first->createdAt > b.first->createdAt; });

		// Store results as pairs of (fullName, userId)
		std::vector<std::pair<std::string, std::string>> res;
		for (const auto &entry : tempRes)
		{
			res.push_back({entry.first->getFullName(), entry.second});
		}
		return res;
	}
};

#endif // USER_MANAGER_H
