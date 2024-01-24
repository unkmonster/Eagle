#include "utility.h"

#include <vector>
#include <sstream>

#include "win_error.h"

std::shared_ptr<wchar_t> GetLastErrorTextW(DWORD err_code) {
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

std::shared_ptr<char> GetLastErrorTextA(DWORD err_code) {
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

std::string LoadStr(HMODULE module, uint32_t id) {
    char buffer[1024]{};
    if (!LoadStringA(module, id, buffer, _countof(buffer)))
        throw WinError(GetLastError());
    return buffer;
}

std::wstring LoadStrW(HMODULE module, uint32_t id) {
    wchar_t buffer[1024]{};
    if (!LoadStringW(module, id, buffer, 1024))
        throw WinError(GetLastError());
    return buffer;
}

std::string LoadStrU8(HMODULE module, uint32_t id) {
    return WstrToUtf8Str(LoadStrW(module, id));
}

std::string WstrToUtf8Str(const std::wstring& wstr) {
    std::string retStr;
    if (!wstr.empty())
    {
        int sizeRequired = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);

        if (sizeRequired > 0)
        {
            std::vector<char> utf8String(sizeRequired);
            int bytesConverted = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(),
                -1, &utf8String[0], utf8String.size(), NULL,
                NULL);
            if (bytesConverted != 0)
            {
                retStr = &utf8String[0];
            } else
            {
                std::stringstream err;
                err << __FUNCTION__
                    << " std::string WstrToUtf8Str failed to convert wstring '"
                    << wstr.c_str() << L"'";
                throw std::runtime_error(err.str());
            }
        }
    }
    return retStr;
}