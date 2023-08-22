#pragma once
//#include "keyauth.h"
#include <random>
#include <filesystem>
#include <string>
#include <Windows.h>
#include "xor.h"
#include <iostream>
#include <vector>
#include <fstream>
#include "InputUtil.h"

//void downloadFile(std::string keyauthID, std::string fileOutput);
const std::string randomString(size_t length);
const std::string Ppath();
TCHAR* tcharp(std::string name);
void saveconf();
void resetconf();
std::string getHDDSerial();
std::string getHardwareID();
int convert(std::string string);