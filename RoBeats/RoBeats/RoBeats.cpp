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
Bound<float> InternalConfig::distanceBound{ 0.3, 0.7 };
Bound<int> InternalConfig::delayBound{ 10, 13 };

vector<vector<Vector3>> InternalConfig::lanePositions {
	{
		Vector3 { -309.00, 387.70, -181.09 },
		Vector3 { -306.87, 387.70, -178.56 },
		Vector3 { -304.53, 387.70, -176.21 },
		Vector3 { -301.99, 387.70, -174.08 }
	},
	{
		Vector3 { -301.99, 387.70, -235.64 },
		Vector3 { -304.53, 387.70, -233.51 },
		Vector3 { -306.87, 387.70, -231.16 },
		Vector3 { -309.00, 387.70, -228.60 }
	},
	{
		Vector3 { -247.44, 387.70, -228.63 },
		Vector3 { -249.57, 387.70, -231.16 },
		Vector3 { -251.92, 387.70, -233.51 },
		Vector3 { -254.46, 387.70, -235.64 }
	},
	{
		Vector3 { -254.46, 387.70, -174.08 },
		Vector3 { -251.92, 387.70, -176.21 },
		Vector3 { -249.57, 387.70, -178.56 },
		Vector3 { -247.44, 387.70, -181.09 }
	}
};

std::random_device randDevice;
std::mt19937 rng(randDevice());
std::uniform_real_distribution<float> distGen(InternalConfig::distanceBound.lower, InternalConfig::distanceBound.upper);
std::uniform_int_distribution delayGen(InternalConfig::delayBound.lower, InternalConfig::delayBound.upper);

namespace RoBeats {
	ExternalConfig config;
	array<string, 4> keys { "", "", "", "" };
	array<int, 4> delays { 0, 0, 0, 0 };
	array<bool, 4> heldLanes { 0, 0, 0, 0 };
	int lanePositionsIndex = 0;

	bool GrabKeys() {
		bool found = false;

		for (const Instance& inst : workspace.GetDescendants()) {
			if (inst.GetName() == "ControlPopup") {
				Lane popupLane = GetNearestLane(BasePart(inst.GetAddress()).GetPosition());

				if (popupLane.Lane != -1) {
					keys[popupLane.Lane] = TextLabel(inst.FindFirstChildOfClass("SurfaceGui").FindFirstChildOfClass("Frame").FindFirstChild("Letter")).GetText();
					found = true;
				}
			}
		}

		return found;
	}

	void Reset() {
		for (int i = 0; i < 4; i++) {
			if (delays[i] || heldLanes[i]) {
				delays[i] = 0;
				heldLanes[i] = false;
				RbxInput::Release(keys[i]);
			}
		}
	}

	void UpdateLanePositions() {
		Vector3 camPos = workspace.CurrentCamera.GetPosition();
		float nearestDistance = InternalConfig::laneDistanceThreshold;
		int nearestGroup = -1;

		for (int i = 0; i < InternalConfig::lanePositions.size(); i++) {
			float distance = (InternalConfig::lanePositions[i][0] - camPos).magnitude;

			if (distance < nearestDistance) {
				nearestDistance = distance;
				nearestGroup = i;
			}
		}

		lanePositionsIndex = nearestGroup;
	}

	Lane GetNearestLane(Vector3 position) {
		float nearestDistance = InternalConfig::laneDistanceThreshold;
		int nearestLane = -1;

		for (int i = 0; i < 4; i++) {
			float distance = (InternalConfig::lanePositions[lanePositionsIndex][i] - position).magnitude;

			if (distance < nearestDistance) {
				nearestLane = i;
				nearestDistance = distance;
			}
		}

		return Lane{ nearestLane != -1 ? InternalConfig::lanePositions[lanePositionsIndex][nearestLane] : Vector3 { 0, 0, 0 }, nearestLane};
	}

	float RandomDistance() {
		return distGen(rng);
	}

	int RandomDelay() {
		return delayGen(rng);
	}
}