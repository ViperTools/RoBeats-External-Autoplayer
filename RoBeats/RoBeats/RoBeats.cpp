#define NOMINMAX
#include "RoBeats.h"
#include "RbxInput.h"
#include <limits>
#include <algorithm>
#include <random>
using namespace RoBeats;

Instance RoBeats::game;
Workspace RoBeats::workspace;
GuiService RoBeats::guiService;
Bound<float> InternalConfig::distanceBound{ 0.1, 0.8 };
Bound<int> InternalConfig::delayBound{ 8, 12 };

vector<Vector3> InternalConfig::singleplayerLanePositions {
	Vector3 { -309.01, 387.7, -181.1 },
	Vector3 { -306.87, 387.7, -178.56 },
	Vector3 { -304.53, 387.7, -176.22 },
	Vector3 { -301.99, 387.7, -174.08 }
};

vector<Vector3> InternalConfig::multiplayerLanePositions{
	Vector3 { -309.01, 387.7, -228.63 },
	Vector3 { -306.87, 387.7, -231.17 },
	Vector3 { -304.53, 387.7, -233.51 },
	Vector3 { -301.99, 387.7, -235.65 }
};

std::random_device randDevice;
std::mt19937 rng(randDevice());
std::uniform_real_distribution<float> distGen(InternalConfig::distanceBound.lower, InternalConfig::distanceBound.upper);
std::uniform_int_distribution delayGen(InternalConfig::delayBound.lower, InternalConfig::delayBound.upper);

namespace RoBeats {
	ExternalConfig config;
	array<string, 4> keys { "", "", "", "" };
	array<int, 4> delays { 0, 0, 0, 0 };
	vector<Vector3>& lanePositions = InternalConfig::singleplayerLanePositions;

	bool GrabKeys() {
		bool found = false;

		for (const Instance& inst : workspace.GetDescendants()) {
			if (inst.GetName() == "ControlPopup") {
				int popupLane = GetNearestLane(BasePart(inst.GetAddress()).GetPosition());

				if (popupLane != -1) {
					keys[popupLane] = TextLabel(inst.FindFirstChildOfClass("SurfaceGui").FindFirstChildOfClass("Frame").FindFirstChild("Letter")).GetText();
					found = true;
				}
			}
		}

		return found;
	}

	void Reset() {
		for (int i = 0; i < 4; i++) {
			if (delays[i]) {
				delays[i] = 0;
				RbxInput::Release(keys[i]);
			}
		}
	}

	int GetNearestLane(Vector3 position) {
		float nearestDistance = InternalConfig::laneDistanceThreshold;
		int nearestLane = -1;

		for (int i = 0; i < 4; i++) {
			float distance = (lanePositions[i] - position).magnitude;

			if (distance < nearestDistance) {
				nearestLane = i;
				nearestDistance = distance;
			}
		}

		return nearestLane;
	}

	float RandomDistance() {
		return distGen(rng);
	}

	int RandomDelay() {
		return delayGen(rng);
	}
}