#include "utility.h"

std::shared_ptr<wchar_t> GetLastErrorTextW(DWORD err_code = GetLastError()) {
    wchar_t* buffer;
    FormatMessageW(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        err_code,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
        reinterpret_cast<wchar_t*>(&buffer),
        0,
        nullptr
    );
    return std::shared_ptr<wchar_t>(buffer, LocalFree);
}

std::shared_ptr<char> GetLastErrorTextA(DWORD err_code = GetLastError()) {
    char* buffer;
    FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        err_code,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
        reinterpret_cast<char*>(&buffer),
        0,
        nullptr
    );
    return std::shared_ptr<char>(buffer, LocalFree);
}