#define _CRT_SECURE_NO_WARNINGS 1
#include "util.h"

extern bool fop;
extern char szFile2[260];
extern char szFile3[21];

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
extern bool all_bypass_alt;
extern bool inc_bypass_alt;
extern bool outg_bypass_alt;
extern KeyBindToggle activate_duration_bind;
extern KeyBindToggle activate_gls_bind;

extern char szFile1[260];

extern bool rnd_activate;
extern int switch_rnd_min;
extern int switch_rnd_max;
extern int fake_rnd_min;
extern int fake_rnd_max;

extern bool eft_bypass;
extern float lagswitchtime_3;
extern float eft_time_separator;
extern float eft_repeat_time;

/*
void downloadFile(std::string keyauthID, std::string fileOutput) // Example : downloadFile(175182, "C://file.exe"
{
	std::vector<std::uint8_t> bytes = FuckKAP.download(keyauthID);
	std::ofstream file(fileOutput, std::ios_base::out | std::ios_base::binary);
	file.write((char*)bytes.data(), bytes.size());
	file.close();
}
*/

const std::string randomString(size_t length)
{
	auto randchar = []() -> char
	{
		const char charset[] =
			"0123456789"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz";
		const size_t max_index = (sizeof(charset) - 1);
		return charset[rand() % max_index];
	};
	std::string str(length, 0);
	std::generate_n(str.begin(), length, randchar);
	return str;
}

const std::string Ppath()
{
	char path1[_MAX_PATH];
	GetModuleFileNameA(NULL, path1, _MAX_PATH);
	return std::string(path1);
}

TCHAR* tcharp(std::string name) {
	const char* Path = xor ("C:\\gls\\");
	std::string file = Path + name;
	TCHAR* f = 0;
	f = new TCHAR[file.size() + 1];
	strcpy(f, file.c_str());
	return f;
}

