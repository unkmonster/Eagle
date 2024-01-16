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
		auto begin = m.as<uint8_t*>();
		auto end = begin + m.size();
		while (begin < end) {
			bool match{true};
			for (auto x : m_sign) {
				if (x && *x != *begin) {
					match = false;
					++begin;
					break;
				}
				++begin;
			}
			if (match) {
				SPDLOG_INFO("Found '{}' at {}+0x{:X}", m_name, m.name(), uintptr_t(begin - m_sign.size() - m.as<uint8_t*>()));
				return begin - m_sign.size();
			}	
		}
		throw std::runtime_error(fmt::format("Unable to find pattern '{}' in <{}>", m_name, m.name()));
	}
private:
	SIGNATURE m_sign;
	std::string m_name;
};