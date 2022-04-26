#include <Windows.h>
#include <cstdint>
#include <iostream>
#include "Memory.h"
#include "Instance.h"
#include "Vector3.h"
#include "RoBeats.h"
#include "RbxInput.h"
using namespace RoBeats;
HANDLE hPipe = INVALID_HANDLE_VALUE;
bool running = false;

void SendNamedPipeMessage(string msg) {
	if (hPipe != INVALID_HANDLE_VALUE) {
		WriteFile(hPipe, (msg + '\n').c_str(), msg.length() + 1, 0, 0);
	}
}

void mainLoop() {
	vector<Adornment> watch;
	bool inGame = false, focused = true;

	for (const Instance& inst : workspace.GetDescendants()) {
		if (inst.GetClass() == "CylinderHandleAdornment" || inst.GetClass() == "SphereHandleAdornment") {
			watch.emplace_back(inst.GetAddress());
		}
	}

	SendNamedPipeMessage("Attached");

	while (IsWindow(hwnd) && running) {
		if (GetForegroundWindow() != hwnd || guiService.IsMenuOpen()) {
			if (focused) {
				focused = false;
				Reset();
			}
			continue;
		}
		else {
			focused = true;
		}

		bool isCameraScriptable = workspace.CurrentCamera.IsScriptable();

		if (!inGame && isCameraScriptable) {
			Sleep(2000);
			Vector3 camPos = workspace.CurrentCamera.GetPosition();
			lanePositions = abs(camPos.z - InternalConfig::singleplayerLanePositions[0].z) < abs(camPos.z - InternalConfig::multiplayerLanePositions[0].z) ? InternalConfig::singleplayerLanePositions : InternalConfig::multiplayerLanePositions;
			inGame = config.ManualKeys || GrabKeys();
		}
		else if (inGame && !isCameraScriptable) {
			inGame = false;
			Reset();
		}

		if (!inGame) {
			continue;
		}

		for (int i = 0; i < 4; i++) {
			if (delays[i] > 0) {
				delays[i]--;
				if (!delays[i]) {
					RbxInput::Release(keys[i]);
				}
			}
		}

		for (Adornment& inst : watch) {
			inst.Update();

			if (!inst.CFrameChanged || !inst.Visible) {
				continue;
			}

			int lane = GetNearestLane(inst.Position);

			if (CanHit(inst, lane)) {
				RbxInput::Press(keys[lane]);
				if (inst.IsCylinder) {
					delays[lane] = RandomDelay();
				}
			}
			else if (CanRelease(inst, lane)) {
				RbxInput::Release(keys[lane]);
				delays[lane] = 8;
			}
		}

		Sleep(1);
	}

	SendNamedPipeMessage("Detached");
}

void InitNamedPipe() {
	DWORD dwWritten;

	hPipe = CreateFile(TEXT("\\\\.\\pipe\\RoBeatsPipe"), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);

	SendNamedPipeMessage("Initialized");
}

extern "C" {
	__declspec(dllexport) int Inject() {
		if (hPipe == INVALID_HANDLE_VALUE) {
			InitNamedPipe();
		}

		OpenRoblox();
		if (!hProc) {
			return 1;
		}

		uintptr_t workspace_vftable = Read<uintptr_t>(ScanSignature("C7 07 ? ? ? ? C7 47 ? ? ? ? ? C7 87 ? ? ? ? ? ? ? ? C7 87 ? ? ? ? ? ? ? ? C7 87 ? ? ? ? ? ? ? ? C7 87 ? ? ? ? ? ? ? ? C7 87 ? ? ? ? ? ? ? ? 8B 40 04 C7 44 38 ? ? ? ? ? 8B 47 6C 8B 48 04 8D 81 ? ? ? ? 89 44 39 68 80 3D ? ? ? ? ?", 2));
		uintptr_t workspaceAddr = ScanVFTable(workspace_vftable);

		if (!workspaceAddr) {
			return 2;
		}

		Instance _game = Instance(workspaceAddr).GetParent();

		if (_game.GetClass() != "DataModel") {
			return 3;
		}

		RoBeats::game = _game;
		RoBeats::workspace = Workspace(game.FindFirstChildOfClass("Workspace"));
		RoBeats::guiService = GuiService(game.FindFirstChildOfClass("GuiService"));

		running = true;
		Reset();
		mainLoop();

		return 0;
	}

	__declspec(dllexport) void Detach() {
		running = false;
	}

	__declspec(dllexport) void SetOptions(ExternalConfig config) {
		RoBeats::config = config;
		for (int i = 0; i < 4; i++) {
			keys[i] = string(config.Keys[i]);
		}
	}
}