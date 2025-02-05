#include "appointment.hpp"

// Helper function to convert Status to string
std::string Appointment::statusToString(Status status)
{
    switch (status)
    {
    case Status::scheduled: return "scheduled";
    case Status::completed: return "completed";
    case Status::cancelled: return "cancelled";
    default: return "unknown";
    }
}

// Helper function to convert string to Status
Appointment::Status Appointment::stringToStatus(const std::string &statusStr)
{
    if (statusStr == "scheduled") return Status::scheduled;
    if (statusStr == "completed") return Status::completed;
    if (statusStr == "cancelled") return Status::cancelled;
    throw std::invalid_argument("Invalid status: " + statusStr);
}

// JSON Serialization for Appointment
void to_json(json &j, const Appointment &a)
{
    j = json{{"dateTime", a.dateTime},
             {"department", a.department},
             {"status", Appointment::statusToString(a.status)}};
}

// JSON Deserialization for Appointment
void from_json(const json &j, Appointment &a)
{
    a.dateTime = j.at("dateTime").get<std::string>();
    a.department = j.at("department").get<std::string>();
    a.status = Appointment::stringToStatus(j.at("status").get<std::string>());
}
