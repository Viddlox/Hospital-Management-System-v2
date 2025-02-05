#ifndef APPOINTMENT_HPP
#define APPOINTMENT_HPP

#include <string>
#include "json.hpp"

using json = nlohmann::json;

struct Appointment
{
    std::string dateTime;
    std::string department;
    
    enum class Status { scheduled, completed, cancelled };
    Status status;

    Appointment(const std::string &dt, const std::string &dept, Status s)
        : dateTime(dt), department(dept), status(s) {}

    Appointment() = default; // Default constructor required for JSON deserialization

    static std::string statusToString(Status status);
    static Status stringToStatus(const std::string &statusStr);
};

// Declare JSON serialization functions
void to_json(json &j, const Appointment &a);
void from_json(const json &j, Appointment &a);

#endif // APPOINTMENT_HPP
