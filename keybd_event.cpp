#include <Windows.h>
#include <iostream>
#include <atomic>


bool g_jt_active = false;
static std::atomic<int> g_QiangDu = 14;
static std::atomic<int> g_vKey_JT = VK_RBUTTON;


void ScreenshotBatch(int shotCount)
{
    for (int i = 0; i < shotCount; i++)
    {
        keybd_event(VK_SNAPSHOT, 0, 0, 0);
        keybd_event(VK_SNAPSHOT, 0, KEYEVENTF_KEYUP, 0);
    }
}

void ShowTipInfo()
{
    SetConsoleOutputCP(936);
    system("cls");
    std::cout << "“HOME”键设置截图强度：" << g_QiangDu.load() << std::endl;
    std::cout << "触发按键：鼠标右键" << std::endl;

}

int main()
{
    ShowTipInfo();


    while (true)
    {
 
        if (GetAsyncKeyState(VK_HOME) & 0x8000)
        {
            int newVal;
            std::cout << "\n请输入新的截图强度数值：";
            std::cin >> newVal;
            if (newVal < 1) newVal = 1;
            g_QiangDu.store(newVal);
            ShowTipInfo();
        }

     
        if (GetAsyncKeyState(g_vKey_JT.load()) & 0x8000) {
            if (!g_jt_active) {
                g_jt_active = true;
                int cnt = g_QiangDu.load();
                ScreenshotBatch(cnt);
            }
        }
        else
        {
            g_jt_active = false;
        }
    }

    system("pause");
    return 0;
}