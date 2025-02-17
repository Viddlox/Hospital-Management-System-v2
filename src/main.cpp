#include "UserManager.hpp"
#include "EventManager.hpp"

/**
 * @brief Atomic flag to prevent multiple cleanup executions.
 */
std::atomic<bool> isCleaningUp{false};

/**
 * @brief Signal handler for handling termination signals (e.g., SIGINT).
 * 
 * @param signum The signal number received.
 */
void signalHandler(int signum)
{
    // Prevent multiple executions of cleanup logic
    if (isCleaningUp.load())
    {
        return;
    }
    isCleaningUp.store(true);

    // Get the singleton instance of EventManager and trigger cleanup
    EventManager &eventManager = EventManager::getInstance();
    eventManager.exit();

    // Exit the application with the received signal number
    exit(signum);
}

/**
 * @brief Main function to initialize and run the event-driven system.
 * 
 * @return int Returns EXIT_SUCCESS on normal execution, EXIT_FAILURE on exceptions.
 */
int main()
{
    // Register signal handler for SIGINT (Ctrl + C)
    signal(SIGINT, signalHandler);

    try
    {
        // Get the singleton instance of EventManager
        EventManager &eventManager = EventManager::getInstance();

        // Start the event loop
        eventManager.start();
    }
    catch (const std::exception &e)
    {
        // Log error and return failure code
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    // Return success code if everything runs smoothly
    return EXIT_SUCCESS;
}
