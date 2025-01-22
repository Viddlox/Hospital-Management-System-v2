#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <ctime>
#include <atomic>

#include "render.hpp"

enum class Screen
{
	Login,
	Dashboard,
	Roster,
	Userbase
};

class EventManager
{
private:
	std::mutex mtx;
	std::atomic<bool> isRunning{false};
	std::condition_variable cv;
	std::queue<std::string> notifications;
	std::thread notificationThread;
	Screen screen;
	std::time_t time;
	

	void handleNotifications()
	{
		while (isRunning.load())
		{
			std::unique_lock<std::mutex> lock(mtx);

			cv.wait(lock, [this]
					{ return !notifications.empty() || !isRunning.load(); });

			while (!notifications.empty())
			{
				std::string notification = notifications.front();
				notifications.pop();
				std::cout << "[Notification] " << notification << std::endl;

				std::this_thread::sleep_for(std::chrono::seconds(2));
			}
		}
	}

	void renderLayout()
	{
		renderHeader();
		switch (screen)
		{
		case Screen::Login:
			break;
		case Screen::Dashboard:
			break;
		case Screen::Roster:
			break;
		case Screen::Userbase:
			break;
		default:
			break;
		}
	}

public:
	EventManager() : screen(Screen::Login), time(getCurrentTime()) {}
	~EventManager()
	{
		stopThreads();
	}

	void stopThreads()
	{
		if (notificationThread.joinable())
		{
			notificationThread.join();
		}
	}

	void switchScreen(Screen newScreen)
	{
		screen = newScreen;
		renderLayout();
	}

	std::time_t getCurrentTime()
	{
		auto now = std::chrono::system_clock::now();
		return std::chrono::system_clock::to_time_t(now);
	}

	void start()
	{
		isRunning.store(true);
		notificationThread = std::thread(&EventManager::handleNotifications, this);
		clearScreen();
		renderLayout();

		while (isRunning.load())
		{
			time = getCurrentTime();
			renderTime(time);
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}

	void exit()
	{
		{
			std::lock_guard<std::mutex> lock(mtx);
			isRunning.store(false);
		}
		cv.notify_one();
		stopThreads();
	}
};

#endif
