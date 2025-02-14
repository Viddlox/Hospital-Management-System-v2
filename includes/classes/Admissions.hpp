#ifndef ADMISSIONS_H
#define ADMISSIONS_H

#include <string>
#include "json.hpp"

using json = nlohmann::json;

struct Admissions
{
	enum class Department
	{
		Emergency,			// Immediate and urgent care
		InternalMedicine,	// General adult healthcare & chronic conditions
		OBGYN,				// Obstetrics & gynecology
		Pediatrics,			// Infants, children, and adolescents
		Surgery,			// General and specialized surgical procedures
		Cardiology,			// Heart and vascular diseases
		Neurology,			// Brain, spinal cord, and nerve disorders
		Oncology,			// Cancer treatment
		Orthopedics,		// Bone, joint, and muscle conditions
		Pulmonology,		// Respiratory and lung disorders
		Psychiatry,			// Mental health treatment
		Nephrology,			// Kidney diseases and dialysis
		Gastroenterology,	// Digestive system disorders
		InfectiousDiseases, // Bacterial, viral, and fungal infections
		Endocrinology,		// Hormonal and metabolic disorders (e.g., diabetes)
		Urology,			// Urinary tract and male reproductive health
		Dermatology,		// Skin conditions and diseases
		Rheumatology,		// Autoimmune and inflammatory disorders (e.g., arthritis)
		ENT,				// Ear, Nose, and Throat (Otolaryngology)
		Ophthalmology,		// Eye care and vision treatment
		PhysicalRehab		// Rehabilitation for injuries, disabilities, post-surgery
	};

	std::string dateTime;
	Department department;

	Admissions(const std::string &dt, const Department dept)
		: dateTime(dt), department(dept) {}

	Admissions() = default; // Default constructor required for JSON deserialization

	static std::string departmentToString(Department dept);
	static Department stringToDepartment(const std::string &deptStr);
};

// Declare JSON serialization functions
void to_json(json &j, const Admissions &a);
void from_json(const json &j, Admissions &a);

#endif