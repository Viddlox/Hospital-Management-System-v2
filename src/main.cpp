#include "UserManager.hpp"
#include "EventManager.hpp"

int main()
{
    auto userManager = std::make_shared<UserManager>();
    auto eventManager = std::make_shared<EventManager>();
    eventManager->start();
    return 0;
}
