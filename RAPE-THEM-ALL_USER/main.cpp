#define _CRT_SECURE_NO_WARNINGS 1
#pragma warning(2:4235)
#pragma warning(disable : 4244)
#pragma warning(disable : 4530)

#include <Windows.h>
#include <iostream>
#include <random>
#include <fstream>
#include <tchar.h>
#include <time.h>
#include <thread>
#include <codecvt>
#include <cstdlib>
#include <d3d9.h>

#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx9.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/imgui.h"
#include "InputUtil.h"
#include "ImGuiLogs.h"
#include "Hotkey.h"
#include "util.h"
#include "lazy.h"
#include "xor.h"

#ifdef DEBUG
#include <DxErr.h>
#pragma comment(lib, "dxerr.lib")
#endif
#include <wininet.h>
#pragma comment ( lib, "Wininet.lib" )
#include <fwpmu.h>
#pragma comment (lib, "fwpuclnt.lib")

#define RGBA_TO_FLOAT(r,g,b,a) (float)r/255.0f, (float)g/255.0f, (float)b/255.0f, (float)a/255.0f

extern void getoffsets();

// Data
static IDirect3D9* g_pD3D = NULL;
static IDirect3DDevice9* g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int button_menu = 1;
int button_menu_p = 1;
bool button_menu_exit = 0;

//extern int dwStatusCode;

extern const char* path;

const char* version = "6.0.0";

bool activate_gls = 0;
int mode_option = 0;
float lagswitchtime = 3000;
float lagswitchtime_2 = 750;
float lagswitchtime_3 = 1000;
float eft_time_separator = 10;
float eft_repeat_time = 3;
float fakelagtime = 500;
float fakelagtime_2 = 1000;
bool activate_sounds = 1;
bool hold_key = 0;
int switch_packages = 1;
KeyBindToggle activate_duration_bind = KeyBindToggle(KeyBind::KeyCode::CAPSLOCK);
KeyBindToggle activate_gls_bind = KeyBindToggle(KeyBind::KeyCode::INSERT);

bool inc_bypass = 0;
bool inc_bypass_alt = 1;
bool outg_bypass_alt = 1;
bool all_bypass_alt = 1;
bool eft_bypass = 0;

std::string rule_name = "GLSbyxvorost";
std::string rule_name2 = "GLSbyxvorost2";

OPENFILENAME ofn;
char szFile1[260];
char szFile2[260];
char szFile3[21];
bool fop = 0;

bool flk = 0;

bool rnd_activate = 1;
int switch_rnd_min = 50;
int switch_rnd_max = 300;
int fake_rnd_min = 1000 + 232 - 950 - 232;
int fake_rnd_max = 200;
int randint = 0;

bool ins_ok = 0;

const char* actualnews;

HWND hwnd;
int WIDTH = 605;
int HEIGHT = 350;
POINTS position = { };

Log logs;
bool prelogs = 0;
int prelogs_count = 0;

using namespace std;

std::string random_string(std::string::size_type length) {
    static auto& chrs = "0123456789"
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    thread_local static std::mt19937 rg{ std::random_device{}() };
    thread_local static std::uniform_int_distribution<std::string::size_type> pick(0, sizeof(chrs) - 2);

    std::string s;
    s.reserve(length);
    while (length--)
        s += chrs[pick(rg)];
    return s;
}

bool is_debugging() {
    // - https://anti-debug.checkpoint.com/
    // - https://github.com/BaumFX/cpp-anti-debug
    // - https://github.com/HackOvert/AntiDBG
    // - https://www.unknowncheats.me/forum/general-programming-and-reversing/374975-coldhide-simple-anti-anti-debug-library-windows.html / https://github.com/Rat431/ColdHide_V2


    // for example 
    BOOL DebuggerPresent = false;
    LI_FN(CheckRemoteDebuggerPresent)(LI_FN(GetCurrentProcess)(), &DebuggerPresent);

    if (LI_FN(IsDebuggerPresent)() or DebuggerPresent) {
        return true;
    }

    return false;
}

static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

