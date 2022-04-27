#pragma once
#include "Instance.h"
#include "Vector3.h"
#include <map>
#include <string>
#include <array>
using std::array, std::string;

template<typename T>
struct Bound {
	T lower, upper;
};

struct Lane {
	Vector3 Position;
	int Lane;
};

typedef struct ExternalConfig {
	bool ManualKeys = false;
	const char* Keys[4] = { "", "", "", "" };
} ExternalConfig;

namespace RoBeats {
	struct InternalConfig {
		constexpr static int noteY = 3879, sliderY = 3878, laneDistanceThreshold = 25, gameStartCameraY = 4017, sliderDebounce = 9.5;
		static Bound<float> distanceBound;
		static Bound<int> delayBound;
		static vector<vector<Vector3>> lanePositions;
	};

	extern vector<Vector3>& currentLanePositions;
	extern int lanePositionsIndex;
	extern ExternalConfig config;
	extern array<string, 4> keys;
	extern array<int, 4> delays;
	extern array<bool, 4> heldLanes;
	extern Instance game;
	extern Workspace workspace;
	extern GuiService guiService;

	Lane GetNearestLane(Vector3 position);
	bool GrabKeys();
	void Reset();
	void UpdateLanePositions();
	float RandomDistance();
	int RandomDelay();

	inline bool IsNote(const Adornment& adorn) {
		return floor(adorn.Position.y * 10) == InternalConfig::noteY && adorn.Transparency == 0;
	};

	inline bool IsSlider(const Adornment& adorn) {
		return floor(adorn.Position.y * 10) == InternalConfig::sliderY && adorn.Height > 0.2 && adorn.Transparency != 1;
	}

	inline bool CanHit(const Adornment& adorn, Lane lane) {
		bool isSlider = IsSlider(adorn);

		if (lane.Lane == -1 || delays[lane.Lane] > 0 || (!isSlider && !IsNote(adorn)))
			return false;

		float distance = isSlider ? (adorn.Position - adorn.LookVector * (adorn.Height / 2)).x - lane.Position.x : adorn.Position.x - lane.Position.x;

		if (lanePositionsIndex > 1)
			distance = abs(distance);

		return (!isSlider || !heldLanes[lane.Lane]) && distance <= RandomDistance();
	}

	inline bool CanRelease(const Adornment& adorn, Lane lane) {
		float distance = (adorn.Position + adorn.LookVector * (adorn.Height / 2)).x - lane.Position.x;

		if (lanePositionsIndex > 1)
			distance = abs(distance);

		return lane.Lane != -1 && heldLanes[lane.Lane] && IsSlider(adorn) && distance <= RandomDistance();
	}
};