#pragma once

#include <Windows.h>
#include <string>

#include "Utility.h"

class Module {
public:
	template <typename T>
	Module(const std::string& name, T addr):
		m_base(reinterpret_cast<uintptr_t>(addr)), 
		m_name(name),
		m_size(GetModuleLength(addr))
	{}

	explicit Module(const std::string& name): Module(name, GetModuleHandleA(name.c_str())) {}

	template <typename T>
	T get() { return (T)(m_base); }

	auto name() const { return m_name; }

	auto size() const { return m_size; }
private:
	uintptr_t m_base;
	std::string m_name;
	std::size_t m_size;
};