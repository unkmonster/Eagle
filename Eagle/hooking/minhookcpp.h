#pragma once

#include <stdexcept>

#include <MinHook.h>
#include <fmt/format.h>

#include "utils/Utility.h"
#include "Logger.h"

template <typename T>
class MinHookCpp {
public:
	MinHookCpp() = default;

	MinHookCpp(LPVOID target, LPVOID detour, std::string name = ""): m_name(name), m_target(target) {
		auto result = MH_CreateHook(target, detour, reinterpret_cast<LPVOID*>(&m_origin));
		if (result != MH_OK)
			throw std::runtime_error(fmt::format("'{}' {}", name, MH_StatusToString(result)).c_str());
		m_disabled = false;
		SPDLOG_INFO("Created Hook '{}'", m_name);
	}

	// Api Hook
	MinHookCpp(const std::wstring& module, const std::string& func, LPVOID detour): m_name(func) {
		auto result = MH_CreateHookApiEx(module.c_str(), func.c_str(),
			detour, reinterpret_cast<LPVOID*>(&m_origin), &m_target);
		if (result != MH_OK) 
			throw std::runtime_error(fmt::format("'{}' {}", func, MH_StatusToString(result)));
		m_disabled = false;
		SPDLOG_INFO("Created Hook '{}'", m_name);
	}

	~MinHookCpp() {
		try {
			disable();
		} catch (const std::runtime_error& err) {
			SPDLOG_WARN(err.what());
		}
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

	MinHookCpp& operator=(MinHookCpp&& rhs) {
		if (&rhs != this) {
			m_target = rhs.m_target;
			m_origin = rhs.m_origin;
			m_disabled = rhs.m_disabled;
			m_name = std::move(rhs.m_name);

			rhs.m_disabled = true;
		}
		return *this;
	}

	auto enable() {
		if (!m_target)
			return MH_ERROR_NOT_CREATED;
		return MH_EnableHook(m_target);
	}

	void disable() {
		if (m_disabled) return;
		if (auto result = MH_DisableHook(m_target); result != MH_OK)
			throw std::runtime_error(fmt::format("({}) Failed to MH_DisableHook '{}'", static_cast<int>(result), m_name).c_str());
		m_disabled = true;
		SPDLOG_INFO("Disabled Hook '{}'", m_name);
	}

	auto get_origin() {
		return m_origin;
	}
	
	template <typename... Args>
	auto call_origin(Args&&... args) {
		return m_origin(std::forward<Args>(args)...);
	}
private:
	void* m_target{};
	T* m_origin{};
	bool m_disabled = true;
	std::string m_name;
};