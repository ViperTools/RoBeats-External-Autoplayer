#include "Instance.h"
#include <iostream>
#include <queue>

Instance Instance::GetParent() const {
	return { Read<uintptr_t>(addr + 0x34) };
}

string Instance::GetName() const {
	return ReadString(Read<uintptr_t>(addr + 0x28));
}

string Instance::GetClass() const {
	uintptr_t desc = Read<uintptr_t>(addr + 0xC);
	return ReadString(Read<uintptr_t>(desc + 0x4));
}

vector<uintptr_t> Instance::_getChildren() const {
	vector<uintptr_t> children;
	uintptr_t v4 = Read<uintptr_t>(addr + 0x2C);
	int v25 = (Read<uintptr_t>(v4 + 4) - Read<uintptr_t>(v4)) >> 3;
	children.reserve(v25);
	uintptr_t v6 = Read<uintptr_t>(v4);
	for (int i = 0; i < v25; i++) {
		children.push_back(Read<uintptr_t>(v6));
		v6 += 8;
	}
	return children;
}

vector<Instance> Instance::GetChildren() const {
	vector<uintptr_t> ptrs = _getChildren();
	vector<Instance> children;
	children.reserve(ptrs.size());
	for (uintptr_t ptr : ptrs)
		children.emplace_back(ptr);
	return children;
}

vector<Instance> Instance::GetDescendants() const {
	vector<Instance> descendants;
	vector<uintptr_t> children = _getChildren();
	std::queue<uintptr_t> traverse;
	for (uintptr_t c : children) {
		traverse.push(c);
	}
	children.clear();
	while (!traverse.empty()) {
		Instance inst{ traverse.front() };
		descendants.push_back(inst);
		traverse.pop();
		vector<uintptr_t> _children = inst._getChildren();
		for (uintptr_t c : _children)
			traverse.push(c);
		_children.clear();
	}
	return descendants;
}

Instance Instance::FindFirstChild(string name, bool recursive) const {
	vector<uintptr_t> ptrs = _getChildren();
	for (uintptr_t ptr : ptrs) {
		Instance inst(ptr);
		if (inst.GetName() == name)
			return inst;
		if (recursive) {
			Instance found = inst.FindFirstChild(name, true);
			if (found.GetAddress() > 0)
				return found;
		}
	}
	return { 0 };
}

Instance Instance::FindFirstChildOfClass(string className) const {
	vector<uintptr_t> ptrs = _getChildren();
	for (uintptr_t ptr : ptrs) {
		Instance inst(ptr);
		if (inst.GetClass() == className)
			return inst;
	}
	return { 0 };
}

uintptr_t Instance::GetAddress() const {
	return addr;
}