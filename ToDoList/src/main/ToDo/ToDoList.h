#pragma once
#include <chrono>
#include <thread>
#include <functional>
#include <algorithm>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <mutex>
#include <sstream>
#include <ctime>
#include "ToDoItem.h"


struct ToDoList
{
    using callback_type = std::function<void(ToDoItem)>;
public:
    void addItem(ToDoPeriodicItem item);
    void showNotifications();
    void deleteNotification(size_t index);
    struct NotifierGuard {
        ToDoList& list;
        NotifierGuard(ToDoList& list_, callback_type callback_)
            : list{ list_ } {
            list.setCallbackNotifier(std::move(callback_));
        }
        ~NotifierGuard() {
            list.detachCallBack();
        } 
    };
    size_t size() const { return notifications.size(); }
    bool empty() const { return notifications.empty(); }
public:
    ToDoList();
    void operator=(const ToDoList&) = delete;
    ToDoList(ToDoList&) = delete;
    ~ToDoList();
private:
    void initThread() {
        isWorking = true;
        thread = std::thread{ [this]() {checkListNotifications(*this); } };
    }
    void checkTime();
    void checkListNotifications(ToDoList&);
    void sortList();

private:
    void setCallbackNotifier(callback_type callback);
    void detachCallBack();
private:
    std::vector<ToDoItem*> notifications;
    callback_type callback;
    bool isWorking;
    std::thread thread;
    std::mutex mutex;
};

