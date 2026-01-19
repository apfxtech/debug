#pragma once

#define DEBUG_LEVEL_NONE    0   // отключено
#define DEBUG_LEVEL_ERROR   1   // ошибки
#define DEBUG_LEVEL_WARN    2   // + предупреждения
#define DEBUG_LEVEL_INFO    3   // + информация
#define DEBUG_LEVEL_DEBUG   4   // + отладка
#define DEBUG_LEVEL_TRACE   5   // + трассировка

#ifndef DEBUG_LEVEL
    #define DEBUG_LEVEL 0
#endif

#include <stdint.h>
#include <stddef.h>

#if DEBUG_LEVEL > 0
    #if defined(ARDUINO)
        #include <Arduino.h>
    #else
        #include <iostream>
        #include <iomanip>
        #include <sstream>
        #include <string>
        #include <chrono>
        #include <cstdio>
    #endif
#endif

#if DEBUG_LEVEL > 0
class DebugLogger {
private:
    static std::string getCurrentTime() {
        #if defined(ARDUINO)
            // Для Arduino используем millis()
            char buf[16];
            snprintf(buf, sizeof(buf), "%lu ms", (unsigned long)millis());
            return std::string(buf);
        #else
            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                now.time_since_epoch()) % 1000;
            std::stringstream ss;
            ss << std::put_time(std::localtime(&time_t), "%H:%M:%S");
            ss << "." << std::setfill('0') << std::setw(3) << ms.count();
            return ss.str();
        #endif
    }

    static void printLine(const char* level, const std::string& msg) {
        #if defined(ARDUINO)
            Serial.print("[");
            Serial.print(getCurrentTime().c_str());
            Serial.print("] ");
            Serial.print(level);
            Serial.print(" ");
            Serial.println(msg.c_str());
        #else
            std::cout << getCurrentTime() << " " << level << " " << msg << std::endl;
        #endif
    }

public:
    static void logError(const std::string& msg) { printLine("[ERROR]", msg); }
    static void logWarn(const std::string& msg)  { printLine("[WARN ]", msg); }
    static void logInfo(const std::string& msg)  { printLine("[INFO ]", msg); }
    static void logDebug(const std::string& msg) { printLine("[DEBUG]", msg); }
    static void logTrace(const std::string& msg) { printLine("[TRACE]", msg); }

    static void logHex(const std::string& prefix, const uint8_t* data, size_t len) {
        if (!data || len == 0) return;
        std::stringstream ss;
        ss << prefix << " [" << len << " bytes]: ";
        for (size_t i = 0; i < len; ++i) {
            ss << std::hex << std::setw(2) << std::setfill('0')
               << static_cast<int>(data[i]) << " ";
        }
        logDebug(ss.str());
    }

    template<typename... Args>
    static void logErrorF(const char* format, Args... args) {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), format, args...);
        logError(std::string(buffer));
    }

    template<typename... Args>
    static void logWarnF(const char* format, Args... args) {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), format, args...);
        logWarn(std::string(buffer));
    }

    template<typename... Args>
    static void logInfoF(const char* format, Args... args) {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), format, args...);
        logInfo(std::string(buffer));
    }

    template<typename... Args>
    static void logDebugF(const char* format, Args... args) {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), format, args...);
        logDebug(std::string(buffer));
    }

    template<typename... Args>
    static void logTraceF(const char* format, Args... args) {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), format, args...);
        logTrace(std::string(buffer));
    }
};
#endif // DEBUG_LEVEL > 0

// ---------------------- МАКРОСЫ ----------------------
#if DEBUG_LEVEL >= 1
    #define LOG_ERROR(msg) DebugLogger::logError(msg)
    #define LOG_ERROR_F(fmt, ...) DebugLogger::logErrorF(fmt, ##__VA_ARGS__)
#else
    #define LOG_ERROR(msg) ((void)0)
    #define LOG_ERROR_F(fmt, ...) ((void)0)
#endif

#if DEBUG_LEVEL >= 2
    #define LOG_WARN(msg) DebugLogger::logWarn(msg)
    #define LOG_WARN_F(fmt, ...) DebugLogger::logWarnF(fmt, ##__VA_ARGS__)
#else
    #define LOG_WARN(msg) ((void)0)
    #define LOG_WARN_F(fmt, ...) ((void)0)
#endif

#if DEBUG_LEVEL >= 3
    #define LOG_INFO(msg) DebugLogger::logInfo(msg)
    #define LOG_INFO_F(fmt, ...) DebugLogger::logInfoF(fmt, ##__VA_ARGS__)
#else
    #define LOG_INFO(msg) ((void)0)
    #define LOG_INFO_F(fmt, ...) ((void)0)
#endif

#if DEBUG_LEVEL >= 4
    #define LOG_DEBUG(msg) DebugLogger::logDebug(msg)
    #define LOG_DEBUG_F(fmt, ...) DebugLogger::logDebugF(fmt, ##__VA_ARGS__)
    #define LOG_HEX(prefix, data, len) DebugLogger::logHex(prefix, data, len)
    #define DEBUG_ONLY(code) do { code } while(0)
#else
    #define LOG_DEBUG(msg) ((void)0)
    #define LOG_DEBUG_F(fmt, ...) ((void)0)
    #define LOG_HEX(prefix, data, len) ((void)0)
    #define DEBUG_ONLY(code) ((void)0)
#endif

#if DEBUG_LEVEL >= 5
    #define LOG_TRACE(msg) DebugLogger::logTrace(msg)
    #define LOG_TRACE_F(fmt, ...) DebugLogger::logTraceF(fmt, ##__VA_ARGS__)
    #define TRACE_ONLY(code) do { code } while(0)
#else
    #define LOG_TRACE(msg) ((void)0)
    #define LOG_TRACE_F(fmt, ...) ((void)0)
    #define TRACE_ONLY(code) ((void)0)
#endif
