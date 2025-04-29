//
// Created by ianpo on 17/06/2023.
//

#pragma once

#ifndef SPDLOG_ACTIVE_LEVEL
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#endif

#include <spdlog/spdlog.h>
#include "LoopBackBuffer.hpp"

namespace Imagine::Core {

    class Log {
    public:
        enum Level {
            Debug,
            Trace,
            Info,
            Warning,
            Error,
            Critical,
        };

        struct Message {
            std::string message;
            Level level;
        };

        inline static constexpr uint64_t MessageBufferCount = 100;
        using MessageBuffer = LoopBackBuffer<Message, MessageBufferCount>;
    public:
        static void Init();
        static void Shutdown();
        inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
        inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
    public:
        inline static MessageBuffer::const_iterator begin() {return s_LastLogs.cbegin();}
        inline static MessageBuffer::const_iterator end() {return s_LastLogs.cend();}
        inline static uint64_t buffer_log_count() {return s_LastLogs.size();}
    private:
        static void OnSpdlogCallback(const spdlog::details::log_msg& msg);
    private:
        static std::shared_ptr<spdlog::logger> s_CoreLogger;
        static std::shared_ptr<spdlog::logger> s_ClientLogger;
        inline static MessageBuffer s_LastLogs{};
    };
}
