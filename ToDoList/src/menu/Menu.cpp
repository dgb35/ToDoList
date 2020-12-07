#include "Menu.h"

Menu::Menu(const HANDLE& hConsole_) : hConsole{ hConsole_ }
{
}

void Menu::AddItem(std::string name, action_type action)
{
    menuItems.push_back(MenuItem{ name, action });
}

void Menu::GetInput()
{
    if (!menuItems.size())  
        return;
    SelectItem(0);
    int position = 0, key = 0;
    while (key != static_cast<int>(Keys::ESCAPE))
    {
        key = _getch();
        if (key == static_cast<int>(Keys::ENTER))
        {
            system("cls");
            menuItems[position].action();
            system("cls");
            ShowItems();
            SelectItem(position);
        }

        if (key != static_cast<int>(Keys::ARROW))
            continue;
        key = _getch();
        const auto selected = position;

        if (key == static_cast<int>(Keys::ARROW_UP))
            ++position;
        else if (key == static_cast<int>(Keys::ARROW_DOWN))
            --position;

        if (position < 0)
            position = menuItems.size() - 1;
        else if (position > menuItems.size() - 1)
            position = 0;

        if (selected != position)
        {
            system("cls");
            ShowItems();
            SelectItem(position);
        }

    }
}

//After adding all items of menu
void Menu::Initialize()
{
    ShowTime();
    CONSOLE_SCREEN_BUFFER_INFO screenBuffer = { 0 };
    std::cout << "Esc - to exit" << std::endl << std::endl << "Menu:" << std::endl;
    for (auto& item : menuItems)
    {
        GetConsoleScreenBufferInfo(hConsole, &screenBuffer);
        item.position = screenBuffer.dwCursorPosition;
        std::cout << item.name << std::endl;
    }
}

void Menu::SelectItem(int position)
{
    system("color 0F");
    SetConsoleCursorPosition(hConsole, menuItems[position].position);
    SetConsoleTextAttribute(hConsole, (WORD)((15 << 4) | 0));
    std::cout << menuItems[position].name;
    SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 15));
    SetConsoleCursorPosition(hConsole, menuItems[menuItems.size() - 1].position);
    std::cout << std::endl;
}

void Menu::ShowItems() const
{
    ShowTime();
    std::cout << "Esc - to exit" << std::endl << std::endl << "Menu:" << std::endl;
    for (auto& item : menuItems)
        std::cout << item.name << std::endl;
}

void Menu::ShowTime() const
{
    SYSTEMTIME st;
    GetLocalTime(&st);
    printf("%d-%02d-%02d %02d:%02d:%02d",
        st.wYear,
        st.wMonth,
        st.wDay,
        st.wHour,
        st.wMinute,
        st.wSecond);
    std::cout << std::endl;
}
