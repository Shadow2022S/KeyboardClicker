#include <Windows.h>
#include <iostream>
#include <string>
#include <array>

bool IsTargetWindow(HWND hwnd)
{
    wchar_t title[256] = { 0 };
    wchar_t className[256] = { 0 };
    GetWindowTextW(hwnd, title, 256);
    GetClassNameW(hwnd, className, 256);
    std::wstring wTitle(title);
    std::wstring wClass(className);
    if (wTitle.find(L"原神") != std::wstring::npos)
    {
        return true;
    }
    if (wClass == L"UnityWndClass")
    {
        return true;
    }
    return false;
}

bool IsCursorVisible()
{
    CURSORINFO cursorInfo;
    cursorInfo.cbSize = sizeof(CURSORINFO);
    if (GetCursorInfo(&cursorInfo))
    {
        return (cursorInfo.flags & CURSOR_SHOWING) != 0;
    }
    return false;
}

void PressF()
{
    INPUT ip = { 0 };
    ip.type = INPUT_KEYBOARD;
    ip.ki.wVk = 'F';
    SendInput(1, &ip, sizeof(INPUT));
    ip.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &ip, sizeof(INPUT));
}

int main()
{
    std::cout << "Start" << std::endl;

    bool running = false;
    std::array<SHORT, 256> prevStates;
    prevStates.fill(0);

    while (true)
    {
        std::array<SHORT, 256> currStates;
        for (int vk = 1; vk < 256; ++vk)
        {
            currStates[vk] = GetAsyncKeyState(vk);
        }
        bool anyKeyPressedEdge = false;
        for (int vk = 1; vk < 256; ++vk)
        {
            bool isDown = (currStates[vk] & 0x8000) != 0;
            bool wasDown = (prevStates[vk] & 0x8000) != 0;
            if (isDown && !wasDown)
            {
                anyKeyPressedEdge = true;
                break;
            }
        }
        bool ctrlDownEdge = false;
        {
            bool ctrlIsDown = (currStates[VK_CONTROL] & 0x8000) != 0;
            bool ctrlWasDown = (prevStates[VK_CONTROL] & 0x8000) != 0;
            ctrlDownEdge = ctrlIsDown && !ctrlWasDown;
        }

        if (!running)
        {
            if (ctrlDownEdge)
            {
                running = true;
                std::cout << "Run" << std::endl;
                prevStates = currStates;
                Sleep(50);
                continue;
            }
        }
        else
        {
            if (anyKeyPressedEdge)
            {
                running = false;
                std::cout << "Paused by input" << std::endl;
                prevStates = currStates;
                Sleep(200);
                continue;
            }
        }
        if (running)
        {
            HWND hwnd = GetForegroundWindow();
            if (hwnd != nullptr && IsTargetWindow(hwnd))
            {
                if (IsCursorVisible())
                {
                    PressF();
                    Sleep(100);
                }
                else
                {
                    std::cout << "Pause" << std::endl;
                    Sleep(10000);
                }
            }
            else
            {
                Sleep(100);
            }
        }
        else
        {
            Sleep(50);
        }
        prevStates = currStates;
    }
    return 0;
}