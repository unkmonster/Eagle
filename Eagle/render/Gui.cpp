#include "Gui.h"

#include <imgui.h>

void Gui::DrawMenu() {
	static bool show = true;
	ImGui::ShowDemoWindow(&show);
}
