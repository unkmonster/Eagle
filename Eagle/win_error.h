#pragma once

#include "utility.h"
#include <stdexcept>

class WinError : public std::runtime_error {
public:
	WinError(uint32_t code) :
		std::runtime_error(nullptr),
		errcode(code),
		msg(GetLastErrorTextA(code)),
		formatted(fmt::format("({}) {}", errcode, msg.get())) {};

	virtual char const* what() const override {
		return formatted.c_str();
	}
private:
	uint32_t errcode;
	std::shared_ptr<char> msg;
	std::string formatted;
};