#ifndef ADMISSIONS_H
#define ADMISSIONS_H

#include <string>   // Provides support for std::string used in Admissions class  
#include "json.hpp" // Enables JSON serialization and deserialization for Admissions  

using json = nlohmann::json; // Aliases the JSON library namespace for convenience  

// The Admissions struct represents hospital admissions, including department and date/time.
struct Admissions
{
	enum class Department
	{
		Emergency,			// Immediate and urgent care
		InternalMedicine,	// General adult healthcare & chronic conditions
		OBGYN,				// Obstetrics & gynecology (pregnancy, childbirth, reproductive health)
		Pediatrics,			// Medical care for infants, children, and adolescents
		Surgery,			// General and specialized surgical procedures
		Cardiology,			// Diagnosis & treatment of heart and vascular diseases
		Neurology,			// Disorders of the brain, spinal cord, and nervous system
		Oncology,			// Cancer diagnosis and treatment
		Orthopedics,		// Conditions related to bones, joints, muscles, and ligaments
		Pulmonology,		// Lung and respiratory system disorders (e.g., asthma, COPD)
		Psychiatry,			// Mental health conditions and therapy
		Nephrology,			// Kidney function and diseases, including dialysis
		Gastroenterology,	// Digestive system conditions (stomach, intestines, liver)
		InfectiousDiseases, // Study and treatment of bacterial, viral, and fungal infections
		Endocrinology,		// Hormonal and metabolic disorders (e.g., diabetes, thyroid issues)
		Urology,			// Urinary tract health and male reproductive system
		Dermatology,		// Skin, hair, and nail diseases
		Rheumatology,		// Autoimmune and inflammatory conditions (e.g., arthritis, lupus)
		ENT,				// Ear, Nose, and Throat (Otolaryngology) conditions
		Ophthalmology,		// Eye care, vision issues, and eye diseases
		PhysicalRehab		// Rehabilitation therapy for injuries, disabilities, and post-surgery
	};

	std::string dateTime; // Stores the admission date and time  
	Department department; // Stores the assigned medical department  

	// Constructor to initialize an admission with a date and department  
	Admissions(const std::string &dt, const Department dept)
		: dateTime(dt), department(dept) {}

	Admissions() = default; // Default constructor required for JSON deserialization  

	// Converts a Department enum value to a human-readable string  
	static std::string departmentToString(Department dept);

	// Converts a department name string back into the Department enum  
	static Department stringToDepartment(const std::string &deptStr);
};

// Declare JSON serialization functions  
void to_json(json &j, const Admissions &a); // Converts an Admissions object to JSON  
void from_json(const json &j, Admissions &a); // Converts JSON data into an Admissions object  

#endif // ADMISSIONS_H
