#pragma once

#include <filesystem>
#include <fstream>

#include "utils/Singleton.h"
#include "Logger.h"

class CFileManager {
	friend Singleton<CFileManager>;
	CFileManager(const std::filesystem::path& base):
		m_base(base),
		m_setting_bin(base / "setting.bin")
	{
		if (!std::filesystem::exists(base))
			std::filesystem::create_directory(base);
	}
public:
	std::filesystem::path m_base;
	std::filesystem::path m_setting_bin;
public:
	template <typename T>
	static bool load(const std::filesystem::path& path, T* buffer) {
		std::ifstream ifs(path, std::ios_base::in | std::ios_base::binary);
		if (!ifs.is_open()) {
			SPDLOG_WARN("Failed to open '{}'", path.string());
			return false;
		}
		ifs.read(reinterpret_cast<char*>(buffer), sizeof(T));
		return true;
	}

	template <typename T>
	static void dump(const std::filesystem::path& path, const T* data) {
		std::ofstream ofs(path, std::ios_base::out | std::ios_base::binary);
		if (!ofs.is_open()) {
			SPDLOG_WARN("Failed to open '{}'", path.string());
			return;
		}
		ofs.write(reinterpret_cast<const char*>(data), sizeof(T));
	}
};

inline CFileManager* gFileManager{};