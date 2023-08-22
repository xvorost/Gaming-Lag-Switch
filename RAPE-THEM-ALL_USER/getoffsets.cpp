#define _CRT_SECURE_NO_WARNINGS 1

#include <iostream>
#include <Windows.h>
#include <fstream>
#include <string>
#include <string_view>
#include <sstream>
#include <urlmon.h>
#pragma comment (lib, "urlmon.lib")

#include "print.h"
#include "util.h"

#include "xor.h"
#include <tchar.h>
#include "ImGuiLogs.h"

using namespace std;

extern const char* version;

extern bool activate_gls;
extern int mode_option;
extern float lagswitchtime;
extern float lagswitchtime_2;
extern float fakelagtime;
extern float fakelagtime_2;
extern bool activate_sounds;
extern bool hold_key;
extern int switch_packages;
extern bool inc_bypass_alt;
extern bool outg_bypass_alt;
extern bool all_bypass_alt;
extern KeyBindToggle activate_duration_bind;
extern KeyBindToggle activate_gls_bind;

extern char szFile1[260];
extern char szFile3[13];

extern bool rnd_activate;
extern int switch_rnd_min;
extern int switch_rnd_max;
extern int fake_rnd_min;
extern int fake_rnd_max;

extern bool eft_bypass;
extern float lagswitchtime_3;
extern float eft_time_separator;
extern float eft_repeat_time;

string s_activate_gls;
string s_mode_option;
string s_lagswitchtime;
string s_lagswitchtime_2;
string s_fakelagtime;
string s_fakelagtime_2;
string s_activate_sounds;
string s_hold_key;
string s_switch_packages;
string s_inc_bypass_alt;
string s_outg_bypass_alt;
string s_all_bypass_alt;
string s_activate_duration_bind;
string s_activate_gls_bind;

string s_szFile1;

string s_version;

string s_rnd_activate;
string s_switch_rnd_min;
string s_switch_rnd_max;
string s_fake_rnd_min;
string s_fake_rnd_max;

string s_eft_bypass;
string s_lagswitchtime_3;
string s_eft_time_separator;
string s_eft_repeat_time;

string musor = "";

extern Log logs;
extern bool prelogs;

LPTSTR ExtractFilePath1(LPCTSTR FileName, LPTSTR buf)
{
    int i, len = lstrlen(FileName);
    for (i = len - 1; i >= 0; i--)
    {
        if (FileName[i] == _T('\\'))
            break;
    }
    lstrcpyn(buf, FileName, i + 2);
    return buf;
}

const char* keyb;

void getoffsets() {
    ifstream offsets;
    offsets.open(xor ("C:\\gls\\config.txt"));
    if (offsets.is_open()) {
        logs.AddLog(xor ("[!] GLS config found"));
        logs.AddLog(xor ("\n[!] Loading GLS config, wait..."));

        while (offsets.good()) {
            getline(offsets, musor);/////////
            getline(offsets, s_activate_gls);
            getline(offsets, s_mode_option);
            getline(offsets, s_lagswitchtime);
            getline(offsets, s_lagswitchtime_2);
            getline(offsets, s_fakelagtime);
            getline(offsets, s_fakelagtime_2);
            getline(offsets, s_activate_sounds);
            getline(offsets, s_hold_key);
            getline(offsets, s_switch_packages);
            getline(offsets, s_all_bypass_alt);
            getline(offsets, s_inc_bypass_alt);
            getline(offsets, s_activate_duration_bind);
            getline(offsets, s_activate_gls_bind);
            getline(offsets, s_szFile1);
            getline(offsets, musor);/////////
            getline(offsets, s_version);
            getline(offsets, musor);/////////
            getline(offsets, s_rnd_activate);
            getline(offsets, s_outg_bypass_alt);
            getline(offsets, s_switch_rnd_min);
            getline(offsets, s_switch_rnd_max);
            getline(offsets, s_fake_rnd_min);
            getline(offsets, s_fake_rnd_max);
            getline(offsets, s_eft_bypass);
            getline(offsets, s_lagswitchtime_3);
            getline(offsets, s_eft_time_separator);
            getline(offsets, s_eft_repeat_time);
        }
        offsets.close();

        if (version[0] == s_version[0] && version[2] == s_version[2]) {
            activate_gls = atoi(s_activate_gls.c_str());//convert(s_activate_gls);
            mode_option = atoi(s_mode_option.c_str());//convert(s_mode_option);
            lagswitchtime = atoi(s_lagswitchtime.c_str());//convert(s_lagswitchtime);
            lagswitchtime_2 = atoi(s_lagswitchtime_2.c_str());//convert(s_lagswitchtime_2);
            fakelagtime = atoi(s_fakelagtime.c_str());//convert(s_fakelagtime);
            fakelagtime_2 = atoi(s_fakelagtime_2.c_str());//convert(s_fakelagtime);
            activate_sounds = atoi(s_activate_sounds.c_str());//convert(s_activate_sounds);
            hold_key = atoi(s_hold_key.c_str());//convert(s_hold_key);
            switch_packages = atoi(s_switch_packages.c_str());
            all_bypass_alt = atoi(s_all_bypass_alt.c_str());
            inc_bypass_alt = atoi(s_inc_bypass_alt.c_str());
            keyb = s_activate_duration_bind.c_str();
            activate_duration_bind = KeyBindToggle(keyb);
            keyb = s_activate_gls_bind.c_str();
            activate_gls_bind = KeyBindToggle(keyb);
            strcpy(szFile1, s_szFile1.c_str());

            for (int i = 0; i <= 20; i++)
                if (i >= 0 && i <= 18) szFile3[i] = szFile1[i];
                else if (i > 18 && i <= 20) szFile3[i] = '.';

            rnd_activate = atoi(s_rnd_activate.c_str());
            outg_bypass_alt = atoi(s_outg_bypass_alt.c_str());
            switch_rnd_min = atoi(s_switch_rnd_min.c_str());
            switch_rnd_max = atoi(s_switch_rnd_max.c_str());
            fake_rnd_min = atoi(s_fake_rnd_min.c_str());
            fake_rnd_max = atoi(s_fake_rnd_max.c_str());
            eft_bypass = atoi(s_eft_bypass.c_str());
            lagswitchtime_3 = atoi(s_lagswitchtime_3.c_str());
            eft_time_separator = atoi(s_eft_time_separator.c_str());
            eft_repeat_time = atoi(s_eft_repeat_time.c_str());

            logs.AddLog(xor ("\n[+] GLS config loaded successfully\n"));
        }
        else {
            logs.AddLog(xor ("\n[!] You have a new version of GLS.\nOld configuration removed to avoid bugs\n- Past installed version of GLS: %s"), s_version);
            logs.AddLog(xor ("\n- Installed current version of GLS: %s"), version);
            logs.AddLog(xor ("\n[!] Deleting the old GLS configuration.."));
            remove("C:\\gls\\config.txt");
            logs.AddLog(xor ("\n[!] Creating a new GLS config, wait..."));

            saveconf();

            logs.AddLog(xor ("\n[+] New GLS config created successfully\n"));
        }
    }
    else {
        logs.AddLog(xor ("[-] GLS config not found"));
        logs.AddLog(xor ("\n[!] Creating a GLS config, wait..."));

        saveconf();

        logs.AddLog(xor ("\n[+] GLS config created successfully\n"));
    }
}