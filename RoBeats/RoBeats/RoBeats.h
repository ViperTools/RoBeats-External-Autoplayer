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

typedef struct ExternalConfig {
	bool ManualKeys = false;
	const char* Keys[4] = { "", "", "", "" };
} ExternalConfig;

namespace RoBeats {
	struct InternalConfig {
		constexpr static int noteY = 3879, sliderY = 3878, laneCameraDistanceThreshold = 110, laneDistanceThreshold = 25;
		static Bound<float> distanceBound;
		static Bound<int> delayBound;
		static vector<Vector3> singleplayerLanePositions;
		static vector<Vector3> multiplayerLanePositions;
	};

	extern ExternalConfig config;
	extern vector<Vector3>& lanePositions;
	extern array<string, 4> keys;
	extern array<int, 4> delays;
	extern Instance game;
	extern Workspace workspace;
	extern GuiService guiService;

	int GetNearestLane(Vector3 position);
	bool GrabKeys();
	void Reset();
	float RandomDistance();
	int RandomDelay();

	inline bool IsNoteOrSliderStart(const Adornment& adorn) {
		return (!adorn.IsCylinder || floor(adorn.Position.y * 10) == InternalConfig::noteY) && adorn.Transparency == 0;
	};

	inline bool IsSlider(const Adornment& adorn) {
		return adorn.IsCylinder && floor(adorn.Position.y * 10) == InternalConfig::sliderY && adorn.Height > 0.2 && adorn.Transparency != 1;
	}

	inline bool CanHit(const Adornment& adorn, int lane) {
		return lane > -1 && !delays[lane] && IsNoteOrSliderStart(adorn) && adorn.Position.x - lanePositions[lane].x <= RandomDistance();
	}

	inline bool CanRelease(const Adornment& adorn, int lane) {
		return lane > -1 && IsSlider(adorn) && adorn.Position.x + adorn.Height / 3 - lanePositions[lane].x <= RandomDistance();
	}
};