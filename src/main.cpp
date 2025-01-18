#include "UserManager.hpp"
#include "Patient.hpp"
#include <cstdio>  // For printf


int main()
{
    auto userManager = std::make_shared<UserManager>();

    // Create a patient
    // userManager->createPatient("johndoe", "password123", 30, "John", "Doe", "Christianity", "American", "ID12345",
    //                            "Single", "Male", "Caucasian", "john@example.com", "1234567890", "0987654321",
    //                            "Jane Doe", "123 Main St", 25, "180cm", "75kg", "None", "Healthy", "No history");
    // // // Create an admin
    // userManager->createAdmin("admin", "1234");

    // auto user = userManager->getUserById("aca9995d-6681-4079-adbc-8eca248e7c81");
    // if (user)
    // {
    //     printf("User ID: %s\n", user->getId().c_str());
    //     printf("Role: %s\n", user->getRoleToString(user->getRole()).c_str());
    //     printf("Created At: %s\n", user->getCreatedAt().c_str());
    // }
    // auto patient = userManager->getUserById("f66af9f8-ae37-4dc8-bf96-29130dad6dba");
    // if (patient)
    // {
    //     printf("User ID: %s\n", patient->getId().c_str());
    //     printf("Role: %s\n", patient->getRoleToString(patient->getRole()).c_str());
    //     printf("Created At: %s\n", patient->getCreatedAt().c_str());
    // }

    // Delete a user
    // userManager->deleteUserById("aca9995d-6681-4079-adbc-8eca248e7c81");

    // Update user record
    // userManager->updateUser("f66af9f8-ae37-4dc8-bf96-29130dad6dba", "firstName", "some random bloke");

    return 0;
}