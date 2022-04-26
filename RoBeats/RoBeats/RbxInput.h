#pragma once
#include <map>
#include <string>
#include <Windows.h>
using std::string, std::map;

struct RbxInput {
	static map<string, USHORT> keycodes;
	static void Press(string key);
	static void Release(string key);
};