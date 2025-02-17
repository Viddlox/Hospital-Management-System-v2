#ifndef PATIENT_H
#define PATIENT_H

// Includes standard libraries and project-specific dependencies

#include <unordered_map>  // Used to store patient attributes efficiently
#include <map>            // Used for the admissions log (department -> list of dates)
#include "Utils.hpp"      // Provides utility functions such as timestamp formatting
#include "User.hpp"       // Base class for all users (Patient inherits from User)
#include "Admissions.hpp" // Handles department-based admissions and their string conversions

class Patient : public User
{
public:
    // Personal information
    int age;                        // Patient's age
    std::string religion;           // Religion of the patient
    std::string nationality;        // Nationality of the patient
    std::string identityCardNumber; // Unique identity card number
    std::string maritalStatus;      // Marital status (Single, Married, etc.)
    std::string gender;             // Gender of the patient
    std::string race;               // Race of the patient

    // Contact information
    std::string emergencyContactNumber; // Emergency contact number
    std::string emergencyContactName;   // Name of the emergency contact person
    std::string address;                // Residential address

    // Medical information
    double bmi;         // Body Mass Index
    std::string height; // Patient's height in cm or inches
    std::string weight; // Patient's weight in kg or lbs

    // Admissions log (Department -> List of admission dates)
    std::map<Admissions::Department, std::vector<std::string>> admissions;

    // Default constructor initializes age and BMI to default values
    Patient() : User(), age(0), bmi(0) {}

    /**
     * Parameterized constructor to initialize all fields of the Patient class
     */
    Patient(
        const std::string &username, const std::string &password, int age, const std::string &fullName,
        const std::string &religion, const std::string &nationality, const std::string &identityCardNumber,
        const std::string &maritalStatus, const std::string &gender, const std::string &race, const std::string &email,
        const std::string &contactNumber, const std::string &emergencyContactNumber, const std::string &emergencyContactName,
        const std::string &address, double bmi, const std::string &height, const std::string &weight, Admissions::Department dept)
        : User(username, password, fullName, email, contactNumber, Role::Patient),
          age(age), religion(religion), nationality(nationality),
          identityCardNumber(identityCardNumber), maritalStatus(maritalStatus), gender(gender), race(race),
          emergencyContactNumber(emergencyContactNumber), emergencyContactName(emergencyContactName),
          address(address), bmi(bmi), height(height), weight(weight)
    {
        addAdmission(dept); // Add initial admission record
    }

    ~Patient() = default;

    /**
     * Adds an admission record for a specific department.
     * Records the current timestamp and saves the updated information.
     */
    void addAdmission(Admissions::Department dept)
    {
        std::string dateTime = formatTimestamp(std::chrono::system_clock::now());
        admissions[dept].push_back(dateTime);
        saveToFile();
    }

    /**
     * Deletes an admission record from a specific department if it exists.
     */
    void deleteAdmission(Admissions::Department dept, const std::string &dateTime)
    {
        auto &dates = admissions[dept];
        auto it = std::find(dates.begin(), dates.end(), dateTime);
        if (it != dates.end())
        {
            dates.erase(it);

            // If no more admissions remain for this department, remove the department from the map
            if (dates.empty())
            {
                admissions.erase(dept);
            }
            saveToFile();
        }
    }

    /**
     * Serializes a Patient object to JSON format.
     */
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
            {"admissions", json::object()}};

        for (const auto &[dept, dates] : p.admissions)
        {
            j["admissions"][Admissions::departmentToString(dept)] = dates;
        }
    }

    /**
     * Deserializes JSON data into a Patient object.
     */
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

        // Deserialize admissions map and sort by createdAt
        if (j.contains("admissions"))
        {
            for (const auto &[deptStr, dateList] : j["admissions"].items())
            {
                Admissions::Department dept = Admissions::stringToDepartment(deptStr);
                p.admissions[dept] = dateList.get<std::vector<std::string>>();
            }
        }

        // Deserialize createdAt
        std::string createdAtStr = j.at("createdAt").get<std::string>();
        std::tm tm = {};
        std::istringstream ss(createdAtStr);
        ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");

        if (ss.fail())
        {
            throw std::invalid_argument("Invalid date format for createdAt");
        }

        std::time_t time = std::mktime(&tm);
        p.createdAt = std::chrono::system_clock::from_time_t(time);
    }

    /**
     * Saves the Patient object to a JSON file for persistent storage.
     */
    void saveToFile() const
    {
        std::filesystem::create_directories("db/patient");
        std::string filePath = "db/patient/" + id + ".json";
        json j = *this;

        std::ofstream file(filePath);
        if (file.is_open())
        {
            file << j.dump(4);
            file.close();
        }
        else
        {
            std::cerr << "Error: Could not open file to save patient data." << std::endl;
        }
    }
};

#endif
