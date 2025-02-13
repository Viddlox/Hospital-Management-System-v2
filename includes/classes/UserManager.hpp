#ifndef USER_MANAGER_H
#define USER_MANAGER_H

#include <unordered_map>
#include <memory>
#include <functional>

#include "User.hpp"
#include "Admin.hpp"
#include "Patient.hpp"
#include "utils.hpp"

class UserManager
{
private:
	// in-memory storage of users as shared pointers for O(1) read/writes
	std::unordered_map<std::string, std::shared_ptr<User>> userMap;
	std::shared_ptr<User> currentUser;

	UserManager()
	{
		populateUserMap();
	}
	~UserManager() = default;
	UserManager(const UserManager &) = delete;
	UserManager &operator=(const UserManager &) = delete;

	std::shared_ptr<User> getUserFromFile(const std::string &userId, const std::string &role)
	{
		std::shared_ptr<User> user = nullptr;
		std::string filePath = "db/" + role + "/" + userId + ".json";

		if (std::filesystem::exists(filePath))
		{
			// Read from file and create user object
			std::ifstream file(filePath);
			if (file.is_open())
			{
				nlohmann::json j;
				file >> j;
				file.close();

				if (role == "admin")
				{
					user = std::make_shared<Admin>();
					from_json(j, *std::dynamic_pointer_cast<Admin>(user));
					userMap[userId] = std::dynamic_pointer_cast<Admin>(user);
				}
				else if (role == "patient")
				{
					user = std::make_shared<Patient>();
					from_json(j, *std::dynamic_pointer_cast<Patient>(user));
					userMap[userId] = std::dynamic_pointer_cast<Patient>(user);
				}
			}
		}
		return user;
	}
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
							userMap[j["id"]] = std::dynamic_pointer_cast<Admin>(user);
						}
						else if (roleStr == "patient")
						{
							user = std::make_shared<Patient>();
							from_json(j, *std::dynamic_pointer_cast<Patient>(user));
							userMap[j["id"]] = std::dynamic_pointer_cast<Patient>(user);
						}
					}
				}
			}
		}
	}

