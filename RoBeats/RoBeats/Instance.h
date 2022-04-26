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
	vector<Instance> GetChildren() const;
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

	inline int GetShape() const {
		float shape = Read<float>(addr + 0x100);
		return shape == 1 ? 0 : (shape == 257 ? 1 : 2);
	}

	 Vector3 GetPosition() const {
		float v[3];
		Read(Read<uintptr_t>(addr + 0xC0) + 0x104, v, sizeof(float) * 3);
		return { v[0], v[1], v[2] };
	}
};

struct Camera : Instance {
	using Instance::Instance;

	Vector3 GetPosition() const {
		float v[3];
		Read(addr + 0xAC, v, sizeof(float) * 3);
		return { v[0], v[1], v[2] };
	}

	bool IsScriptable () {
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
		return Read<bool>(addr + 0x1B0);
	}
};

struct Adornment : Instance {
	using Instance::Instance;

	bool Visible = false;
	Vector3 Position{ 0, 0, 0 };
	float Transparency = 0, Height = 0, Radius = 0;
	bool CFrameChanged = false;
	bool IsCylinder;

	void Update() {
		Vector3 lastPos = Position;
		float v[3];
		Read(addr + 0xF8, v, sizeof(float) * 3);
		Position = { v[0], v[1], v[2] };
		CFrameChanged = Position != lastPos;

		Transparency = Read<float>(addr + 0x98);
		Radius = Read<float>(addr + 0x110);
		if (IsCylinder) {
			Height = Read<float>(addr + 0x114);
		}
		Visible = Read<bool>(addr + 0x9C);
	}

	Adornment(uintptr_t addr) : Instance(addr) {
		IsCylinder = GetClass() == "CylinderHandleAdornment";
	}
};