#pragma once

#include <Windows.h>
#include <string>
#include <vector>
#include <optional>
#include <sstream>
#include <iterator>
#include <algorithm>

#include <fmt/format.h>

#include "Module.h"

class Pattern {
	using SIGNATURE = std::vector<std::optional<uint8_t>>;
public:
	Pattern(const std::string& sig, const std::string& name = "") : m_name(name) {
		std::istringstream iss(sig);
		for (std::string bt; iss >> bt;/**/) {
			if (bt.front() != '?')
				m_sign.emplace_back(std::stoi(bt, nullptr, 16));
			else
				m_sign.emplace_back(std::nullopt);
		}
	}

	Pointer find(const Module& m) {
		static auto match = [](Pattern::SIGNATURE sign, BYTE* begin) -> bool {
			for (int i = 0; i < sign.size(); ++i) {
				if (sign[i] && sign[i].value() != begin[i])
					return false;
			}
			return true;
		};

		auto module_size = m.size();
		auto pattern_size = m_sign.size();
		for (std::size_t i = 0; i < module_size; ++i) {
			if (match(m_sign, m.as<BYTE*>() + i)) {
				SPDLOG_INFO("Found '{}' at {}+0x{:X}", m_name, m.name(), i);
				return m.as<BYTE*>() + i;
			}
		}
		SPDLOG_WARN("Unable to find pattern '{}' in <{}>", m_name, m.name());
		return nullptr;
		//throw std::runtime_error(fmt::format("Unable to find pattern '{}' in <{}>", m_name, m.name()));
	}
private:
	SIGNATURE m_sign;
	std::string m_name;
};