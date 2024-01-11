#pragma once

#include <vector>
#include <functional>
#include <optional>
#include <shared_mutex>

#include <imgui.h>

#include "utils/Singleton.h"

class Gui {
	friend Singleton<Gui>;
	Gui() {
		m_tasks.emplace_back([]() {
			static bool show{true};
			ImGui::ShowDemoWindow(&show);
		});
	}
public:
	~Gui() {
		toggle(false);
	}

	void display() {
		if (!m_is_open) return;

		for (auto x : m_tasks)
			x();
	}

	void toggle(std::optional<bool> val = std::nullopt) {
		if (val)
			m_is_open = *val;
		else
			m_is_open ^= 1;
	}
private:
	bool m_is_open{true};
	std::vector<void(*)()> m_tasks;
	//std::shared_mutex m_mutex;
};