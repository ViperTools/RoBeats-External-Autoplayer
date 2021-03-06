#include "RbxInput.h"

map<string, USHORT> RbxInput::keycodes {{ "Backspace",  VK_BACK },{ "Tab", VK_TAB },{ "Clear", VK_CLEAR },{ "Return", VK_RETURN },{ "Pause", VK_PAUSE },{ "Escape", VK_ESCAPE },{ "Space", VK_SPACE },{ "QuotedDouble", VK_OEM_7 },{ "Comma", VK_OEM_COMMA },{ "Minus", VK_OEM_MINUS },{ "Period", VK_OEM_PERIOD },{ "Zero", 0x30 },{ "One", 0x31 },{ "Two", 0x32 },{ "Three", 0x33 },{ "Four", 0x34 },{ "Five", 0x35 },{ "Six", 0x36 },{ "Seven", 0x37 },{ "Eight", 0x38 },{ "Nine", 0x39 },{ "Semicolon", VK_OEM_1 },{ "LeftBracket", VK_OEM_4 },{ "BackSlash", VK_OEM_5 },{ "RightBracket", VK_OEM_6 },{ "BackQuote", VK_OEM_3 },{ "A", 0x41 },{ "B", 0x42 },{ "C", 0x43 },{ "D", 0x44 },{ "E", 0x45 },{ "F", 0x46 },{ "G", 0x47 },{ "H", 0x48 },{ "I", 0x49 },{ "J", 0x4A },{ "K", 0x4B },{ "L", 0x4C },{ "M", 0x4D },{ "N", 0x4E },{ "O", 0x4F },{ "P", 0x50 },{ "Q", 0x51 },{ "R", 0x52 },{ "S", 0x53 },{ "T", 0x54 },{ "U", 0x55 },{ "V", 0x56 },{ "W", 0x57 },{ "X", 0x58 },{ "Y", 0x59 },{ "Z", 0x5A },{ "Delete", VK_DELETE },{ "KeypadZero", VK_NUMPAD0 },{ "KeypadOne", VK_NUMPAD1 },{ "KeypadTwo", VK_NUMPAD2 },{ "KeypadThree", VK_NUMPAD3 },{ "KeypadFour", VK_NUMPAD4 },{ "KeypadFive", VK_NUMPAD5 },{ "KeypadSix", VK_NUMPAD6 },{ "KeypadSeven", VK_NUMPAD7 },{ "KeypadEight", VK_NUMPAD8 },{ "KeypadNine", VK_NUMPAD9 },{ "KeypadDivide", VK_DIVIDE },{ "KeypadMultiply", VK_MULTIPLY },{ "KeypadMinus", VK_SUBTRACT },{ "KeypadPlus", VK_ADD },{ "KeypadEnter", VK_RETURN },{ "Up", VK_UP },{ "Down", VK_DOWN },{ "Right", VK_RIGHT },{ "Left", VK_LEFT },{ "Insert", VK_INSERT },{ "Home", VK_HOME },{ "End", VK_END },{ "PageUp", VK_PRIOR },{ "PageDown", VK_NEXT },{ "LeftShift", VK_LSHIFT },{ "RightShift", VK_RSHIFT },{ "LeftAlt", VK_LMENU },{ "RightAlt", VK_RMENU },{ "LeftControl", VK_LCONTROL },{ "RightControl", VK_RCONTROL },{ "CapsLock", VK_CAPITAL },{ "NumLock", VK_NUMLOCK },{ "ScrollLock", VK_SCROLL },{ "F1", VK_F1 },{ "F2", VK_F2 },{ "F3", VK_F3 },{ "F4", VK_F4 },{ "F5", VK_F5 },{ "F6", VK_F6 },{ "F7", VK_F7 },{ "F8", VK_F8 },{ "F9", VK_F9 },{ "F10", VK_F10 },{ "F11", VK_F11 },{ "F12", VK_F12 },{ "F13", VK_F13 },{ "F14", VK_F14 },{ "F15", VK_F15 }};

void RbxInput::Press(string key) {
	INPUT input;
	input.type = INPUT_KEYBOARD;
	input.ki.wScan = MapVirtualKeyA(keycodes[key], 0);
	input.ki.dwFlags = KEYEVENTF_SCANCODE;
	SendInput(1, &input, sizeof(input));
}

void RbxInput::Release(string key) {
	INPUT input;
	input.type = INPUT_KEYBOARD;
	input.ki.wScan = MapVirtualKeyA(keycodes[key], 0);
	input.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
	SendInput(1, &input, sizeof(input));
}