
#include <Windows.h>
#include <stdio.h>
#include <stdint.h>

#define TOGGLE_KEY 163

#define CLICKS_PER_SECOND 100

HHOOK _hook;
KBDLLHOOKSTRUCT kbdStruct;

volatile bool click_toggle = false;

LRESULT __stdcall HookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0)
    {
        if (wParam == WM_KEYDOWN)
        {
            // lParam is the pointer to the struct containing the data needed, so cast and assign it to kdbStruct.
            kbdStruct = *((KBDLLHOOKSTRUCT*)lParam);
            // a key (non-system) is pressed.
            //fprintf(stderr, "%lu is pressed\n", kbdStruct.vkCode);

            if(kbdStruct.vkCode == TOGGLE_KEY)
            {
                if(click_toggle)
                {
                    printf("click off\n");
                    click_toggle = false;
                }
                else
                {
                    printf("click on\n");
                    click_toggle = true;
                }
            }
        }
    }
 
    // call the next hook in the hook chain. This is nessecary or your hook chain will break and the hook stops
    return CallNextHookEx(_hook, nCode, wParam, lParam);
}


void SetHook()
{

    if (!(_hook = SetWindowsHookEx(WH_KEYBOARD_LL, HookCallback, NULL, 0)))
    {
        fprintf(stderr, "hook failed");
    exit(1);
    }
}


void ReleaseHook()
{
    UnhookWindowsHookEx(_hook);
}


static FILETIME time;

uint64_t get_micro_time() 
{
    GetSystemTimePreciseAsFileTime(&time);

    return ((((uint64_t)time.dwHighDateTime) << 32) + (uint64_t)time.dwLowDateTime) / 10;    
}


DWORD WINAPI toggle_thread(LPVOID lpParam)
{
    SetHook();
    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0))
    {}

    return 0;
}




int main(int argc, char *argv[])
{   
    

    HANDLE thread1 = CreateThread(NULL, 0, toggle_thread, 0, 0, NULL);
    
    INPUT input;
    input.type=INPUT_MOUSE;
    input.mi.dx=0; 
    input.mi.dy=0;
    input.mi.mouseData=0;
    input.mi.dwExtraInfo=NULL;
    input.mi.time=0;

    uint64_t target_time = 1000000 / CLICKS_PER_SECOND;

    uint64_t curr;
    uint64_t prev = get_micro_time();
    uint64_t dt; // dt in microseconds 10^-6 seconds

    while (1)
    {
        curr = get_micro_time();
        dt = curr - prev;
        prev = curr;

        if(click_toggle)
        {
            input.mi.dwFlags=(MOUSEEVENTF_LEFTDOWN);
            SendInput(1, &input, sizeof(INPUT));

            input.mi.dwFlags=(MOUSEEVENTF_LEFTUP);
            SendInput(1, &input, sizeof(INPUT));
        }


        do
        {
            curr = get_micro_time();
            dt = curr - prev;
        } while(dt < target_time);
    }

    return 0;
}