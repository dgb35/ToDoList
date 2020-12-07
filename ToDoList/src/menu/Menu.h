#pragma once
#include <Windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <conio.h>
#include <functional>
using action_type = std::function<void()>;

struct MenuItem
{
    std::string name;
    action_type action;
    COORD position;
};

class Menu
{
public:
    Menu(const HANDLE&);
    void AddItem(std::string, action_type);
    void GetInput();
    void Initialize();
private:
    void SelectItem(int position);
    void ShowItems() const;
    void ShowTime() const;
private:
    enum class Keys{ ARROW = 224, ARROW_UP = 80, ARROW_DOWN = 72, ENTER = 13, ESCAPE = 27};
    HANDLE hConsole;
    std::vector<MenuItem> menuItems;
};


