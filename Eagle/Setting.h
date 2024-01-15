#pragma once

#include "math/Vector.h"
#include "Logger.h"

enum BoxType {
	BOX_FULL,
	BOX_CORNER
};

struct Setting {
	struct Esp {
		bool m_showBox{true};
		Vec4 m_boxColor{1.f, 1, 1, 1};
		BoxType m_boxType = BOX_FULL;

		bool m_showName{true};
	} m_esp;
};