#pragma once

#ifdef _DEBUG
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#else
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO
#endif // _DEBUG


#include <Windows.h>

#include <optional>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/msvc_sink.h>

#include "Utils/Singleton.h"
#include "Utils/Utility.h"


class Logger {
	friend Singleton<Logger>;

	Logger() {
		if (!AttachConsole(GetCurrentProcessId())) {
			auto err = GetLastError();
			if (err == ERROR_ACCESS_DENIED) {
				// Console has exist
				m_oConMode = 0;
				auto hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
				GetConsoleMode(hConsole, &m_oConMode.value());

				DWORD mode = m_oConMode.value() % ~ENABLE_QUICK_EDIT_MODE;
				SetConsoleMode(hConsole, mode);

			} else if (err == ERROR_INVALID_HANDLE) 
				AllocConsole();
		}

		// Setting Sink
		spdlog::sinks_init_list sinks{
			std::make_shared<spdlog::sinks::stdout_color_sink_mt>(),
#ifdef _DEBUG
			std::make_shared<spdlog::sinks::msvc_sink_mt>(false)
#endif // _DEBUG
		};

		// Setting Logger 
		auto logger = std::make_shared<spdlog::logger>("deflogger", sinks.begin(), sinks.end());
#ifdef _DEBUG
		logger->set_level(spdlog::level::debug);
		logger->set_pattern("[%Y-%m-%d %T.%e] [%s:%#] [%^%l%$] %v");
#else
		logger->set_pattern("[%Y-%m-%d %T.%e] [%^%l%$] %v");
		logger->set_level(spdlog::level::info);
#endif // _DEBUG
		spdlog::set_default_logger(logger);
	}
public:
	~Logger() {
		if (m_oConMode.has_value()) 
			SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), m_oConMode.value());
		else 
			FreeConsole();
		SPDLOG_DEBUG(__func__);
	}
private:
	std::optional<DWORD> m_oConMode;
};

inline Logger* gLogger;