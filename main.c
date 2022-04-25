#include <Windows.h>
#include <stdio.h>
#include <stdint.h>


typedef uint8_t  U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;

typedef int8_t  I8;
typedef int16_t I16;
typedef int32_t I32;
typedef int64_t I64;

#define bool U8
#define true 1
#define false 0


U16 screen_width;
U16 screen_height;



#define TOGGLE_KEY 163
#define RECORD_KEY 161

#define CLICKS_PER_SECOND 200


#define MAX_INPUT 1024

INPUT input[MAX_INPUT];
U16 input_count = 0;

volatile bool record_toggle = false;
volatile bool click_toggle = false;

HHOOK key_hook;
KBDLLHOOKSTRUCT kbdStruct;

LRESULT __stdcall HookCallback_key(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0)
    {
        if (wParam == WM_KEYDOWN)
        {
            // lParam is the pointer to the struct containing the data needed, so cast and assign it to kdbStruct.
            kbdStruct = *((KBDLLHOOKSTRUCT*)lParam);
            // a key (non-system) is pressed.
            fprintf(stderr, "%lu is pressed\n", kbdStruct.vkCode);

            if(kbdStruct.vkCode == TOGGLE_KEY)
            {
                if(click_toggle)
                {
                    click_toggle = false;
                    printf("click off\n");
                }
                else
                {
                    click_toggle = true;
                    printf("click on\n");
                }
            }

            if(kbdStruct.vkCode == RECORD_KEY)
            {
                if(record_toggle)
                {
                    record_toggle = false;
                    printf("record off\n");
                }
                else
                {
                    memset(input, 0, sizeof(INPUT) * MAX_INPUT);
                    input_count = 0;
                    record_toggle = true;
                    printf("record on\n");
                }
            }
        }
    }
 
    // call the next hook in the hook chain. This is nessecary or your hook chain will break and the hook stops
    return CallNextHookEx(key_hook, nCode, wParam, lParam);
}


U16 getAbsoluteX(U16 x, U16 screen_width)
{
    return (x * 65535) / screen_width;
}


U16 getAbsoluteY(U16 y, U16 screen_height)
{
    return (y * 65535) / screen_height;
}


HHOOK mouse_hook;
MSLLHOOKSTRUCT mouse_struct;

LRESULT __stdcall HookCallback_mouse(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0 && record_toggle && input_count < MAX_INPUT)
    {
        mouse_struct = *((MSLLHOOKSTRUCT*)lParam);

        switch (wParam)
        {
            case WM_LBUTTONDOWN:
            {

                fprintf(stderr, "L mouse button down is pressed at [%lu, %lu]\n", mouse_struct.pt.x, mouse_struct.pt.y);

                input[input_count].type           = INPUT_MOUSE;
                input[input_count].mi.dx          = getAbsoluteX(mouse_struct.pt.x, screen_width);
                input[input_count].mi.dy          = getAbsoluteY(mouse_struct.pt.y, screen_height);
                input[input_count].mi.mouseData   = 0;
                input[input_count].mi.dwExtraInfo = 0;
                input[input_count].mi.time        = 0;
                input[input_count].mi.dwFlags = (MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_MOVE|MOUSEEVENTF_LEFTDOWN);

                ++input_count;
                break;
            }


            case WM_LBUTTONUP:
            {

                fprintf(stderr, "L mouse button up is pressed at [%lu, %lu]\n", mouse_struct.pt.x, mouse_struct.pt.y);

                input[input_count].type           = INPUT_MOUSE;
                input[input_count].mi.dx          = getAbsoluteX(mouse_struct.pt.x, screen_width);
                input[input_count].mi.dy          = getAbsoluteY(mouse_struct.pt.y, screen_height);
                input[input_count].mi.mouseData   = 0;
                input[input_count].mi.dwExtraInfo = 0;
                input[input_count].mi.time        = 0;
                input[input_count].mi.dwFlags = (MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_MOVE|MOUSEEVENTF_LEFTUP);

                ++input_count;
                break;
            }


            case WM_RBUTTONDOWN:
            {

                fprintf(stderr, "R mouse button down is pressed at [%lu, %lu]\n", mouse_struct.pt.x, mouse_struct.pt.y);

                input[input_count].type           = INPUT_MOUSE;
                input[input_count].mi.dx          = getAbsoluteX(mouse_struct.pt.x, screen_width);
                input[input_count].mi.dy          = getAbsoluteY(mouse_struct.pt.y, screen_height);
                input[input_count].mi.mouseData   = 0;
                input[input_count].mi.dwExtraInfo = 0;
                input[input_count].mi.time        = 0;
                input[input_count].mi.dwFlags = (MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_MOVE|MOUSEEVENTF_RIGHTDOWN);


                ++input_count;
                break;
            }


            case WM_RBUTTONUP:
            {

                fprintf(stderr, "R mouse button up is pressed at [%lu, %lu]\n", mouse_struct.pt.x, mouse_struct.pt.y);

                input[input_count].type           = INPUT_MOUSE;
                input[input_count].mi.dx          = getAbsoluteX(mouse_struct.pt.x, screen_width);
                input[input_count].mi.dy          = getAbsoluteY(mouse_struct.pt.y, screen_height);
                input[input_count].mi.mouseData   = 0;
                input[input_count].mi.dwExtraInfo = 0;
                input[input_count].mi.time        = 0;
                input[input_count].mi.dwFlags = (MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_MOVE|MOUSEEVENTF_RIGHTUP);

                ++input_count;
                break;
            }
 
        }

    }
 
    // call the next hook in the hook chain. This is nessecary or your hook chain will break and the hook stops
    return CallNextHookEx(mouse_hook, nCode, wParam, lParam);
}




void SetHook()
{

    if (!(key_hook = SetWindowsHookEx(WH_KEYBOARD_LL, HookCallback_key, NULL, 0)))
    {
        fprintf(stderr, "key hook failed");
        exit(1);
    }


    if (!(mouse_hook = SetWindowsHookEx(WH_MOUSE_LL, HookCallback_mouse, NULL, 0)))
    {
        fprintf(stderr, "mouse hook failed");
        exit(1);
    }

}


void ReleaseHook()
{
    UnhookWindowsHookEx(key_hook);
    UnhookWindowsHookEx(mouse_hook);
}


static FILETIME time;

U64 get_micro_time() 
{
    GetSystemTimePreciseAsFileTime(&time);

    return ((((U64)time.dwHighDateTime) << 32) + (U64)time.dwLowDateTime) / 10;    
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
    screen_width  = GetSystemMetrics(SM_CXSCREEN);
    screen_height = GetSystemMetrics(SM_CYSCREEN);

    printf("load screen with [%u, %u]\n", screen_width, screen_height);

    HANDLE thread1 = CreateThread(NULL, 0, toggle_thread, 0, 0, NULL);
    

    U64 target_time = 1000000 / CLICKS_PER_SECOND;

    U64 curr;
    U64 prev = get_micro_time();
    U64 dt; // dt in microseconds 10^-6 seconds

    U16 i = 0;
    while (1)
    {
        curr = get_micro_time();
        dt = curr - prev;
        prev = curr;

        if(click_toggle && !record_toggle)
        {
            //fprintf(stderr, "sendInput [%lu, %lu], %u\n", input[i].mi.dx, input[i].mi.dy, i);
            SendInput(1, &input[i], sizeof(INPUT));
            ++i;

        }

        if(i >= input_count)
        {
            i = 0;
        }

        do
        {
            curr = get_micro_time();
            dt = curr - prev;
        } while(dt < target_time);
    }

    ReleaseHook();
    return 0;
}
