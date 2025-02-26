#include "admissions.hpp"
#include <unordered_map>

// 🔹 Converts Department enum to string
std::string Admissions::departmentToString(Department dept)
{
	switch (dept)
	{
	case Department::Emergency:
		return "Emergency";
	case Department::InternalMedicine:
		return "Internal Medicine";
	case Department::OBGYN:
		return "OBGYN";
	case Department::Pediatrics:
		return "Pediatrics";
	case Department::Surgery:
		return "Surgery";
	case Department::Cardiology:
		return "Cardiology";
	case Department::Neurology:
		return "Neurology";
	case Department::Oncology:
		return "Oncology";
	case Department::Orthopedics:
		return "Orthopedics";
	case Department::Pulmonology:
		return "Pulmonology";
	case Department::Psychiatry:
		return "Psychiatry";
	case Department::Nephrology:
		return "Nephrology";
	case Department::Gastroenterology:
		return "Gastroenterology";
	case Department::InfectiousDiseases:
		return "Infectious Diseases";
	case Department::Endocrinology:
		return "Endocrinology";
	case Department::Urology:
		return "Urology";
	case Department::Dermatology:
		return "Dermatology";
	case Department::Rheumatology:
		return "Rheumatology";
	case Department::ENT:
		return "ENT";
	case Department::Ophthalmology:
		return "Ophthalmology";
	case Department::PhysicalRehab:
		return "Physical Rehab";
	default:
		return "Unknown";
	}
}

// 🔹 Converts string to Department enum
Admissions::Department Admissions::stringToDepartment(const std::string &deptStr)
{
	static const std::unordered_map<std::string, Department> departmentMap = {
		{"Emergency", Department::Emergency},
		{"Internal Medicine", Department::InternalMedicine},
		{"OBGYN", Department::OBGYN},
		{"Pediatrics", Department::Pediatrics},
		{"Surgery", Department::Surgery},
		{"Cardiology", Department::Cardiology},
		{"Neurology", Department::Neurology},
		{"Oncology", Department::Oncology},
		{"Orthopedics", Department::Orthopedics},
		{"Pulmonology", Department::Pulmonology},
		{"Psychiatry", Department::Psychiatry},
		{"Nephrology", Department::Nephrology},
		{"Gastroenterology", Department::Gastroenterology},
		{"Infectious Diseases", Department::InfectiousDiseases},
		{"Endocrinology", Department::Endocrinology},
		{"Urology", Department::Urology},
		{"Dermatology", Department::Dermatology},
		{"Rheumatology", Department::Rheumatology},
		{"ENT", Department::ENT},
		{"Ophthalmology", Department::Ophthalmology},
		{"Physical Rehab", Department::PhysicalRehab}};

	auto it = departmentMap.find(deptStr);
	if (it != departmentMap.end())
		return it->second;

	throw std::invalid_argument("Invalid department string: " + deptStr);
}

// 🔹 JSON Serialization for Admissions
void to_json(json &j, const Admissions &a)
{
	j = json{
		{"dateTime", a.dateTime},
		{"department", Admissions::departmentToString(a.department)}};
}

// 🔹 JSON Deserialization for Admissions
void from_json(const json &j, Admissions &a)
{
	a.dateTime = j.at("dateTime").get<std::string>();
	a.department = Admissions::stringToDepartment(j.at("department").get<std::string>());
}
