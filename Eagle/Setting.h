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
		BoxType m_boxType = BOX_FULL;
		int m_effective{500};

		// text info
		Vec4 m_textColor{1, 1, 1, 1};	//
		bool m_showName{true};
		bool m_showHp{true};
		bool m_showDistance{true};

		// status line
		bool m_showStatusLine{false};
		PosType m_statusLinePos{POS_TOP};

		// health bar
		bool m_showHealthBar{true};
		PosType m_healthBarPos{POS_RIGHT};
	} m_esp;

	struct AimBot {
		bool m_enable;
		bool m_showCrossHair{true};
		Vec4 m_crossHairColor{1, 1, 1, 1};	//
		Vec4 m_crossHairColorAtEnemy{1, 0, 0, 1};
		Vec4 m_targetColor{1, 1, 1, 1};
		Vec4 m_targetColorOccluded{1, 1, 1, 1};
	} m_aimBot;
	float m_textSize = 16.f;	//
};