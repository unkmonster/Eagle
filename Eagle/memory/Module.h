#pragma once

#include <Windows.h>
#include <string>
#include <type_traits>

#include "utils/Utility.h"

class Pointer {
public:
	template <typename T>
	Pointer(T addr) : m_data(reinterpret_cast<uint8_t*>(addr)) {}

	explicit operator bool() {
		return m_data;
	}

	template <typename T>
	std::enable_if_t<std::is_pointer_v<T>, T> as() const {
		return reinterpret_cast<T>(m_data);
	}

	template <typename T>
	std::enable_if_t<std::is_same_v<T, uintptr_t>, T> as() const {
		return reinterpret_cast<T>(m_data);
	}

	// as T* and deref
	template <typename T>
	std::enable_if_t<std::is_lvalue_reference_v<T>, std::remove_reference_t<T>> as() const {
		using ty = std::remove_reference_t<T>;
		return *as<ty*>();
	}

	Pointer add(std::ptrdiff_t offset) const {
		return m_data + offset;
	}

	Pointer rip() const {
		return this->add(sizeof(int)).add(this->as<int&>());
	}
private:
	uint8_t* m_data;
};


class Module: public Pointer {
public:
	template <typename T>
	Module(const std::string& name, T addr):
		Pointer(addr), 
		m_name(name),
		m_size(GetModuleLength(addr))
	{}

	explicit Module(const std::string& name): 
		Module(name, GetModuleHandleA(name.c_str()))
	{}

	auto name() const { return m_name; }

	auto size() const { return m_size; }
private:
	std::string m_name;
	std::size_t m_size;
};