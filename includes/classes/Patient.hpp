#ifndef PATIENT_H
#define PATIENT_H

#include <unordered_map>

#include "Utils.hpp"
#include "User.hpp"

class Patient : public User
{
public:
    // Personal information
    int age;
    std::string religion;
    std::string nationality;
    std::string identityCardNumber;
    std::string maritalStatus;
    std::string gender;
    std::string race;

    // Contact information
    std::string emergencyContactNumber;
    std::string emergencyContactName;
    std::string address;

    // Medical information
    double bmi;
    std::string height;
    std::string weight;

    // Default constructor
    Patient()
        : User(),
          age(0), religion(""), nationality(""),
          identityCardNumber(""), maritalStatus(""), gender(""), race(""), emergencyContactNumber(""), emergencyContactName(""),
          address(""), bmi(0), height(""), weight("") {}

    // Parameterized constructor
    Patient(
        const std::string &username, const std::string &password, int age, const std::string &fullName,
        const std::string &religion, const std::string &nationality, const std::string &identityCardNumber,
        const std::string &maritalStatus, const std::string &gender, const std::string &race, const std::string &email,
        const std::string &contactNumber, const std::string &emergencyContactNumber, const std::string &emergencyContactName,
        const std::string &address, double bmi, const std::string &height, const std::string &weight)
        : User(username, password, fullName, email, contactNumber, Role::Patient), // Call base class constructor
          age(age), religion(religion), nationality(nationality),
          identityCardNumber(identityCardNumber), maritalStatus(maritalStatus), gender(gender), race(race),
          emergencyContactNumber(emergencyContactNumber), emergencyContactName(emergencyContactName),
          address(address), bmi(bmi), height(height), weight(weight)
    {
    }

    ~Patient() = default;

    // Friend functions to handle JSON serialization/deserialization as JSON is an external class
    friend void to_json(json &j, const Patient &p)
    {
        j = json{
            {"id", p.id},
            {"role", p.getRoleToString(p.role)},
            {"username", p.username},
            {"password", p.password},
            {"createdAt", p.getCreatedAt()},
            {"age", p.age},
            {"fullName", p.fullName},
            {"religion", p.religion},
            {"nationality", p.nationality},
            {"identityCardNumber", p.identityCardNumber},
            {"maritalStatus", p.maritalStatus},
            {"gender", p.gender},
            {"race", p.race},
            {"contactNumber", p.contactNumber},
            {"emergencyContactNumber", p.emergencyContactNumber},
            {"emergencyContactName", p.emergencyContactName},
            {"email", p.email},
            {"address", p.address},
            {"bmi", p.bmi},
            {"height", p.height},
            {"weight", p.weight},
        };
    }

    friend void from_json(const json &j, Patient &p)
    {
        p.id = j.at("id").get<std::string>();
        p.role = p.getRoleToEnum(j.at("role").get<std::string>());
        p.username = j.at("username").get<std::string>();
        p.password = j.at("password").get<std::string>();
        p.age = j.at("age").get<int>();
        p.fullName = j.at("fullName").get<std::string>();
        p.religion = j.at("religion").get<std::string>();
        p.nationality = j.at("nationality").get<std::string>();
        p.identityCardNumber = j.at("identityCardNumber").get<std::string>();
        p.maritalStatus = j.at("maritalStatus").get<std::string>();
        p.gender = j.at("gender").get<std::string>();
        p.race = j.at("race").get<std::string>();
        p.contactNumber = j.at("contactNumber").get<std::string>();
        p.emergencyContactNumber = j.at("emergencyContactNumber").get<std::string>();
        p.emergencyContactName = j.at("emergencyContactName").get<std::string>();
        p.email = j.at("email").get<std::string>();
        p.address = j.at("address").get<std::string>();
        p.bmi = j.at("bmi").get<double>();
        p.height = j.at("height").get<std::string>();
        p.weight = j.at("weight").get<std::string>();

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
        p.createdAt = std::chrono::system_clock::from_time_t(time);
    }
    void saveToFile() const
    {
        // Ensure the folder exists
        std::filesystem::create_directories("db/patient");

        // Generate the file name using the patient ID
        std::string filePath = "db/patient/" + id + ".json";

        // Serialize the object to JSON
        json j = *this;

        // Write to the file
        std::ofstream file(filePath);
        if (file.is_open())
        {
            file << j.dump(4); // Pretty print with 4 spaces
            file.close();
        }
        else
        {
            std::cerr << "Error: Could not open file to save patient data." << std::endl;
        }
    }
};

#endif
