#pragma once

#include <Windows.h>

#include <memory>

#include <fmt/format.h>

#include "common.h"

namespace fmt {
template <typename... Args>
inline void output_debug(const std::string& msg, Args&&... args) {
#ifdef _DEBUG
	OutputDebugStringA(format("[{}] {}\n", PROJECTNAME, format(msg, std::forward<Args>(args)...)).c_str());
#endif // _DEBUG
}
} // fmt


std::shared_ptr<wchar_t> GetLastErrorTextW(DWORD err_code = GetLastError());
std::shared_ptr<char> GetLastErrorTextA(DWORD err_code = GetLastError());

#ifdef UNICODE
#define GetLastErrorText  GetLastErrorTextW
#else
#define GetLastErrorText GetLastErrorTextA
#endif // !UNICODE


template <typename T>
inline size_t GetModuleLength(T m) {
	if (!m) return 0;
	auto addr = (uintptr_t)(m);

	auto dosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(addr);
	auto ntHeader = reinterpret_cast<IMAGE_NT_HEADERS*>(addr + dosHeader->e_lfanew);
	return ntHeader->OptionalHeader.SizeOfImage;
}

std::string LoadStr(HMODULE module, uint32_t id);
std::wstring LoadStrW(HMODULE module, uint32_t id);
std::string LoadStrU8(HMODULE module, uint32_t id);

std::string WstrToUtf8Str(const std::wstring& wstr);