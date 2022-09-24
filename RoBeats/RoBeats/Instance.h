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
		int len = Read<int>(addr + 0x664);
		if (len >= 16) {
			return ReadString(Read<uintptr_t>(addr + 0x66C), len);
		}
		return ReadString(addr + 0x66C, len);
	}
};

struct BasePart : Instance {
	using Instance::Instance;

	 Vector3 GetPosition() const {
		return Read<Vector3>(Read<uintptr_t>(addr + 0xE8) + 0xFC);
	}
};

struct Camera : Instance {
	using Instance::Instance;

	Vector3 GetPosition() {
		return Read<Vector3>(addr + 0xE8);
	}

	Vector3 GetLookVector() {
		float cframe[12];
		Read(addr + 0xC4, cframe, sizeof(float) * 12);
		return { -cframe[2], -cframe[5], -cframe[8] };
	}

	bool IsScriptable() {
		return Read<int>(addr + 0x204) == 6;
	}
};

struct Workspace : Instance {
	Camera CurrentCamera;

	Workspace(uintptr_t addr = 0) : Instance(addr) {
		CurrentCamera = Camera(Read<uintptr_t>(addr + 0x378));
	}
};

struct GuiService : Instance {
	using Instance::Instance;

	bool IsMenuOpen() {
		return Read<bool>(addr + 0x2B4);
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
		Read(addr + 0x138, cframe, sizeof(float) * 12);
		Position = { cframe[9], cframe[10], cframe[11] };
		LookVector = { -cframe[2], -cframe[5], -cframe[8] };
		CFrameChanged = Position != lastPos;
		Transparency = Read<float>(addr + 0xBC);
		Height = Read<float>(addr + 0x178);
		Visible = Read<bool>(addr + 0xC0);
	}
};