static void TextHM(const char* desc) {
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

const char* getnews(const char* WEB_URL) {
    HINTERNET WEB_CONNECT = InternetOpenA("Default_User_Agent", NULL, NULL, NULL, 0);
    HINTERNET WEB_ADDRESS = InternetOpenUrlA(WEB_CONNECT, WEB_URL, NULL, 0, INTERNET_FLAG_KEEP_CONNECTION, 0);
    char* _DATA_RECIEVED = new char[1024];
    DWORD NO_BYTES_READ = 0;
    const char* STRING_DATA = new char[1024];
    while (InternetReadFile(WEB_ADDRESS, _DATA_RECIEVED, 1024, &NO_BYTES_READ) && (NO_BYTES_READ)) {
        //cout << _DATA_RECIEVED;
    }
    InternetCloseHandle(WEB_ADDRESS);
    InternetCloseHandle(WEB_CONNECT);
    return _DATA_RECIEVED;
}

wchar_t* GetWC(const char* c)
{
    const size_t cSize = strlen(c) + 1;
    wchar_t* wc = new wchar_t[cSize];
    mbstowcs(wc, c, cSize);

    return wc;
}

int rnd() {
    if (rnd_activate == 1) {
        if (mode_option == 0) {
            int r = switch_rnd_min + (rand() % switch_rnd_max);
            randint = r;
            return r;
        }
        else {
            int r = fake_rnd_min + (rand() % fake_rnd_max);
            randint = r;
            return r;
        }
    }
    else return 0;
}

bool rlready = 0;
int i1 = 0;
int i2 = 0;
int i3 = 0;
int i4 = 0;

void run_lags() {
wok1:
    //discord_webhook("**[+]** User logged into GLS");
    //if (((int)dwStatusCode < 200 || (int)dwStatusCode > 299) && i1 <= 5) { i1++; goto wok1; }
    //else 
    logs.AddLog(xor ("\n\n[+] Lags function ready"));
    Sleep(2000);
    rlready = 1;
    int prv = 0;
    int prv1 = 0;
    int prv2 = 0;
    bool eft_prv = 0;
    int count_eft = 0;
    std::string cmd;

    HANDLE engineHandle;
    if (DWORD result = FwpmEngineOpen0(NULL, RPC_C_AUTHN_WINNT, NULL, NULL, &engineHandle))
    {
        logs.AddLog(xor ("\n[-] Error 1 - 0x%s"), to_string(result));
    }
    FWPM_FILTER0 filter;
    SecureZeroMemory(&filter, sizeof(filter));
    FWPM_FILTER_CONDITION0 conditions[2];
    conditions[0].fieldKey = FWPM_CONDITION_IP_PROTOCOL;
    conditions[0].conditionValue.type = FWP_UINT8;
    conditions[0].conditionValue.uint8 = 17; //rfc 1700
    conditions[0].matchType = FWP_MATCH_EQUAL;
    conditions[1].fieldKey = FWPM_CONDITION_DIRECTION;
    conditions[1].conditionValue.type = FWP_UINT32;
    conditions[1].conditionValue.uint32 = FWP_DIRECTION_INBOUND;
    conditions[1].matchType = FWP_MATCH_EQUAL;
    filter.action.type = FWP_ACTION_BLOCK;
    filter.displayData.name = GetWC("GLSblockudpINBOUND");
    filter.layerKey = FWPM_LAYER_DATAGRAM_DATA_V4;
    filter.numFilterConditions = 2;
    filter.weight.type = FWP_EMPTY;
    filter.filterCondition = conditions;

    HANDLE engineHandle1;
    if (DWORD result1 = FwpmEngineOpen0(NULL, RPC_C_AUTHN_WINNT, NULL, NULL, &engineHandle1))
    {
        logs.AddLog(xor ("\n[-] Error 11 - 0x%s"), to_string(result1));
    }
    FWPM_FILTER0 filter1;
    SecureZeroMemory(&filter1, sizeof(filter1));
    FWPM_FILTER_CONDITION0 conditions1[2];
    conditions1[0].fieldKey = FWPM_CONDITION_IP_PROTOCOL;
    conditions1[0].conditionValue.type = FWP_UINT8;
    conditions1[0].conditionValue.uint8 = 17; //rfc 1700
    conditions1[0].matchType = FWP_MATCH_EQUAL;
    conditions1[1].fieldKey = FWPM_CONDITION_DIRECTION;
    conditions1[1].conditionValue.type = FWP_UINT32;
    conditions1[1].conditionValue.uint32 = FWP_DIRECTION_OUTBOUND;
    conditions1[1].matchType = FWP_MATCH_EQUAL;
    filter1.action.type = FWP_ACTION_BLOCK;
    filter1.displayData.name = GetWC("GLSblockudpOUTBOUND");
    filter1.layerKey = FWPM_LAYER_DATAGRAM_DATA_V4;
    filter1.numFilterConditions = 2;
    filter1.weight.type = FWP_EMPTY;
    filter1.filterCondition = conditions1;

    while (true) {
        if (activate_gls == 1 && (szFile1[5] == szFile2[5] || (((inc_bypass_alt == 1 && switch_packages == 1) || (outg_bypass_alt == 1 && switch_packages == 0) || (all_bypass_alt == 1 && switch_packages == 2)) && mode_option == 0))) {
            if (prv2 == 0 && ((inc_bypass_alt == 0 && switch_packages == 1) || (outg_bypass_alt == 0 && switch_packages == 0) || (all_bypass_alt == 1 && switch_packages == 2))) {
                cmd = "netsh advfirewall firewall delete rule name=\"" + rule_name + "\"";
                WinExec(cmd.c_str(), 0);
                cmd = "netsh advfirewall firewall delete rule name=\"" + rule_name2 + "\"";
                WinExec(cmd.c_str(), 0);
                prv2 = 1;
            }
            if (mode_option == 0) {
                if (hold_key == 0) {
                    if (GetKeyState(activate_duration_bind.toInt()) & 0x8000) {
                        if ((inc_bypass_alt == 0 && switch_packages == 1) || (outg_bypass_alt == 0 && switch_packages == 0) || (all_bypass_alt == 0 && switch_packages == 2)) {
                            if (switch_packages == 0) cmd = "netsh advfirewall firewall add rule name =\"" + rule_name + "\" dir=out action=block program=\"" + szFile1 + "\"";
                            else if (switch_packages == 1) cmd = "netsh advfirewall firewall add rule name =\"" + rule_name + "\" dir=in action=block program=\"" + szFile1 + "\"";
                            else {
                                cmd = "netsh advfirewall firewall add rule name =\"" + rule_name + "\" dir=out action=block program=\"" + szFile1 + "\"";
                                WinExec(cmd.c_str(), 0);
                                cmd = "netsh advfirewall firewall add rule name =\"" + rule_name + "\" dir=in action=block program=\"" + szFile1 + "\"";
                            }
                            WinExec(cmd.c_str(), 0);
                            if (activate_sounds == 1) Beep(1000, 100);
                            Sleep(lagswitchtime + rnd());
                            cmd = "netsh advfirewall firewall delete rule name=\"" + rule_name + "\"";
                            WinExec(cmd.c_str(), 0);
                            if (activate_sounds == 1) Beep(1000, 200);
                            Sleep(lagswitchtime_2);
                        }
                        else if (inc_bypass_alt == 1 && switch_packages == 1) {
                            if (eft_bypass == 0) {
                                //logs.AddLog("\nincoming bypass add time");
                                if (DWORD result = FwpmFilterAdd0(engineHandle, &filter, NULL, &filter.filterId))
                                {
                                    logs.AddLog(xor ("\n[-] Error 2 - 0x%s"), to_string(result));
                                }
                                if (activate_sounds == 1) Beep(1000, 100);
                                Sleep(lagswitchtime + rnd());
                                //logs.AddLog("\nincoming bypass delete time");
                                if (DWORD result = FwpmFilterDeleteById0(engineHandle, filter.filterId))
                                {
                                    logs.AddLog(xor ("\n[-] Error 3 - 0x%s"), to_string(result));
                                }
                                if (activate_sounds == 1) Beep(1000, 200);
                                Sleep(lagswitchtime_2);
                            }
                            else {
                                if (activate_sounds == 1) Beep(1000, 100);
                                for (int i = 1; i <= eft_repeat_time; i++) {
                                    if (DWORD result = FwpmFilterAdd0(engineHandle, &filter, NULL, &filter.filterId))
                                    {
                                        logs.AddLog(xor ("\n[-] Error 2 EFT - 0x%s"), to_string(result));
                                    }
                                    Sleep(lagswitchtime_3);
                                    if (DWORD result = FwpmFilterDeleteById0(engineHandle, filter.filterId))
                                    {
                                        logs.AddLog(xor ("\n[-] Error 3 EFT - 0x%s"), to_string(result));
                                    }
                                    Sleep(eft_time_separator);
                                }
                                if (activate_sounds == 1) Beep(1000, 200);
                                Sleep(lagswitchtime_2);
                            }
                        }
                        else if (outg_bypass_alt == 1 && switch_packages == 0) {
                            //logs.AddLog("\noutgoing bypass add time");
                            if (DWORD result = FwpmFilterAdd0(engineHandle1, &filter1, NULL, &filter1.filterId))
                            {
                                logs.AddLog(xor ("\n[-] Error 22 - 0x%s"), to_string(result));
                            }
                            if (activate_sounds == 1) Beep(1000, 100);
                            Sleep(lagswitchtime + rnd());
                            //logs.AddLog("\noutgoing bypass delete time");
                            if (DWORD result = FwpmFilterDeleteById0(engineHandle1, filter1.filterId))
                            {
                                logs.AddLog(xor ("\n[-] Error 33 - 0x%s"), to_string(result));
                            }
                            if (activate_sounds == 1) Beep(1000, 200);
                            Sleep(lagswitchtime_2);
                        }
                        else if (all_bypass_alt == 1 && switch_packages == 2) {
                            //logs.AddLog("\nall bypass add time");
                            if (DWORD result = FwpmFilterAdd0(engineHandle, &filter, NULL, &filter.filterId))
                            {
                                logs.AddLog(xor ("\n[-] Error 2 - 0x%s"), to_string(result));
                            }
                            if (DWORD result = FwpmFilterAdd0(engineHandle1, &filter1, NULL, &filter1.filterId))
                            {
                                logs.AddLog(xor ("\n[-] Error 22 - 0x%s"), to_string(result));
                            }
                            if (activate_sounds == 1) Beep(1000, 100);
                            Sleep(lagswitchtime + rnd());
                            //logs.AddLog("\nall bypass delete time");
                            if (DWORD result = FwpmFilterDeleteById0(engineHandle, filter.filterId))
                            {
                                logs.AddLog(xor ("\n[-] Error 3 - 0x%s"), to_string(result));
                            }
                            if (DWORD result = FwpmFilterDeleteById0(engineHandle1, filter1.filterId))
                            {
                                logs.AddLog(xor ("\n[-] Error 33 - 0x%s"), to_string(result));
                            }
                            if (activate_sounds == 1) Beep(1000, 200);
                            Sleep(lagswitchtime_2);
                        }
                    }
                }
                else {
                    if (GetAsyncKeyState(activate_duration_bind.toInt()) & 0x8000) {
                        if (prv == 0 && eft_bypass == 0) {
                            if ((inc_bypass_alt == 0 && switch_packages == 1) || (outg_bypass_alt == 0 && switch_packages == 0) || (all_bypass_alt == 0 && switch_packages == 2)) {
                                if (switch_packages == 0) cmd = "netsh advfirewall firewall add rule name =\"" + rule_name + "\" dir=out action=block program=\"" + szFile1 + "\"";
                                else if (switch_packages == 1) cmd = "netsh advfirewall firewall add rule name =\"" + rule_name + "\" dir=in action=block program=\"" + szFile1 + "\"";
                                else {
                                    cmd = "netsh advfirewall firewall add rule name =\"" + rule_name + "\" dir=out action=block program=\"" + szFile1 + "\"";
                                    WinExec(cmd.c_str(), 0);
                                    cmd = "netsh advfirewall firewall add rule name =\"" + rule_name + "\" dir=in action=block program=\"" + szFile1 + "\"";
                                }
                                WinExec(cmd.c_str(), 0);
                            }
                            else if (inc_bypass_alt == 1 && switch_packages == 1) {
                                //logs.AddLog("\nincoming bypass add");
                                if (DWORD result = FwpmFilterAdd0(engineHandle, &filter, NULL, &filter.filterId))
                                {
                                    logs.AddLog(xor ("\n[-] Error 2 - 0x%s"), to_string(result));
                                }
                            }
                            else if (outg_bypass_alt == 1 && switch_packages == 0) {
                                //logs.AddLog("\noutgoing bypass add");
                                if (DWORD result = FwpmFilterAdd0(engineHandle1, &filter1, NULL, &filter1.filterId))
                                {
                                    logs.AddLog(xor ("\n[-] Error 22 - 0x%s"), to_string(result));
                                }
                            }
                            else if (all_bypass_alt == 1 && switch_packages == 2) {
                                //logs.AddLog("\nall bypass add");
                                if (DWORD result = FwpmFilterAdd0(engineHandle, &filter, NULL, &filter.filterId))
                                {
                                    logs.AddLog(xor ("\n[-] Error 2 - 0x%s"), to_string(result));
                                }
                                if (DWORD result = FwpmFilterAdd0(engineHandle1, &filter1, NULL, &filter1.filterId))
                                {
                                    logs.AddLog(xor ("\n[-] Error 22 - 0x%s"), to_string(result));
                                }
                            }
                            if (activate_sounds == 1) Beep(1000, 100);
                            prv = 1;
                        }
                        else if (eft_bypass == 1 && inc_bypass_alt == 1 && switch_packages == 1) {
                            if (activate_sounds == 1) Beep(1000, 100);
                            while (GetAsyncKeyState(activate_duration_bind.toInt()) & 0x8000) {
                                Sleep(1);
                                count_eft++;
                                if (eft_prv == 0) {
                                    if (DWORD result = FwpmFilterAdd0(engineHandle, &filter, NULL, &filter.filterId))
                                    {
                                        logs.AddLog(xor ("\n[-] Error 2 EFT - 0x%s"), to_string(result));
                                    }
                                    eft_prv = 1;
                                }
                                if (count_eft >= (lagswitchtime_3 / 10)) {
                                    if (DWORD result = FwpmFilterDeleteById0(engineHandle, filter.filterId))
                                    {
                                        logs.AddLog(xor ("\n[-] Error 3 EFT - 0x%s"), to_string(result));
                                    }
                                    Sleep(eft_time_separator);
                                    count_eft = 0;
                                    eft_prv = 0;
                                }
                            }
                        }
                    }
                    else {
                        if (prv == 1 && eft_bypass == 0) {
                            if ((inc_bypass_alt == 0 && switch_packages == 1) || (outg_bypass_alt == 0 && switch_packages == 0) || (all_bypass_alt == 0 && switch_packages == 2)) {
                                cmd = "netsh advfirewall firewall delete rule name=\"" + rule_name + "\"";
                                WinExec(cmd.c_str(), 0);
                            }
                            else if (inc_bypass_alt == 1 && switch_packages == 1) {
                                //logs.AddLog("\nincoming bypass delete");
                                if (DWORD result = FwpmFilterDeleteById0(engineHandle, filter.filterId))
                                {
                                    logs.AddLog(xor ("\n[-] Error 3 - 0x%s"), to_string(result));
                                }
                            }
                            else if (outg_bypass_alt == 1 && switch_packages == 0) {
                                //logs.AddLog("\noutgoing bypass delete");
                                if (DWORD result = FwpmFilterDeleteById0(engineHandle1, filter1.filterId))
                                {
                                    logs.AddLog(xor ("\n[-] Error 33 - 0x%s"), to_string(result));
                                }
                            }
                            else if (all_bypass_alt == 1 && switch_packages == 2) {
                                //logs.AddLog("\nall bypass delete");
                                if (DWORD result = FwpmFilterDeleteById0(engineHandle, filter.filterId))
                                {
                                    logs.AddLog(xor ("\n[-] Error 3 - 0x%s"), to_string(result));
                                }
                                if (DWORD result = FwpmFilterDeleteById0(engineHandle1, filter1.filterId))
                                {
                                    logs.AddLog(xor ("\n[-] Error 33 - 0x%s"), to_string(result));
                                }
                            }
                            if (activate_sounds == 1) Beep(1000, 200);
                            prv = 0;
                        }
                        else if (eft_bypass == 1 && inc_bypass_alt == 1 && switch_packages == 1 && eft_prv == 1) {
                            if (DWORD result = FwpmFilterDeleteById0(engineHandle, filter.filterId))
                            {
                                logs.AddLog(xor ("\n[-] Error 3 EFT - 0x%s"), to_string(result));
                            }
                            if (activate_sounds == 1) Beep(1000, 200);
                            Sleep(lagswitchtime_2);
                            eft_prv = 0;
                        }
                    }
                }
            }
            else {
                if (hold_key == 0) {
                    if (GetKeyState(activate_duration_bind.toInt()) & 0x8000) { if (flk == 0) flk = 1; else flk = 0; if (activate_sounds == 1) Beep(1000, 100); Sleep(600); }
                    if (flk == 1) {
                        cmd = "netsh advfirewall firewall add rule name =\"" + rule_name + "\" dir=out action=block program=\"" + szFile1 + "\"";
                        WinExec(cmd.c_str(), 0);
                        Sleep(fakelagtime + rnd());
                        cmd = "netsh advfirewall firewall delete rule name=\"" + rule_name + "\"";
                        WinExec(cmd.c_str(), 0);
                        Sleep(fakelagtime_2);
                    }
                }
                else {
                    if (GetAsyncKeyState(activate_duration_bind.toInt()) & 0x8000) {
                        cmd = "netsh advfirewall firewall add rule name =\"" + rule_name + "\" dir=out action=block program=\"" + szFile1 + "\"";
                        WinExec(cmd.c_str(), 0);
                        Sleep(fakelagtime + rnd());
                        cmd = "netsh advfirewall firewall delete rule name=\"" + rule_name + "\"";
                        WinExec(cmd.c_str(), 0);
                        Sleep(fakelagtime_2);
                    }
                    else {
                        if (prv1 == 1) prv1 = 0;
                    }
                }
            }
        }
        else prv2 = 0;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

int main(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    FreeConsole();
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFile = szFile1;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile1);
    ofn.lpstrFilter = "EXE (*.exe)\0*.exe\0\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    szFile2[5] = { '*' };

    getoffsets();
    ifstream ins;
    ins.open(xor ("C:\\gls\\ins.txt"));
    if (!ins.is_open()) {
        HANDLE h_file = CreateFile(xor ("C:\\gls\\ins.txt"), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        CloseHandle(h_file);
        ins_ok = 1;
    }
    std::thread run_lg(run_lags);

    RECT desktop;
    GetWindowRect(GetDesktopWindow(), &desktop);

    // Create application window
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T(" "), NULL };
    ::RegisterClassEx(&wc);
    hwnd = ::CreateWindow(wc.lpszClassName, _T(" "), WS_POPUP, desktop.right / 2 - WIDTH / 2, desktop.bottom / 2 - HEIGHT / 2, WIDTH, HEIGHT, NULL, NULL, wc.hInstance, NULL);

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
        exit(0);
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    if (szFile1[1] == ':') { fop = 1; for (int i = 0; i <= 260; i++) szFile2[i] = szFile1[i]; }

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    ImGuiStyle& Style = ImGui::GetStyle();

    ImGui::GetStyle().WindowPadding = ImVec2(10.000f, 8.000f);
    Style.WindowTitleAlign = ImVec2(0.500f, 0.500f);

    Style.Colors[ImGuiCol_FrameBg] = ImVec4(0.141f, 0.141f, 0.141f, 0.667f);
    Style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.141f, 0.141f, 0.141f, 0.667f);
    Style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.141f, 0.141f, 0.141f, 0.667f);
    Style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.039f, 0.039f, 0.037f, 1.000f);
    Style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.141f, 0.141f, 0.141f, 0.667f);
    Style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.580f, 0.294f, 0.282f, 1.000f);
    Style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.643f, 0.329f, 0.318f, 1.000f);
    Style.Colors[ImGuiCol_CheckMark] = ImVec4(0.769f, 0.349f, 0.333f, 1.000f);
    Style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.580f, 0.294f, 0.282f, 1.000f);
    Style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.769f, 0.349f, 0.333f, 1.000f);
    Style.Colors[ImGuiCol_Button] = ImVec4(0.137f, 0.137f, 0.141f, 0.549f);
    Style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.580f, 0.294f, 0.282f, 1.000f);
    Style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.769f, 0.349f, 0.333f, 1.000f);
    Style.Colors[ImGuiCol_Header] = ImVec4(0.141f, 0.141f, 0.141f, 0.667f);
    Style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.580f, 0.294f, 0.282f, 1.000f);
    Style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.769f, 0.349f, 0.333f, 1.000f);
    Style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.580f, 0.294f, 0.282f, 0.549f);
    Style.Colors[ImGuiCol_Separator] = ImVec4(0.780f, 0.349f, 0.333f, 1.000f);
    Style.Colors[ImGuiCol_Tab] = ImVec4(0.137f, 0.137f, 0.141f, 0.549f);
    Style.Colors[ImGuiCol_TabHovered] = ImVec4(0.580f, 0.294f, 0.282f, 1.000f);
    Style.Colors[ImGuiCol_TabActive] = ImVec4(0.769f, 0.349f, 0.333f, 1.000f);
    Style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.137f, 0.137f, 0.141f, 0.549f);
    Style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.580f, 0.294f, 0.282f, 0.549f);

    // Main loop
    bool done = false;
    int cwr = 0;
    bool chk_eft = 0;
    while (!done)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        MSG msg;
        while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        cwr++;

        // Start the Dear ImGui frame
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(WIDTH, HEIGHT), ImGuiCond_Once);

        if (GetKeyState(activate_gls_bind.toInt()) & 0x8000) { if (activate_gls == 0) activate_gls = 1; else activate_gls = 0; if (activate_sounds == 1) Beep(1000, 150); Sleep(200); }

        {
            ImGui::Begin(xor ("GLS - Gaming Lag Switch by xvorost"), NULL, 18470);

            ImVec2 P1, P2;
            const auto& CurrentWindowPos = ImGui::GetWindowPos();
            const auto& pWindowDrawList = ImGui::GetWindowDrawList();
            const auto& pBackgroundDrawList = ImGui::GetBackgroundDrawList();
            const auto& pForegroundDrawList = ImGui::GetForegroundDrawList();

            if (rlready == 0) {
                ImGui::Text(xor ("      "));
                ImGui::Text(xor ("      "));
                ImGui::SameLine();
                ImGui::BeginGroup();
                {
                    logs.Draw(xor ("Logs:"));
                    ImGui::Spacing();
                    if (prelogs_count < 50 || (prelogs_count > 150 && prelogs_count <= 200) || (prelogs_count > 300 && prelogs_count <= 350)) ImGui::Text(xor ("Wait ."));
                    if (prelogs_count > 50 && prelogs_count <= 100 || (prelogs_count > 200 && prelogs_count <= 250) || (prelogs_count > 350 && prelogs_count <= 400)) ImGui::Text(xor ("Wait . ."));
                    if (prelogs_count > 100 && prelogs_count <= 150 || (prelogs_count > 250 && prelogs_count <= 300) || prelogs_count > 400) { ImGui::Text(xor ("Wait . . .")); if (prelogs_count >= 450) prelogs_count = 0; }
                }
                ImGui::EndGroup();
                prelogs_count++;
            }
            else {
                if (ins_ok == 1) {
                    ImGui::Text(xor ("      "));
                    ImGui::Text(xor ("      "));
                    ImGui::SameLine();
                    ImGui::BeginGroup();
                    {
                        ImGui::Text("      ");
                        ImGui::Text("      ");
                        ImGui::Text("      ");
                        ImGui::Text("      ");
                        ImGui::Text("      ");
                        ImGui::Text("      ");
                        ImGui::Text("                     ");
                        ImGui::SameLine();
                        ImGui::BeginGroup();
                        {
                            ImGui::Text(xor ("Open instructions for use?"));
                            ImGui::Spacing();
                            if (ImGui::Button(xor ("EN"), ImVec2(50, 20))) {
                                ShellExecute(NULL, "open", "https://lagswitch.org/instructions/en/", NULL, NULL, SW_SHOWNORMAL);
                                ins_ok = 0;
                            }
                            ImGui::SameLine();
                            if (ImGui::Button(xor ("RU"), ImVec2(50, 20))) {
                                ShellExecute(NULL, "open", "https://lagswitch.org/instructions/ru/", NULL, NULL, SW_SHOWNORMAL);
                                ins_ok = 0;
                            }
                            ImGui::SameLine();
                            if (ImGui::Button(xor ("CLOSE"), ImVec2(65, 20))) {
                                ins_ok = 0;
                            }
                        }
                        ImGui::EndGroup();
                    }
                    ImGui::EndGroup();
                }
                else {
                    ImGui::Text("      ");
                    ImGui::SameLine();

                    ImGui::BeginGroup();
                    {
                        static bool menu_1 = true;
                        static bool menu_2 = false;
                        static bool menu_3 = false;

                        if (button_menu_exit != 1) {
                            if (!menu_1)
                                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.137f, 0.137f, 0.141f, 0.549f));
                            else
                                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.769f, 0.349f, 0.333f, 1.000f));
                            if (ImGui::Button(xor ("Main"), ImVec2(90, 25)))
                            {
                                menu_1 = 1;
                                menu_2 = 0;
                                menu_3 = 0;
                                button_menu = 1;
                            }
                            ImGui::PopStyleColor(1);

                            ImGui::SameLine();

                            if (!menu_2)
                                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.137f, 0.137f, 0.141f, 0.549f));
                            else
                                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.769f, 0.349f, 0.333f, 1.000f));
                            if (ImGui::Button(xor ("Settings"), ImVec2(90, 25)))
                            {
                                menu_2 = 1;
                                menu_1 = 0;
                                menu_3 = 0;
                                button_menu = 5;
                            }
                            ImGui::PopStyleColor(1);

                            ImGui::SameLine();

                            if (!menu_3)
                                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.137f, 0.137f, 0.141f, 0.549f));
                            else
                                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.769f, 0.349f, 0.333f, 1.000f));
                            if (ImGui::Button(xor ("Logs"), ImVec2(90, 25)))
                            {
                                menu_3 = 1;
                                menu_2 = 0;
                                menu_1 = 0;
                                button_menu = 4;
                            }
                            ImGui::PopStyleColor(1);

                            ImGui::SameLine();
                            ImGui::Text("                    "); ImGui::SameLine();
                            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.769f, 0.349f, 0.333f, 1.000f));
                            if (ImGui::Button(xor ("_"))) ::ShowWindow(hwnd, SW_MINIMIZE);
                            ImGui::PopStyleColor(1);
                            ImGui::SameLine();
                            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.769f, 0.349f, 0.333f, 1.000f));
                            if (ImGui::Button(xor ("X"))) button_menu_exit = 1;
                            ImGui::PopStyleColor(1);
                        }
                    }
                    ImGui::EndGroup();

                    if ((button_menu == 1 || button_menu == 2 || button_menu == 3 || button_menu == 4 || button_menu == 5) && button_menu_exit != 1) {
                        ImGui::Spacing();
                        ImGui::Separator();
                        ImGui::Spacing();
                    }

                    if (button_menu == 1 && button_menu_exit != 1) {

                        ImGui::Text(xor ("                     "));

                        ImGui::Text(xor ("      "));
                        ImGui::SameLine();

                        ImGui::BeginGroup();
                        {
                            ImGui::BeginGroup();
                            {
                                ImGui::PushItemWidth(100);
                                {
                                    ImGui::BulletText(xor ("Enable GLS:")); ImGui::SameLine();
                                    ImGui::Checkbox(xor ("###ActivateGLS"), &activate_gls);
                                    ImGuiCustom::hotkey(xor ("Enable keybind:"), activate_gls_bind, 110.0f);
                                    TextHM(xor ("Toggle key for enable GLS."));
                                    ImGui::SameLine();
                                    ImGui::Text(xor ("   "));
                                    ImGui::Spacing();
                                    ImGui::Text(xor ("Lag mode:"));
                                    ImGui::SameLine();
                                    ImGui::Combo(xor ("###lagmode"), &mode_option, xor ("Lag Switch\0Fake Lag\0"));
                                    TextHM(xor ("Lag mode.\nLag Switch - Incoming / outgoing / all packets\nFake Lag - Only outgoing packets without udp bypass"));
                                    ImGui::Text(xor (""));
                                    ImGui::BulletText(xor ("Settings:"));
                                    ImGui::Spacing();
                                }
                            }
                            ImGui::EndGroup();
                            ImGui::SameLine();
                            ImGui::BeginGroup();
                            {
                                ImGui::Text(xor ("Path: "));
                                ImGui::SameLine();
                                if (ImGui::Button(xor ("Select"), ImVec2(85, 20))) {
                                    fop = 0;
                                    if (GetOpenFileName(&ofn) == TRUE) {
                                        fop = 1;
                                        for (int i = 0; i <= 260; i++) {
                                            szFile2[i] = szFile1[i];
                                            if (i >= 0 && i <= 18) szFile3[i] = szFile1[i];
                                            else if (i > 18 && i <= 20) szFile3[i] = '.';
                                        }
                                        rlready = 1;
                                        logs.AddLog(xor ("\n[+] Path selected"));
                                    }
                                    else {
                                        if (szFile1[1] == ':') { fop = 1; for (int i = 0; i <= 260; i++) szFile2[i] = szFile1[i]; logs.AddLog(xor ("\n[+] Path loaded")); }
                                    }
                                }
                                TextHM(xor ("Path to the executable (exe) file of the game.\nExample: C:\\example-folder\\example.exe"));
                                if ((inc_bypass_alt == 0 && switch_packages == 1) || (outg_bypass_alt == 0 && switch_packages == 0) || (all_bypass_alt == 0 && switch_packages == 2) || mode_option == 1) {
                                    if (fop == 1) {
                                        ImGui::Text(xor ("Selected: ")); ImGui::SameLine();
                                        ImGui::TextDisabled(szFile3);
                                        TextHM(szFile1);
                                    }
                                    else {
                                        ImGui::Text(xor ("Selected: ")); ImGui::SameLine(); ImGui::TextDisabled(xor ("none"));
                                    }
                                }
                                else if (inc_bypass_alt == 1 || outg_bypass_alt == 1 || all_bypass_alt == 1 && (switch_packages == 0 || switch_packages == 1 || switch_packages == 2)) {
                                    ImGui::Text(xor ("Selected: ")); ImGui::SameLine(); ImGui::TextDisabled(xor ("Alternative bypass")); ImGui::SameLine(); HelpMarker(xor ("Alternative bypass block all selected traffic connections to the system (packages)."));
                                }
                                ImGui::Spacing();
                                ImGui::Spacing();
                                ImGui::Text(xor ("Sounds:"));
                                ImGui::SameLine(); ImGui::Checkbox(xor ("##activate_sounds"), &activate_sounds);
                                TextHM(xor ("Sound accompaniment when lag action is activated."));
                            }
                            ImGui::EndGroup();

                            if (button_menu_p == 1) {
                                ImGui::BeginGroup();
                                {
                                    ImGui::PushItemWidth(100);
                                    {
                                        if (mode_option == 0) {
                                            ImGui::Text(xor ("Traffic:"));
                                            ImGui::SameLine(); ImGui::Combo(xor ("###package"), &switch_packages, xor ("Outgoing\0Incoming\0All\0"));
                                            TextHM(xor ("Select to block certain traffic."));
                                            if (hold_key == 0 && eft_bypass == 0 || switch_packages != 1) {
                                                ImGui::Text(xor ("Duration:"));
                                                ImGui::SameLine(); ImGui::InputFloat("##lagswitchtime", &lagswitchtime, 0.0f, 0.0f, "%.0f");
                                                TextHM(xor ("Lag switch are activated for a given number of ms (milliseconds).\nMin: 100\nMax: 10000"));
                                                if (lagswitchtime < 100) lagswitchtime = 100; else if (lagswitchtime > 10000) lagswitchtime = 10000;
                                                ImGui::Text(xor ("Delay after:"));
                                                ImGui::SameLine(); ImGui::InputFloat("##lagswitchtime_2", &lagswitchtime_2, 0.0f, 0.0f, "%.0f");
                                                TextHM(xor ("Delay after lag switch action (milliseconds).\nMin: 100\nMax: 1000"));
                                                if (lagswitchtime_2 < 100) lagswitchtime_2 = 100; else if (lagswitchtime_2 > 1000) lagswitchtime_2 = 1000;
                                            }
                                            else if (eft_bypass == 1 || switch_packages == 1) {
                                                ImGui::Text(xor ("Duration:")); ImGui::SameLine(); ImGui::TextDisabled("%.0f ms", lagswitchtime);
                                                ImGui::Spacing();
                                                ImGui::Text(xor ("Delay after:"));
                                                ImGui::SameLine(); ImGui::InputFloat("##lagswitchtime_2", &lagswitchtime_2, 0.0f, 0.0f, "%.0f");
                                                TextHM(xor ("Delay after lag switch action (milliseconds).\nMin: 100\nMax: 1000"));
                                                if (lagswitchtime_2 < 100) lagswitchtime_2 = 100; else if (lagswitchtime_2 > 1000) lagswitchtime_2 = 1000;
                                            }
                                            else {
                                                ImGui::Text(xor ("Duration:")); ImGui::SameLine(); ImGui::TextDisabled("%.0f ms", lagswitchtime);
                                                ImGui::Spacing();
                                                ImGui::Text(xor ("Delay after:")); ImGui::SameLine(); ImGui::TextDisabled("%.0f ms", lagswitchtime_2);
                                                ImGui::Spacing();
                                            }
                                        }
                                        else {
                                            ImGui::Text(xor ("Enable:"));
                                            ImGui::SameLine(); ImGui::Checkbox(xor ("###activateflk"), &flk);
                                            TextHM(xor ("Enable fake lag action."));
                                            ImGui::Text(xor ("Delay:"));
                                            ImGui::SameLine(); ImGui::InputFloat("##lagswitchtime", &fakelagtime, 0.0f, 0.0f, "%.0f");
                                            TextHM(xor ("Interval between activation and deactivation of the lag in ms (milliseconds).\nMin: 10\nMax: 5000"));
                                            if (fakelagtime < 10) fakelagtime = 10; else if (fakelagtime > 5000) fakelagtime = 5000;
                                            ImGui::Text(xor ("Delay after:"));
                                            ImGui::SameLine(); ImGui::InputFloat("##lagswitchtime_2", &fakelagtime_2, 0.0f, 0.0f, "%.0f");
                                            TextHM(xor ("Interval after deactivation of the lag action in ms (milliseconds).\nMin: 10\nMax: 5000"));
                                            if (fakelagtime_2 < 10) fakelagtime_2 = 10; else if (fakelagtime_2 > 5000) fakelagtime_2 = 5000;
                                        }
                                        ImGui::Text(xor ("Hold key:"));
                                        ImGui::SameLine(); ImGui::Checkbox(xor ("##hold_key"), &hold_key);
                                        TextHM(xor ("Activate the lag action by holding down a key."));
                                        ImGuiCustom::hotkey(xor ("Activation keybind:"), activate_duration_bind, 140.0f);
                                        TextHM(xor ("Lag action activation key."));
                                    }
                                }
                                ImGui::EndGroup();
                                ImGui::SameLine();
                                ImGui::BeginGroup();
                                {
                                    ImGui::PushItemWidth(100);
                                    {
                                        if (mode_option == 0 && switch_packages == 1) {
                                            ImGui::Text(xor ("UDP bypass:"));
                                            ImGui::SameLine(); ImGui::Checkbox(xor ("###Incomingbypassalt"), &inc_bypass_alt);
                                            TextHM(xor ("UDP bypass protection of incoming packets.\n(For UDP games: EFT, DayZ and etc.)\nATTENTION! Blocks all incoming connections to the system."));
                                            if (inc_bypass_alt == 1) {
                                                ImGui::Text(xor ("Kick bypass:"));
                                                ImGui::SameLine(); ImGui::Checkbox(xor ("###kickbypass"), &eft_bypass);
                                                TextHM(xor ("Bypass kicking from games."));
                                                if (eft_bypass == 1) {
                                                    ImGui::Text(xor ("Lag duration:"));
                                                    ImGui::SameLine(); ImGui::InputFloat("##kbtime", &lagswitchtime_3, 0.0f, 0.0f, "%.0f");
                                                    TextHM(xor ("Additional lag time (in ms)\nMin: 100\nMax: 1000"));
                                                    if (lagswitchtime_3 < 100) lagswitchtime_3 = 100; else if (lagswitchtime_3 > 1000) lagswitchtime_3 = 1000;
                                                    ImGui::Text(xor ("Separator:"));
                                                    ImGui::SameLine(); ImGui::InputFloat("##kbtimeseparator", &eft_time_separator, 0.0f, 0.0f, "%.0f");
                                                    TextHM(xor ("Time separator between lag (in ms)\nMax: 150"));
                                                    if (eft_time_separator < 1) eft_time_separator = 1; else if (eft_time_separator > 150) eft_time_separator = 150;
                                                    if (hold_key == 0) {
                                                        ImGui::Text(xor ("Iterations:"));
                                                        ImGui::SameLine(); ImGui::InputFloat("##eftrepeattime", &eft_repeat_time, 1.0f, 0.0f, "%.0f");
                                                        TextHM(xor ("Number of lag duration iterations\nExample: 1000 ms duration * 3 iterations = 3000 ms\nMin: 1\nMax: 10"));
                                                        if (eft_repeat_time < 1) eft_repeat_time = 1; else if (eft_repeat_time > 10) eft_repeat_time = 10;
                                                    }
                                                    else {
                                                        ImGui::Text(xor ("Iterations:")); ImGui::SameLine(); ImGui::TextDisabled("%.0f", eft_repeat_time);
                                                        ImGui::Spacing();
                                                    }
                                                }
                                            }
                                            else {
                                                ImGui::Text(xor ("Kick bypass:"));
                                                ImGui::SameLine(); ImGui::TextDisabled(xor ("off"));
                                                TextHM(xor ("Bypass kicking from EFT - Escape From Tarkov game.\nTo activate the feature, enable \"UDP bypass\""));
                                            }
                                        }
                                        else if (mode_option == 0 && switch_packages == 0) {
                                            ImGui::Text(xor ("UDP bypass:"));
                                            ImGui::SameLine(); ImGui::Checkbox(xor ("###Outgoingbypassalt"), &outg_bypass_alt);
                                            TextHM(xor ("UDP bypass protection of outgoing packets.\n(For UDP games: EFT, DayZ and etc.)\nATTENTION! Blocks all outgoing connections to the system."));
                                        }
                                        else if (mode_option == 0 && switch_packages == 2) {
                                            ImGui::Text(xor ("UDP bypass:"));
                                            ImGui::SameLine(); ImGui::Checkbox(xor ("###Allbypassalt"), &all_bypass_alt);
                                            TextHM(xor ("UDP bypass protection of all packets.\n(For UDP games: EFT, DayZ and etc.)\nATTENTION! Blocks all connections to the system."));
                                        }
                                    }
                                }
                                ImGui::EndGroup();
                            }
                        }
                        ImGui::EndGroup();
                    }

                    if (button_menu == 4 && button_menu_exit != 1) {

                        ImGui::Text(xor ("      "));
                        ImGui::SameLine();
                        ImGui::BeginGroup();
                        {
                            ImGui::Text(xor ("      "));
                            logs.Draw(xor ("Logs:"));
                        }
                        ImGui::EndGroup();
                    }

                    if (button_menu == 5 && button_menu_exit != 1) {

                        ImGui::Text(xor ("      "));
                        ImGui::SameLine();
                        ImGui::BeginGroup();
                        {
                            ImGui::Text(xor ("      "));
                            ImGui::PushItemWidth(45);
                            {
                                ImGui::Spacing();
                                if (ImGui::Button(xor ("Save configuration"), ImVec2(140, 20))) {
                                    saveconf();
                                    logs.AddLog(xor ("\n[+] Configuration saved"));
                                    Beep(500, 200);
                                }
                                if (ImGui::Button(xor ("Reset configuration"), ImVec2(140, 20))) {
                                    resetconf();
                                    logs.AddLog(xor ("\n[+] Configuration reseted"));
                                    Beep(500, 200);
                                }
                                ImGui::Spacing();
                                ImGui::BulletText(xor ("Randomization:"));
                                ImGui::SameLine(); ImGui::Checkbox(xor ("###randomization"), &rnd_activate);
                                TextHM(xor ("Randomization lag switch duration & fake lag delay.\nMax: 1000\nMin: 10"));
                                ImGui::SameLine(); ImGui::Text(xor ("            "));
                                if (rnd_activate == 1) {
                                    ImGui::Spacing();
                                    ImGui::BeginGroup();
                                    {
                                        ImGui::BulletText(xor ("Lag Switch: "));
                                        ImGui::Text(xor ("Min: ")); ImGui::SameLine(); ImGui::InputInt(xor ("###minrndls"), &switch_rnd_min, 0, 0);
                                        if (switch_rnd_min <= 9) switch_rnd_min = 10;
                                        if (switch_rnd_min > switch_rnd_max) switch_rnd_min = switch_rnd_max - 1;
                                        ImGui::Text(xor ("Max: ")); ImGui::SameLine(); ImGui::InputInt(xor ("###maxrndls"), &switch_rnd_max, 0, 0);
                                        if (switch_rnd_max > 1000) switch_rnd_max = 1000;
                                        if (switch_rnd_max < switch_rnd_min) switch_rnd_max = switch_rnd_min + 1;
                                    }
                                    ImGui::EndGroup();
                                    ImGui::SameLine();
                                    ImGui::BeginGroup();
                                    {
                                        ImGui::BulletText(xor ("Fake Lag: "));
                                        ImGui::Text(xor ("Min: ")); ImGui::SameLine(); ImGui::InputInt(xor ("###minrndfl"), &fake_rnd_min, 0, 0);
                                        if (fake_rnd_min <= 9) fake_rnd_min = 10;
                                        if (fake_rnd_min > fake_rnd_max) fake_rnd_min = fake_rnd_max - 1;
                                        ImGui::Text(xor ("Max: ")); ImGui::SameLine(); ImGui::InputInt(xor ("###maxrndfl"), &fake_rnd_max, 0, 0);
                                        if (fake_rnd_max > 1000) fake_rnd_max = 1000;
                                        if (fake_rnd_max < fake_rnd_min) fake_rnd_max = fake_rnd_min + 1;
                                    }
                                    ImGui::EndGroup();
                                    ImGui::Text(xor ("Last random value:")); ImGui::SameLine();
                                    ImGui::TextDisabled(to_string(randint).c_str());
                                }
                            }
                        }
                        ImGui::EndGroup();
                        ImGui::SameLine();
                        ImGui::BeginGroup();
                        {
                            ImGui::Text(xor ("      "));
                            ImGui::Text(xor ("GLS version: %s"), version); ImGui::SameLine(); ImGui::TextDisabled(xor ("Since 2019 (c)"));
                            ImGui::Spacing();
                            ImGui::Spacing();
                            ImGui::Text(xor ("  ")); ImGui::SameLine();
                            if (ImGui::Button(xor ("Site & instructions"), ImVec2(180, 20))) {
                                ShellExecute(NULL, "open", "https://lagswitch.org/", NULL, NULL, SW_SHOWNORMAL);
                            }
                            ImGui::Spacing();
                            ImGui::Spacing();
                            ImGui::Text(xor ("  ")); ImGui::SameLine();
                            if (ImGui::Button(xor ("Developer Github"), ImVec2(180, 20))) {
                                ShellExecute(NULL, "open", "https://github.com/xvorost", NULL, NULL, SW_SHOWNORMAL);
                            }
                        }
                        ImGui::EndGroup();
                    }

                    if (button_menu_exit == 1) {
                        ImGui::Text("      ");
                        ImGui::SameLine();
                        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.769f, 0.349f, 0.333f, 1.000f));
                        if (ImGui::Button(xor ("<< Back"), ImVec2(80, 20))) {
                            button_menu_exit = 0;
                        }
                        ImGui::PopStyleColor(1);
                        ImGui::Text(xor ("      "));
                        ImGui::SameLine();
                        ImGui::BeginGroup();
                        {
                            ImGui::Text("      ");
                            ImGui::Text("      ");
                            ImGui::Text("      ");
                            ImGui::Text("      ");
                            ImGui::Text("      ");
                            ImGui::Text("                ");
                            ImGui::SameLine();
                            ImGui::BeginGroup();
                            {
                                ImGui::Text(xor ("Save configuration before exit?"));
                                ImGui::Spacing();
                                ImGui::Text("    ");
                                ImGui::SameLine();
                                if (ImGui::Button(xor ("Yes"), ImVec2(60, 20))) {
                                    saveconf(); exit(1);
                                }
                                ImGui::SameLine();
                                ImGui::Text(xor ("  "));
                                ImGui::SameLine();
                                if (ImGui::Button(xor ("No"), ImVec2(60, 20))) {
                                    exit(1);
                                }
                            }
                            ImGui::EndGroup();
                        }
                        ImGui::EndGroup();
                    }
                }
            }


            ImGui::End();
        }

        // Rendering
        if (cwr >= 1000) { ::UpdateWindow(hwnd); cwr = 0; }
        ImGui::EndFrame();
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
        D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * clear_color.w * 255.0f), (int)(clear_color.y * clear_color.w * 255.0f), (int)(clear_color.z * clear_color.w * 255.0f), (int)(clear_color.w * 255.0f));
        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }
        HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

        // Handle loss of D3D9 device
        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
            ResetDevice();

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // Cleanup
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClass(wc.lpszClassName, wc.hInstance);
    //}

    return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
        return false;

    // Create the D3DDevice
    g_d3dpp.Windowed = true;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.hDeviceWindow = hWnd;

    HRESULT dummyDeviceCreated = g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_d3dpp.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice);
    if (dummyDeviceCreated != S_OK)
    {
        g_pD3D->Release();
        Sleep(5000);
        return false;
    }

    return true;
}

