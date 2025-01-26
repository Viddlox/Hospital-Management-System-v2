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
    if (EventManager::instance)
    {
        EventManager::instance->exit();
    }
    exit(signum);
}

int main()
{
    signal(SIGINT, signalHandler);

    try
    {
        EventManager eventManager;
        EventManager::instance = &eventManager;
        eventManager.start();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        if (EventManager::instance)
        {
            EventManager::instance->exit();
        }
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
