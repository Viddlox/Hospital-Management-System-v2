#ifndef USER_MANAGER_H
#define USER_MANAGER_H

#include <unordered_map>
#include <memory>
#include <functional>

#include "User.hpp"
#include "Admin.hpp"
#include "Patient.hpp"

class UserManager
{
private:
	// in-memory storage of users as shared pointers for O(1) read/writes
	std::unordered_map<std::string, std::shared_ptr<User>> userMap;
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
		for (int i = 0; i <= static_cast<int>(Role::Patient); i++)
		{
			Role role = static_cast<Role>(i);
			std::string roleStr = User::getRoleToString(role);
			std::string filePath = "db/" + roleStr + "/" + userId + ".json";

			if (fs::exists(filePath))
			{
				try
				{
					fs::remove(filePath);
					std::cout << "Deleted user file: " << filePath << std::endl;
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

public:
	// Add record (Patient)
	void createPatient(const std::string &username, const std::string &password, int age, const std::string &firstName,
					   const std::string &lastName, const std::string &religion, const std::string &nationality,
					   const std::string &identityCardNumber, const std::string &maritalStatus, const std::string &gender,
					   const std::string &race, const std::string &email, const std::string &contactNumber,
					   const std::string &emergencyContactNumber, const std::string &emergencyContactName, const std::string &address,
					   int bmi, const std::string &height, const std::string &weight, const std::string &knownAllergies,
					   const std::string &medicalHistory, const std::string &familyMedicalHistory,
					   const std::vector<std::string> &appointments = {})
	{
		std::shared_ptr<Patient> newPatient = std::make_shared<Patient>(
			username, password, age, firstName, lastName, religion, nationality, identityCardNumber,
			maritalStatus, gender, race, email, contactNumber, emergencyContactNumber, emergencyContactName,
			address, bmi, height, weight, knownAllergies, medicalHistory, familyMedicalHistory, appointments);

		auto result = userMap.insert({newPatient->getId(), newPatient});
		if (!result.second)
		{
			std::cout << "Patient with ID " << newPatient->getId() << " already exists.\n";
			return;
		}

		newPatient->saveToFile();
		std::cout << "Patient added successfully.\n";
	}

	// Add record (Admin)
	void createAdmin(const std::string &username, const std::string &password)
	{
		std::shared_ptr<Admin> newAdmin = std::make_shared<Admin>(username, password);

		auto result = userMap.insert({newAdmin->getId(), newAdmin});
		if (!result.second)
		{
			std::cout << "Admin with ID " << newAdmin->getId() << " already exists.\n";
			return;
		}
		newAdmin->saveToFile();
		std::cout << "Admin added successfully.\n";
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

		std::cout << "User with ID " << userId << " not found.\n";
		return nullptr;
	}

	// Search record by username
	std::shared_ptr<User> getUserByUsername(const std::string &username)
	{
		// Check in-memory cache first
		for (const auto &pair : userMap)
		{
			if (pair.second->getUsername() == username)
			{
				return pair.second;
			}
		}
		// If not found in memory, check the folder-based database (by role)
		for (int i = 0; i <= static_cast<int>(Role::Admin); i++)
		{
			Role role = static_cast<Role>(i);
			std::string roleStr = User::getRoleToString(role);
			std::string filePath = "db/" + roleStr + "/";
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

						if (j.contains("username") && j["username"] == username)
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
		if (isDeleted)
		{
			std::cout << "User with ID " << userId << " deleted successfully.\n";
		}
		else if (!isDeleted)
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
				{"firstName", [&patient](const std::string &value)
				 { patient->firstName = value; }},
				{"lastName", [&patient](const std::string &value)
				 { patient->lastName = value; }},
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
						 patient->bmi = std::stoi(value);
					 }
					 catch (const std::exception &e)
					 {
						 std::cerr << "Invalid value for 'bmi': " << value << "\n";
					 }
				 }},
				{"height", [&patient](const std::string &value)
				 { patient->height = value; }},
				{"weight", [&patient](const std::string &value)
				 { patient->weight = value; }},
				{"knownAllergies", [&patient](const std::string &value)
				 { patient->knownAllergies = value; }},
				{"medicalHistory", [&patient](const std::string &value)
				 { patient->medicalHistory = value; }},
				{"familyMedicalHistory", [&patient](const std::string &value)
				 { patient->familyMedicalHistory = value; }},
				{"appointments", [&patient](const std::string &value)
				 {
					 patient->appointments = split(value, ',');
				 }},
			};

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
		return user && user->getPassword() == password;
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
};

#endif
