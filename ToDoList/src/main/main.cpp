#include <stdio.h>
#include <chrono>
#include <thread>
#include <functional>
#include <iostream>
#include <iomanip>
#include "ToDo/ToDoList.h"
#include "../menu/Menu.h"

std::mutex outputMutex;

std::chrono::time_point<std::chrono::system_clock> GetDate()
{
    auto n = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(n);
    std::tm tm = {0};
    localtime_s(&tm, &in_time_t);
    std::string str;
    std::cin.ignore(32767, '\n');
    std::cin >> str;
    std::stringstream ss(str);
    ss >> std::get_time(&tm, "%H:%M:%S");
    time_t t = std::mktime(&tm);
    std::chrono::time_point<std::chrono::system_clock> tp = std::chrono::system_clock::from_time_t(t);
    return tp;
}
//22:26:00

void Input(std::string& message, std::chrono::time_point<std::chrono::system_clock>& time)
{
    std::cout << "Input data" << std::endl;
    std::cout << "Enter message: " << std::endl;;
    std::cin >> message;
    std::cout << "Enter time(HH:MM:SS): " << std::endl;;
    time = GetDate();
}

void CreateNotification(ToDoList& list)
{
    std::lock_guard<std::mutex> guard{ outputMutex };
    std::string message;
    std::chrono::time_point<std::chrono::system_clock> time;
    Input(message, time);
    list.addItem(ToDoPeriodicItem(time, message, 0));
    std::cout << std::endl;
    system("pause");
    std::cout << std::endl;
}

void CreatePeriodicNotification(ToDoList& list)
{
    std::lock_guard<std::mutex> guard{ outputMutex };
    std::string message;
    std::chrono::time_point<std::chrono::system_clock> time;
    Input(message, time);
    unsigned int period = 0;
    char periodType;
    std::cout << "Enter period: " << std::endl;;
    std::cin >> period;
    std::cout << "Enter period type (seconds, minutes, hours, days):" << std::endl;;
    std::cin >> periodType;
    list.addItem(ToDoPeriodicItem(time, message, period, periodType));
    std::cout << std::endl;
    system("pause");
    std::cout << std::endl;
}

void ShowNotifications(ToDoList& list)
{
    std::lock_guard<std::mutex> guard{ outputMutex };
    list.showNotifications();
    system("pause");
}

void DeleteNotification(ToDoList& list)
{
    std::lock_guard<std::mutex> guard{ outputMutex };
    list.showNotifications();
    size_t number;
    std::cout << "Enter number of notification which you want delete: ";
    std::cin >> number;
    list.deleteNotification(--number);
    std::cout << std::endl;
    system("pause");
}

int main()
{
    outputMutex.lock();
    system("color 0F");
    auto hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    auto testNotification =
        [](const ToDoItem& item) mutable
        { std::lock_guard<std::mutex> guard{ outputMutex }; 
    std::cout << "Notification: " << item.message << std::endl; };
    ToDoList list;
    ToDoList::NotifierGuard guard{ list, testNotification };

    Menu menu(hConsole);
    menu.AddItem("Show notifications", [&list]() {ShowNotifications(list); });
    menu.AddItem("Create notification", [&list]() {CreateNotification(list); });
    menu.AddItem("Create periodic notification", [&list]() {CreatePeriodicNotification(list); });
    menu.AddItem("Delete notification(s)", [&list]() {DeleteNotification(list); });
    menu.Initialize();
    outputMutex.unlock();
    menu.GetInput();



    return 0;
}