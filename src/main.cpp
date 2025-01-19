#include "UserManager.hpp"
#include "Patient.hpp"
#include <cstdio>  // For printf


int main()
{
    auto userManager = std::make_shared<UserManager>();

    // // Create a patient
    // userManager->createPatient("johndoe", "password123", 30, "John", "Doe", "Christianity", "American", "ID12345",
    //                            "Single", "Male", "Caucasian", "john@example.com", "1234567890", "0987654321",
    //                            "Jane Doe", "123 Main St", 25, "180cm", "75kg", "None", "Healthy", "No history");
    // // // Create an admin
    // userManager->createAdmin("admin", "1234");

    // // Get/Search user
    // auto user = userManager->getUserById("fa7bacec-38cc-4126-8599-2cb1294ff48b");
    // if (user)
    // {
    //     printf("User ID: %s\n", user->getId().c_str());
    //     printf("Role: %s\n", user->getRoleToString(user->getRole()).c_str());
    //     printf("Created At: %s\n", user->getCreatedAt().c_str());
    // }
    // auto patient = userManager->getUserById("0f069dcc-2fa4-4974-9e68-87ca5fc7c669");
    // if (patient)
    // {
    //     printf("User ID: %s\n", patient->getId().c_str());
    //     printf("Role: %s\n", patient->getRoleToString(patient->getRole()).c_str());
    //     printf("Created At: %s\n", patient->getCreatedAt().c_str());
    // }

    // // Delete a user
    // userManager->deleteUserById("0f069dcc-2fa4-4974-9e68-87ca5fc7c669");

    // // Update user record
    // userManager->updateUser("fa7bacec-38cc-4126-8599-2cb1294ff48b", "username", "some random bloke");

    return 0;
}