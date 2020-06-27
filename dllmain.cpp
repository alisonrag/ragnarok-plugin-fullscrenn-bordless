// dllmain.cpp : Define o ponto de entrada para o aplicativo DLL.
#include "pch.h"
#include <stdio.h>
#include "wtypes.h"
#include <iostream>
using namespace std;

// load Microsoft Detour Lib
#include "detours.h"
#pragma comment(lib, "detours.lib")

// pre-declare variables and functions
HMODULE hModule;
void GetDesktopResolution(int& horizontal, int& vertical);
void hook();

HWND(WINAPI* pCreateWindowExA) (DWORD, LPCSTR, LPCSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID) = CreateWindowExA;

// detoured function
HWND WINAPI CreateWindowExA_hook(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
	char title[128];
    sprintf_s(title, "%s | [Njord]", lpWindowName); // window title
    
    DWORD style = 0x940A0000; // style bordless
    int width;
    int height;
    GetDesktopResolution(width, height); // set width and height = to screen resolution

    // CreateWindowExA fullscreen bordless at 0,0
    return pCreateWindowExA(dwExStyle, lpClassName, title, style, 0, 0, width, height, hWndParent, hMenu, hInstance, lpParam);
}

// Get the horizontal and vertical screen sizes in pixel
void GetDesktopResolution(int& horizontal, int& vertical) {
    RECT desktop;
    // Get a handle to the desktop window
    const HWND hDesktop = GetDesktopWindow();
    // Get the size of screen to the variable desktop
    GetWindowRect(hDesktop, &desktop);
    // The top left corner will have coordinates (0,0)
    // and the bottom right corner will have coordinates
    // (horizontal, vertical)
    horizontal = desktop.right;
    vertical = desktop.bottom;
}

// Hook function
void hook() {
    // disable libary call
    DisableThreadLibraryCalls(hModule);

    // detour stuff
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    // We attach our hooked function the the original 
    /* HOOK CUSTOM FUNCTION*/
    DetourAttach(&(LPVOID&)pCreateWindowExA, &CreateWindowExA_hook);

    // detour commit changes
    DetourTransactionCommit();
}

// dll entry
BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved ) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)hook, NULL, 0, NULL);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