public:
	// get singleton instance
	static UserManager &getInstance()
	{
		static UserManager instance;
		return instance;
	}

	// Add record (Patient)
	void createPatient(const std::string &username, const std::string &password, int age, const std::string &fullName,
					   const std::string &religion, const std::string &nationality,
					   const std::string &identityCardNumber, const std::string &maritalStatus, const std::string &gender,
					   const std::string &race, const std::string &email, const std::string &contactNumber,
					   const std::string &emergencyContactNumber, const std::string &emergencyContactName, const std::string &address,
					   double bmi, const std::string &height, const std::string &weight)
	{
		std::shared_ptr<Patient> newPatient = std::make_shared<Patient>(
			username, password, age, fullName, religion, nationality, identityCardNumber,
			maritalStatus, gender, race, email, contactNumber, emergencyContactNumber, emergencyContactName,
			address, bmi, height, weight);

		auto result = userMap.insert({newPatient->getId(), newPatient});
		if (!result.second)
		{
			std::cout << "Patient with ID " << newPatient->getId() << " already exists.\n";
			std::cout << "Patient with username " << newPatient->getUsername() << " already exists.\n";
			return;
		}

		newPatient->saveToFile();
	}

	// Add record (Admin)
	void createAdmin(const std::string &username, const std::string &password, const std::string &fullName, const std::string &email, const std::string &contactNumber)
	{
		std::shared_ptr<Admin> newAdmin = std::make_shared<Admin>(username, password, fullName, email, contactNumber);

		auto result = userMap.insert({newAdmin->getId(), newAdmin});
		if (!result.second)
		{
			std::cout << "Admin with ID " << newAdmin->getId() << " already exists.\n";
			std::cout << "Admin with username " << newAdmin->getUsername() << " already exists.\n";
			return;
		}
		newAdmin->saveToFile();
	}

	// Search record
	std::shared_ptr<User> getUserById(const std::string &userId)
	{
		// Check in-memory cache first
		auto it = userMap.find(userId);
		if (it != userMap.end())
		{
			return it->second;
		}

		// If not found in memory, check the folder-based database (by role)
		std::shared_ptr<User> user = nullptr;
		user = getUserFromFile(userId, "admin");
		if (user)
		{
			userMap[userId] = user;
			return user;
		}

		user = getUserFromFile(userId, "patient");
		if (user)
		{
			userMap[userId] = user;
			return user;
		}
		return nullptr;
	}

	// Search record by username
	std::shared_ptr<User> getUserByUsername(const std::string &username)
	{
		std::string trimmedUsername = toLower(trim(username)); // Trim input username

		// Check in-memory cache first
		for (const auto &pair : userMap)
		{
			if (toLower(trim(pair.second->getUsername())) == trimmedUsername)
			{
				return pair.second;
			}
		}

		// If not found in memory, check the folder-based database (by role)
		std::vector<Role> roles = {Role::Admin, Role::Patient, Role::User};
		for (const auto &role : roles)
		{
			std::string roleStr = User::getRoleToString(role);
			std::string filePath = "db/" + roleStr + "/";

			try
			{
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
		return nullptr;
	}

	// Search record by name
	std::shared_ptr<User> getUserByName(const std::string &fullName)
	{
		std::string trimmedName = toLower(trim(fullName));

		// Check in-memory cache first
		for (const auto &pair : userMap)
		{
			if (toLower(trim(pair.second->getFullName())) == trimmedName)
			{
				return pair.second;
			}
		}

		// Check the folder-based database
		std::vector<Role> roles = {Role::Admin, Role::Patient, Role::User};
		for (const auto &role : roles)
		{
			std::string roleStr = User::getRoleToString(role);
			std::string filePath = "db/" + roleStr + "/";

			try
			{
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
		return nullptr;
	}

	// Delete record
	void deleteUserById(const std::string &userId)
	{
		auto it = userMap.find(userId);
		if (it != userMap.end())
		{
			userMap.erase(it);
		}
		// If not found in memory, check the folder-based database (by role)
		bool isDeleted = deleteUserFromFile(userId);
		if (!isDeleted)
		{
			std::cout << "User with ID " << userId << " not found.\n";
		}
	}

	// Update Record
	void updateUser(const std::string &userId, const std::string &fieldName, const std::string &newValue)
	{
		auto user = getUserById(userId);
		if (!user)
		{
			std::cerr << "User with ID " << userId << " not found.\n";
			return;
		}
		Role role = user->role;

		if (role == Role::Admin)
		{
			auto admin = std::dynamic_pointer_cast<Admin>(user);
			if (!admin)
			{
				std::cerr << "Failed to cast User to Admin for user ID " << userId << ".\n";
				return;
			}

			static const std::unordered_map<std::string, std::function<void(const std::string &)>> adminUpdates = {
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

			auto it = adminUpdates.find(fieldName);
			if (it != adminUpdates.end())
			{
				it->second(newValue);
				admin->saveToFile();
				std::cout << "Admin with ID " << userId << " updated successfully. Field: " << fieldName << "\n";
			}
			else
			{
				std::cerr << "Field '" << fieldName << "' is not valid for Admin.\n";
			}
			return;
		}

		// Handle Patient updates
		if (role == Role::Patient)
		{
			auto patient = std::dynamic_pointer_cast<Patient>(user);
			if (!patient)
			{
				std::cerr << "Failed to cast User to Patient for user ID " << userId << ".\n";
				return;
			}

			static const std::unordered_map<std::string, std::function<void(const std::string &)>> patientUpdates = {
				// Personal Information
				{"age", [&patient](const std::string &value)
				 {
					 try
					 {
						 patient->age = std::stoi(value);
					 }
					 catch (const std::exception &e)
					 {
						 std::cerr << "Invalid value for 'age': " << value << "\n";
					 }
				 }},
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

				// Medical Information
				{"bmi", [&patient](const std::string &value)
				 {
					 try
					 {
						 patient->bmi = std::stod(value);
					 }
					 catch (const std::exception &e)
					 {
						 std::cerr << "Invalid value for 'bmi': " << value << "\n";
					 }
				 }},
				{"height", [&patient](const std::string &value)
				 { patient->height = value; }},
				{"weight", [&patient](const std::string &value)
				 { patient->weight = value; }}};

			auto it = patientUpdates.find(fieldName);
			if (it != patientUpdates.end())
			{
				it->second(newValue);
				patient->saveToFile();
				std::cout << "Patient with ID " << userId << " updated successfully. Field: " << fieldName << "\n";
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
	bool validateUser(const std::string &username, const std::string &password)
	{
		auto user = getUserByUsername(username);
		if (user && user->getRole() == Role::Admin && user->getPassword() == password)
		{
			setCurrentUser(user);
			return true;
		}
		return false;
	}
	int getAdminCount()
	{
		std::string filePath = "db/admin/";
		int count = 0;

		if (std::filesystem::exists(filePath) && std::filesystem::is_directory(filePath))
		{
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
	int getPatientCount()
	{
		std::string filePath = "db/patient/";
		int count = 0;

		if (std::filesystem::exists(filePath) && std::filesystem::is_directory(filePath))
		{
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
	void setCurrentUser(std::shared_ptr<User> user)
	{
		currentUser = user;
	}
	const std::shared_ptr<User> &getCurrentUser() const
	{
		return currentUser;
	}
	std::vector<std::pair<std::string, std::string>> getAdmins(const std::string &query)
	{
		std::vector<std::pair<std::string, std::string>> res;
		std::string filteredQuery = query.empty() ? "" : toLower(trim(query));

		for (const auto &pair : userMap)
		{
			const std::shared_ptr<User> &userPtr = pair.second;
			if (userPtr && userPtr->role == Role::Admin)
			{
				if (filteredQuery.empty() ||
					toLower(userPtr->getFullName()).find(filteredQuery) != std::string::npos ||
					toLower(userPtr->getId()).find(filteredQuery) != std::string::npos ||
					toLower(userPtr->getUsername()).find(filteredQuery) != std::string::npos)

				{
					res.push_back({userPtr->getFullName(), userPtr->getId()});
				}
			}
		}
		return res;
	}

	std::vector<std::pair<std::string, std::string>> getPatients(const std::string &query)
	{
		std::vector<std::pair<std::string, std::string>> res;
		std::string filteredQuery = query.empty() ? "" : toLower(trim(query));

		for (const auto &pair : userMap)
		{
			const std::shared_ptr<User> &userPtr = pair.second;
			if (userPtr && userPtr->role == Role::Patient)
			{
				if (filteredQuery.empty() ||
					toLower(userPtr->getFullName()).find(filteredQuery) != std::string::npos ||
					toLower(userPtr->getId()).find(filteredQuery) != std::string::npos ||
					toLower(userPtr->getUsername()).find(filteredQuery) != std::string::npos)
				{
					res.push_back({userPtr->getFullName(), userPtr->getId()});
				}
			}
		}
		return res;
	}
};

#endif
