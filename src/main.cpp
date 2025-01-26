#include "UserManager.hpp"
#include "EventManager.hpp"

std::atomic<bool> isCleaningUp{false};

void signalHandler(int signum)
{
    if (isCleaningUp.load())
    {
        return;
    }
    isCleaningUp.store(true);
    EventManager &eventManager = EventManager::getInstance();
    eventManager.exit();
    exit(signum);
}

int main()
{
    signal(SIGINT, signalHandler);
    try
    {
        EventManager& eventManager = EventManager::getInstance();
        eventManager.start();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
