#pragma once
#include "Memory.h"
#include "Vector3.h"
#include <string>
#include <vector>
using std::string;
using std::vector;

class Instance
{
public:
	Instance GetParent() const;
	string GetName() const;
	string GetClass() const;
	vector<Instance> GetDescendants() const;
	Instance FindFirstChild(string name, bool recursive = false) const;
	Instance FindFirstChildOfClass(string className) const;
	uintptr_t GetAddress() const;
	operator uintptr_t() {
		return GetAddress();
	};
	virtual ~Instance() {};
	Instance()
		: addr(0)
	{};
	Instance(uintptr_t addr)
		: addr(addr)
	{};
protected:
	vector<uintptr_t> _getChildren() const;
	uintptr_t addr;
};

struct TextLabel : Instance {
	using Instance::Instance;
	string GetText() const {
		int len = Read<int>(addr + 0x3C4);
		if (len >= 16) {
			return ReadString(Read<uintptr_t>(addr + 0x3CC), len);
		}
		return ReadString(addr + 0x3CC, len);
	}
};

struct BasePart : Instance {
	using Instance::Instance;

	 Vector3 GetPosition() const {
		return Read<Vector3>(Read<uintptr_t>(addr + 0xC0) + 0x104);
	}
};

struct Camera : Instance {
	using Instance::Instance;

	Vector3 GetPosition() {
		return Read<Vector3>(addr + 0xAC);
	}

	Vector3 GetLookVector() {
		float cframe[12];
		Read(addr + 0x88, cframe, sizeof(float) * 12);
		return { -cframe[2], -cframe[5], -cframe[8] };
	}

	bool IsScriptable() {
		return Read<int>(addr + 0x1C8) == 6;
	}
};

struct Workspace : Instance {
	Camera CurrentCamera;

	Workspace(uintptr_t addr = 0) : Instance(addr) {
		CurrentCamera = Camera(Read<uintptr_t>(addr + 0x310));
	}
};

struct GuiService : Instance {
	using Instance::Instance;

	bool IsMenuOpen() {
		return Read<bool>(addr + 0x1B4);
	}
};

struct Adornment : Instance {
	using Instance::Instance;

	bool Visible = false, CFrameChanged = false;
	Vector3 Position{ 0, 0, 0 }, LookVector{ 0, 0, 0 };
	float Transparency = 0, Height = 0;

	void Update() {
		Vector3 lastPos = Position;
		float cframe[12];
		Read(addr + 0xD4, cframe, sizeof(float) * 12);
		Position = { cframe[9], cframe[10], cframe[11] };
		LookVector = { -cframe[2], -cframe[5], -cframe[8] };
		CFrameChanged = Position != lastPos;
		Transparency = Read<float>(addr + 0x98);
		Height = Read<float>(addr + 0x114);
		Visible = Read<bool>(addr + 0x9C);
	}
};