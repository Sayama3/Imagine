//
// Created by ianpo on 17/06/2023.
//

#include "Imagine/Core/Logger.hpp"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/callback_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>


namespace Imagine {

	inline static std::string s_LogPattern = "%^[%T] [%l] %n (%s:%#->%!): %v%$";
	// static constexpr const char* const c_LogPattern = "%^[%T] [%l] %n (%s:%#->%!): %v%$";
	// static inline const char* const c_LogFileName = "Imagine.log";
	// static inline const char* const c_OldLogFileName = "Imagine.old.log";

	std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
	std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

	void Log::OnSpdlogCallback(const spdlog::details::log_msg &msg) {
		static spdlog::pattern_formatter formatter{s_LogPattern};

		spdlog::memory_buf_t buffer;
		formatter.format(msg, buffer);
		Level level;
		switch (msg.level) {
			case spdlog::level::trace:
				level = Level::Trace;
				break;
			case spdlog::level::debug:
				level = Level::Debug;
				break;
			case spdlog::level::info:
				level = Level::Info;
				break;
			case spdlog::level::warn:
				level = Level::Warning;
				break;
			case spdlog::level::err:
				level = Level::Error;
				break;
			case spdlog::level::critical:
				level = Level::Critical;
				break;
			default:
				level = Level::Trace;
		}
		s_LastLogs.push_front(Message{std::string{buffer.data(), buffer.size()}, level});
	}

	void Log::Init(const LogParameters &parameters) {

		if (parameters.LogFileParameters) {
			if (std::filesystem::exists(parameters.LogFileParameters->LogFilePath)) {
				if (parameters.LogFileParameters->OldLogFilePath) std::filesystem::copy_file(parameters.LogFileParameters->LogFilePath, parameters.LogFileParameters->OldLogFilePath.value(), std::filesystem::copy_options::overwrite_existing);
				std::filesystem::remove(parameters.LogFileParameters->LogFilePath);
			}
		}

		// Setting Pattern
		spdlog::set_pattern(parameters.LogPattern);
		s_LogPattern = parameters.LogPattern;

		// Creating core logger
		std::vector<spdlog::sink_ptr> core_sinks;
		core_sinks.reserve(3);
		if (parameters.LogToConsole) core_sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		if (parameters.LogFileParameters) core_sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(parameters.LogFileParameters->LogFilePath.string().c_str()));
		core_sinks.push_back(std::make_shared<spdlog::sinks::callback_sink_mt>(Log::OnSpdlogCallback));

		s_CoreLogger = std::make_shared<spdlog::logger>("CORE", core_sinks.begin(), core_sinks.end());
		s_CoreLogger->set_level(spdlog::level::trace);

		// Creating Client logger
		std::vector<spdlog::sink_ptr> client_sinks{core_sinks.begin(), core_sinks.end()};

		s_ClientLogger = std::make_shared<spdlog::logger>("IMAGINE", client_sinks.begin(), client_sinks.end());
		s_ClientLogger->set_level(spdlog::level::trace);

		// TODO: Add imgui sink logger.
	}

	void Log::Shutdown() {
		// TODO: Do something later on.
	}
} // namespace Imagine
