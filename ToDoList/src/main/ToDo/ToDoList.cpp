#include "ToDoList.h"
#include <iomanip>
#include <time.h>
#include <algorithm>

ToDoList::ToDoList() : isWorking{ false }
{
	initThread();
	std::ifstream file("notifications", std::ios_base::in);
	if (!file.is_open())
		return;

	std::istringstream iss(std::ios_base::app | std::ios_base::in);
	std::string line, message, tmp;
	std::time_t file_time, period = 0;
	while (!file.eof())
	{
		iss.clear();
		file_time = 0, message = "", period = 0;
		std::getline(file, line, '\n');
		if (line.empty())
			continue;
		iss.str(line);
		iss >> file_time >> message >> period;
		std::chrono::time_point<std::chrono::system_clock> chrono_time{ };
		chrono_time = std::chrono::system_clock::from_time_t(file_time) ;
		addItem(ToDoPeriodicItem(chrono_time, message, period));
	}
}

void ToDoList::addItem(ToDoPeriodicItem item)
{
	std::lock_guard<std::mutex> guard(mutex);
	if (!item.period)
		notifications.push_back(new ToDoItem(item));
	else
		notifications.push_back(new ToDoPeriodicItem(item));
	sortList();
}

ToDoList::~ToDoList()
{
	std::ofstream file("notifications", std::ios_base::out);
	for (auto& i : notifications)
	{
		file << std::chrono::system_clock::to_time_t(i->timePoint) << ' ';
		file << i->message << ' ';
		if (i->IsPeriodic())
			file << i->getPeriod().count();
		file << std::endl;
	}
	isWorking = false;
	thread.join();
	
}

void ToDoList::checkTime() {

	auto element = notifications.back();

	if (element->IsReady()) {
		if (std::chrono::system_clock::now() - element->timePoint > std::chrono::milliseconds{ 10 } and !element->IsPeriodic())
		{
			notifications.pop_back();
			return;
		}
		callback(*element);
		if (element->IsPeriodic()) {
			while(element->IsReady())
				element->timePoint += element->getPeriod();
			sortList();
		}
		else 
			notifications.pop_back();
	}

}

void ToDoList::checkListNotifications(ToDoList& list) {
	while (list.isWorking) {
		std::lock_guard<std::mutex> guard{ mutex };
		if (!list.empty())
			list.checkTime();
	}

}

void ToDoList::sortList()
{
	std::sort(notifications.begin(), notifications.end(), [](ToDoItem* item1, ToDoItem* item2) {
		return item1 > item2; });
}

void ToDoList::showNotifications()
{
	std::lock_guard<std::mutex> guard{ mutex };
	for (auto& i : notifications)
	{
		std::tm tm{ 0 };
		auto in_time_t = std::chrono::system_clock::to_time_t(i->timePoint);
		std::cout <<  "Message text: " << i->message << std::endl;
		localtime_s(&tm, &in_time_t);
		std::cout << "Will work in: " << std::put_time(&tm, "%b/%d %H:%M:%S") << std::endl;
		if (i->IsPeriodic())
		{
			in_time_t = i->getPeriod().count();
			std::tm p{ 0 };
			localtime_s(&p, &in_time_t);
			p.tm_hour -= 2;
			std::cout << "With period: " << std::put_time(&p, "%H:%M:%S");
		}
		std::cout << std::endl;
		std::cout << std::endl;
	}

}

void ToDoList::deleteNotification(size_t index)
{
	std::lock_guard<std::mutex> guard{ mutex };
	if (index < notifications.size())
		notifications.erase(notifications.begin() + index);
}

void ToDoList::setCallbackNotifier(callback_type callback_)
{
	std::lock_guard<std::mutex> guard{ mutex };
	callback = std::move(callback_);
}

void ToDoList::detachCallBack()
{
	std::lock_guard<std::mutex> guard{ mutex };
	callback = nullptr;
}
