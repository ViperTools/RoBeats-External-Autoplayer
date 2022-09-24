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

void SendEvent(string msg) {
	if (hPipe != INVALID_HANDLE_VALUE) {
		WriteFile(hPipe, (msg + '\n').c_str(), msg.length() + 1, 0, 0);
	}
}

void MainLoop() {
	vector<Adornment> watch;
	bool inGame = false, focused = true;

	for (const Instance& inst : workspace.GetDescendants()) {
		if (inst.GetClass() == "CylinderHandleAdornment") {
			watch.emplace_back(inst.GetAddress());
		}
	}

	SendEvent("Attached");

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
			Sleep(1000);
			if (inGame = (config.ManualKeys || GrabKeys()) && floor(workspace.CurrentCamera.GetPosition().y * 10) == InternalConfig::gameStartCameraY) {
				UpdateLanePositions();
			}
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
				else if (delays[i] < 0) { // For slider debounce since it is set to a decimal
					delays[i] = 0;
				}
			}
		}

		for (Adornment& inst : watch) {
			inst.Update();

			if (!inst.CFrameChanged || !inst.Visible) {
				continue;
			}

			Lane lane = GetNearestLane(inst.Position);

			if (CanHit(inst, lane)) {
				RbxInput::Press(keys[lane.Lane]);
				if (IsSlider(inst)) {
					heldLanes[lane.Lane] = true;
				}
				else {
					delays[lane.Lane] = RandomDelay();
				}
			}
			else if (CanRelease(inst, lane)) {
				heldLanes[lane.Lane] = false;
				delays[lane.Lane] = InternalConfig::sliderDebounce; // Make it a decimal to prevent release check from passing
				RbxInput::Release(keys[lane.Lane]);
			}
		}

		Sleep(1);
	}

	SendEvent("Detached");
}

extern "C" {
	__declspec(dllexport) int Inject() {
		if (hPipe == INVALID_HANDLE_VALUE) {
			hPipe = CreateFile(TEXT("\\\\.\\pipe\\RoBeatsPipe"), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
			SendEvent("Initialized");
		}

		OpenRoblox();
		if (!hProc)
			return 1;

		uintptr_t workspace_vftable = Read<uintptr_t>(ScanSignature("C7 07 ? ? ? ? C7 87 98 00 00 00 ? ? ? ? C7 87 B4 00 00 00 ? ? ? ? C7 87 B4 01 00 00 ? ? ? ? C7 87 B8 01 00 00 ? ? ? ? C7 87 54 02 00 00 ? ? ? ? C7 87 58 02 00 00 ? ? ? ? 8B 40 04 C7 84 38 90 00 00 00 ? ? ? ? 8B 87 90 00 00 00 8B 48 04 8D 81 E4 FB FF FF 89 84 39 8C 00 00 00 8B 9F A0 03 00 00", 2));
		uintptr_t workspaceAddr = ScanVFTable(workspace_vftable);

		if (!workspaceAddr)
			return 2;

		Workspace workspace(workspaceAddr);
		Instance game = workspace.GetParent();

		if (game.GetClass() != "DataModel")
			return 3;

		RoBeats::game = game;
		RoBeats::workspace = workspace;
		RoBeats::guiService = GuiService(game.FindFirstChildOfClass("GuiService"));

		running = true;
		Reset();
		MainLoop();

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