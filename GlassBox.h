#pragma once
#include <Windows.h>

#include <iostream>
#include <print>
#include <string>
#include <vector>
#include <optional>

namespace glassbox {

    /// @brief Sandbox configuration (limits & policies).
    struct JobObjectConfig {
        size_t memoryLimitMB = 0;     ///< Per-process memory cap (0 = no limit).
        int cpuPercent = 0;           ///< CPU cap as percentage (0 = no limit).
        DWORD timeoutMs = INFINITE;   ///< Timeout in milliseconds (default = no timeout).
    };

    /// @brief RAII wrapper for Windows HANDLE to prevent leaks.
    class UniqueHandle {
        HANDLE h = nullptr;
    public:
        explicit UniqueHandle(HANDLE handle = nullptr) : h(handle) {}
        ~UniqueHandle() { if (h) CloseHandle(h); }

        // non-copyable
        UniqueHandle(const UniqueHandle&) = delete;
        UniqueHandle& operator=(const UniqueHandle&) = delete;

        // movable
        UniqueHandle(UniqueHandle&& other) noexcept : h(other.h) { other.h = nullptr; }
        UniqueHandle& operator=(UniqueHandle&& other) noexcept {
            if (this != &other) {
                if (h) CloseHandle(h);
                h = other.h;
                other.h = nullptr;
            }
            return *this;
        }

        HANDLE get() const { return h; }
        explicit operator bool() const { return h != nullptr; }
    };

    /// @brief Main class for launching and controlling sandboxed processes.
    class Sandbox {
    private:
        JobObjectConfig config_{};
        int GB_CreateProcess(const std::string& app,
            const std::vector<std::string>& args,
            DWORD timeout_ms);

    public:
        Sandbox() = default;
        explicit Sandbox(const JobObjectConfig& config) : config_(config) {}

        // --- configuration setters ---
        void setMemoryLimitMB(size_t mb) { config_.memoryLimitMB = mb; }
        void setCpuPercent(int percent) { config_.cpuPercent = percent; }
        void setTimeoutMs(DWORD ms) { config_.timeoutMs = ms; }

        // --- process execution ---
        int run(const std::string& path);
        int run(const std::string& path, const std::vector<std::string>& args);
        int run_with_timeout(const std::string& path, DWORD timeout_ms);
        int run_with_timeout(const std::string& path,
            const std::vector<std::string>& args,
            DWORD timeout_ms);
    };

} // namespace glassbox