void saveconf() {
    std::ifstream iff;
    iff.open(xor ("C:\\gls\\config.txt"));
    iff.close();
    if (!iff) {
        std::string directory = xor ("C:\\gls");
        CreateDirectory(directory.c_str(), NULL);
        directory.append(xor ("\\config.txt"));
    }
    remove(xor ("C:\\gls\\config.txt"));
    HANDLE h_file = CreateFile(xor ("C:\\gls\\config.txt"), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    CloseHandle(h_file);

    int numerator[] = {
        activate_gls, mode_option, lagswitchtime, lagswitchtime_2, fakelagtime, fakelagtime_2, activate_sounds, hold_key,
        switch_packages, all_bypass_alt, inc_bypass_alt, rnd_activate, outg_bypass_alt, switch_rnd_min, switch_rnd_max,
        fake_rnd_min, fake_rnd_max, eft_bypass, lagswitchtime_3, eft_time_separator, eft_repeat_time
    };

    string numerator_s[] = {
        activate_duration_bind.toString(), activate_gls_bind.toString()
    };

    fstream f;
    f.open(xor ("C:\\gls\\config.txt"), ios::out);
    for (int i = 0; i <= 27; i++) { //16
        if (i == 0) f << xor ("[SETTINGS]") << endl;
        else if (i > 0 && i <= 11) f << numerator[i - 1] << endl;
        else if (i > 11 && i <= 13) f << numerator_s[i - 12] << endl;
        else if (i == 14) f << szFile1 << endl;
        else if (i == 15) f << xor ("[VERSION]") << endl;
        else if (i == 16) f << version << endl;
        else if (i == 17) f << xor ("[SETTINGS-2]") << endl;
        else if (i > 17 && i <= 23) f << numerator[i - 7] << endl;
        else if (i > 23 && i <= 27) f << numerator[i - 7] << endl;
    }
    f.close();
}

void resetconf() {
    remove(xor ("C:\\gls\\config.txt"));
    HANDLE h_file = CreateFile(xor ("C:\\gls\\config.txt"), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    CloseHandle(h_file);

    activate_gls = 0;
    mode_option = 0;
    lagswitchtime = 1600;
    lagswitchtime_2 = 750;
    fakelagtime = 500;
    fakelagtime_2 = 1000;
    activate_sounds = 1;
    hold_key = 0;
    switch_packages = 1;
    activate_duration_bind = KeyBindToggle(KeyBind::KeyCode::CAPSLOCK);
    activate_gls_bind = KeyBindToggle(KeyBind::KeyCode::INSERT);

    all_bypass_alt = 1;
    inc_bypass_alt = 1;
    outg_bypass_alt = 1;

    memset(szFile1, 0, sizeof szFile1);
    memset(szFile2, 0, sizeof szFile2);
    szFile2[5] = { '*' };
    memset(szFile3, 0, sizeof szFile3);
    fop = 0;

    rnd_activate = 1;
    switch_rnd_min = 50;
    switch_rnd_max = 300;
    fake_rnd_min = 1000 + 232 - 950 - 232;
    fake_rnd_max = 200;

    int numerator[] = {
        activate_gls, mode_option, lagswitchtime, lagswitchtime_2, fakelagtime, fakelagtime_2, activate_sounds, hold_key,
        switch_packages, all_bypass_alt, inc_bypass_alt, rnd_activate, outg_bypass_alt, switch_rnd_min, switch_rnd_max,
        fake_rnd_min, fake_rnd_max
    };

    string numerator_s[] = {
        activate_duration_bind.toString(), activate_gls_bind.toString()
    };

    fstream f;
    f.open(xor ("C:\\gls\\config.txt"), ios::out);
    for (int i = 0; i <= 23; i++) { //16
        if (i == 0) f << xor ("[SETTINGS]") << endl;
        else if (i > 0 && i <= 11) f << numerator[i - 1] << endl;
        else if (i > 11 && i <= 13) f << numerator_s[i - 12] << endl;
        else if (i == 14) f << szFile1 << endl;
        else if (i == 15) f << xor ("[VERSION]") << endl;
        else if (i == 16) f << version << endl;
        else if (i == 17) f << xor ("[RANDOMIZATION]") << endl;
        else if (i > 17 && i <= 23) f << numerator[i - 7] << endl;
    }
    f.close();
}

char* HashMD5(char* data, DWORD* result)
{
	DWORD dwStatus = 0;
	DWORD cbHash = 16;
	int i = 0;
	HCRYPTPROV cryptProv;
	HCRYPTHASH cryptHash;
	BYTE hash[16];
	const char* hex = "0123456789abcdef";
	char* strHash;
	strHash = (char*)malloc(500);
	memset(strHash, '\0', 500);
	if (!CryptAcquireContext(&cryptProv, NULL, MS_DEF_PROV, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
	{
		dwStatus = GetLastError();
		//printf("CryptAcquireContext failed: %d\n", dwStatus);
		*result = dwStatus;
		return NULL;
	}
	if (!CryptCreateHash(cryptProv, CALG_MD5, 0, 0, &cryptHash))
	{
		dwStatus = GetLastError();
		//printf("CryptCreateHash failed: %d\n", dwStatus);
		CryptReleaseContext(cryptProv, 0);
		*result = dwStatus;
		return NULL;
	}
	if (!CryptHashData(cryptHash, (BYTE*)data, strlen(data), 0))
	{
		dwStatus = GetLastError();
		//printf("CryptHashData failed: %d\n", dwStatus);
		CryptReleaseContext(cryptProv, 0);
		CryptDestroyHash(cryptHash);
		*result = dwStatus;
		return NULL;
	}
	if (!CryptGetHashParam(cryptHash, HP_HASHVAL, hash, &cbHash, 0))
	{
		dwStatus = GetLastError();
		//printf("CryptGetHashParam failed: %d\n", dwStatus);
		CryptReleaseContext(cryptProv, 0);
		CryptDestroyHash(cryptHash);
		*result = dwStatus;
		return NULL;
	}
	for (i = 0; i < cbHash; i++)
	{
		strHash[i * 2] = hex[hash[i] >> 4];
		strHash[(i * 2) + 1] = hex[hash[i] & 0xF];
	}
	CryptReleaseContext(cryptProv, 0);
	CryptDestroyHash(cryptHash);
	return strHash;
}

std::string getHDDSerial() {
    DWORD disk_serialINT;
    GetVolumeInformationA(NULL, NULL, NULL, &disk_serialINT, NULL, NULL, NULL, NULL);
    return std::to_string(disk_serialINT);
}

std::string getHardwareID() {
    HW_PROFILE_INFO hwProfileInfo;
    GetCurrentHwProfile(&hwProfileInfo);
	DWORD err;
	std::string hd = getHDDSerial() + ":" + hwProfileInfo.szHwProfileName + ":" + hwProfileInfo.szHwProfileGuid;
	std::string hash = HashMD5((char*)hd.c_str(), &err);
	return hash;
}

int convert(std::string string) {
    ////// Remove Comments
    string = string.substr(0, string.find(" ", 0));

    /////// Convert string to Char, then Char to Int
    const char* Char = string.c_str();
    int offset = (int)strtol(Char, NULL, 16);

    return offset;
}