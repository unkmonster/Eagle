#pragma once

#include <MinHook.h>
#include <stdexcept>
#include <fmt/format.h>
#include "Utility.h"

template <typename T>
class MinHookCpp {
public:
	MinHookCpp() = default;

	MinHookCpp(LPVOID target, LPVOID detour, std::string name = ""): m_name(name), m_target(target) {
		try {
			auto result = MH_CreateHook(target, detour, reinterpret_cast<LPVOID*>(&m_origin));
			if (result != MH_OK)
				throw std::runtime_error(fmt::format("({}) Failed to MH_CreateHook '{}'", static_cast<int>(result), m_name).c_str());
			m_disabled = false;
			fmt::output_debug("Created Hook '{}'", m_name);
		} catch (const std::runtime_error& err) {
			fmt::output_debug(err.what());
		}
		
	}

	~MinHookCpp() {
		disable();
	}

	MinHookCpp(const MinHookCpp&) = delete;
	MinHookCpp& operator=(const MinHookCpp&) = delete;

	MinHookCpp(MinHookCpp&& rhs): 
		m_target(rhs.m_target), 
		m_origin(rhs.m_origin),
		m_disabled(rhs.m_disabled),
		m_name(std::move(rhs.m_name))
	{
		rhs.m_disabled = true;
	}



	auto enable() {
		return MH_EnableHook(m_target);
	}

	void disable() {
		if (m_disabled) return;
		if (auto result = MH_DisableHook(m_target); result != MH_OK)
			throw std::runtime_error(fmt::format("Failed to MH_DisableHook ({})", static_cast<int>(result)).c_str());
		m_disabled = true;
		fmt::output_debug("Disabled Hook '{}'", m_name);
	}

	auto get_origin() {
		return m_origin;
	}
private:
	LPVOID m_target;
	T* m_origin;
	bool m_disabled;
	std::string m_name;
};