void CleanupDeviceD3D()
{
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
    if (g_pD3D) { g_pD3D->Release(); g_pD3D = NULL; }
}

void ResetDevice()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            g_d3dpp.BackBufferWidth = LOWORD(lParam);
            g_d3dpp.BackBufferHeight = HIWORD(lParam);
            ResetDevice();
        }
        return 0;
        break;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    case WM_LBUTTONDOWN:
        position = MAKEPOINTS(lParam); // set click points
        return 0;
    case WM_MOUSEMOVE:
        if (wParam == MK_LBUTTON)
        {
            const auto points = MAKEPOINTS(lParam);
            auto rect = ::RECT{ };

            GetWindowRect(hwnd, &rect);

            rect.left += points.x - position.x;
            rect.top += points.y - position.y;

            if (position.x >= 0 &&
                position.x <= WIDTH &&
                position.y >= 0 && position.y <= 19)
                SetWindowPos(
                    hwnd,
                    HWND_TOPMOST,
                    rect.left,
                    rect.top,
                    0, 0,
                    SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER
                );
        }
        return 0;
    }

    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

std::string tm_to_readable_time(tm ctx) {
    char buffer[80];

    strftime(buffer, sizeof(buffer), "%d/%m/%y", &ctx);
    //strftime(buffer, sizeof(buffer), "%a %d/%m/%y %H:%M:%S %Z", &ctx);

    return std::string(buffer);
}

std::string tm_to_readable_time2(tm ctx) {
    char buffer[80];

    strftime(buffer, sizeof(buffer), "%d/%m/%y", &ctx);
    //strftime(buffer, sizeof(buffer), "%a %d/%m/%y %H:%M:%S %Z", &ctx);

    return buffer;
}

static std::time_t string_to_timet(std::string timestamp) {
    auto cv = strtol(timestamp.c_str(), NULL, 10); // long

    return (time_t)cv;
}

static std::tm timet_to_tm(time_t timestamp) {
    std::tm context;

    localtime_s(&context, &timestamp);

    return context;
}
