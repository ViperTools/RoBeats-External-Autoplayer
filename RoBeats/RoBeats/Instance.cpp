#include "Instance.h"
#include <iostream>
#include <queue>

Instance Instance::GetParent() const {
	return { Read<uintptr_t>(addr + 0x30) };
}

string Instance::GetName() const {
	return ReadString(Read<uintptr_t>(addr + 0x24));
}

string Instance::GetClass() const {
	uintptr_t desc = Read<uintptr_t>(addr + 0xC);
	return ReadString(Read<uintptr_t>(desc + 0x4));
}

vector<uintptr_t> Instance::_getChildren() const {
	vector<uintptr_t> children;
	uintptr_t childrenStart = Read<uintptr_t>(addr + 0x28);
	uintptr_t childrenEnd = Read<uintptr_t>(childrenStart + 4);

	for (uintptr_t i = Read<uintptr_t>(childrenStart); i < childrenEnd; i += 8) {
		children.push_back(Read<uintptr_t>(i));
	}

	return children;
}

vector<Instance> Instance::GetDescendants() const {
	vector<Instance> descendants;
	vector<uintptr_t> children = _getChildren();
	std::queue<uintptr_t> traverse;

	for (uintptr_t c : children) {
		traverse.push(c);
	}

	while (!traverse.empty()) {
		Instance inst{ traverse.front() };
		descendants.push_back(inst);
		traverse.pop();
		vector<uintptr_t> _children = inst._getChildren();

		for (uintptr_t c : _children) {
			traverse.push(c);
		}

		_children.clear();
	}

	return descendants;
}

Instance Instance::FindFirstChild(string name, bool recursive) const {
	vector<uintptr_t> ptrs = _getChildren();

	for (uintptr_t ptr : ptrs) {
		Instance inst(ptr);

		if (inst.GetName() == name) {
			return inst;
		}

		if (recursive) {
			Instance found = inst.FindFirstChild(name, true);

			if (found.GetAddress() > 0) {
				return found;
			}
		}
	}

	return { 0 };
}

Instance Instance::FindFirstChildOfClass(string className) const {
	vector<uintptr_t> ptrs = _getChildren();

	for (uintptr_t ptr : ptrs) {
		Instance inst(ptr);

		if (inst.GetClass() == className) {
			return inst;
		}
	}

	return { 0 };
}

uintptr_t Instance::GetAddress() const {
	return addr;
}