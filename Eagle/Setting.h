#pragma once

#include "math/Vector.h"
#include "Logger.h"

enum BoxType {
	BOX_FULL,
	BOX_CORNER,
	BOX_3D
};

enum PosType {
	POS_TOP,
	POS_BOTTOM,
	POS_LEFT,
	POS_RIGHT
};

struct Setting {
	struct EspPage {
		bool m_showBox{true};
		Vec4 m_boxColor{1, 1, 1, 1};
		Vec4 m_boxColorOccluded{1, 1, 1, 1};
		Vec4 m_vehicleColor{1, 1, 1, 1};
		BoxType m_boxType = BOX_FULL;
		int m_effective{3000};

		// text info
		bool m_showName{};
		bool m_showHp{};
		bool m_showDistance{};

		// status line
		bool m_showStatusLine{false};
		PosType m_statusLinePos{POS_TOP};

		// health bar
		bool m_showHealthBar{true};
		PosType m_healthBarPos{POS_LEFT};

		// CrossHair
		bool m_showCrossHair{true};
		Vec4 m_crossHairColor{1, 1, 1, 1};
		Vec4 m_crossHairColorAtEnemy{1, 0, 0, 1};

		// Skeleton
		bool m_showSkeleton{true};
	} m_esp;

	struct AimBot {
		bool m_enable{true};
		bool m_showArea{false};
		Vec2 m_areaSize{1920.f, 1080.f};
		Vec4 m_areaColor{1, 1, 1, 1};
		int m_maxDistance = 100;
		bool m_onlyVisible{true};
		int m_coldDown{500};
		int m_bone{};
	} m_aimBot;

	float m_textSize = 16.f;
	bool m_showObserversList{true};
	float m_menuFontSize = 16.f;
	int m_theme{};
};