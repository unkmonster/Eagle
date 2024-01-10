#pragma once

#include <Windows.h>
#include <string>
#include <vector>
#include <optional>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <type_traits>

#include <fmt/format.h>

#include "Module.h"

class Pointer {
public:
	template <typename T>
	Pointer(T addr) : m_data(reinterpret_cast<uint8_t*>(addr)) {}

	template <typename T>
	std::enable_if_t<std::is_pointer_v<T>, T> as() {
		return reinterpret_cast<T>(m_data);
	}

	template <typename T>
	std::enable_if_t<std::is_same_v<T, uintptr_t>, T> as() {
		return reinterpret_cast<T>(m_data);
	}

	// as T* and deref
	template <typename T>
	std::enable_if_t<std::is_lvalue_reference_v<T>, std::remove_reference_t<T>> as() {
		using ty = std::remove_reference_t<T>;
		return *as<ty*>();
	}

	Pointer add(std::ptrdiff_t offset) {
		return m_data + offset;
	}

	Pointer rip() {
		return this->add(sizeof(int)).add(this->as<int&>());
	}
private:
	uint8_t* m_data;
};


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
			if (match(m_sign, m.get<BYTE*>() + i)) {
				fmt::output_debug("Found '{}' at {}+0x{:X}", m_name, m.name(), i);
				return m.get<BYTE*>() + i;
			}
				
		}
		throw std::runtime_error(fmt::format("Unable to find pattern '{}' in <{}>", m_name, m.name()));
	}
private:
	SIGNATURE m_sign;
	std::string m_name;
};