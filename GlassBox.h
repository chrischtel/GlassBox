#pragma once
#include <Windows.h>

#include <iostream>
#include <print>
#include <string>
#include <vector>
#include <optional>

namespace glassbox {

    // RAII wrapper for Windows HANDLE
    class UniqueHandle {
        HANDLE h = nullptr;
    public:
        explicit UniqueHandle(HANDLE handle = nullptr) : h(handle) {}
        ~UniqueHandle() { if (h) CloseHandle(h); }
        UniqueHandle(const UniqueHandle&) = delete;
        UniqueHandle& operator=(const UniqueHandle&) = delete;
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

    class Sandbox {
    private:
        int GB_CreateProcess(const std::string& app,
            const std::vector<std::string>& args,
            DWORD timeout_ms);

    public:
        int run(const std::string& path);
        int run(const std::string& path, const std::vector<std::string>& args);
        int run_with_timeout(const std::string& path, DWORD timeout_ms);
        int run_with_timeout(const std::string& path,
            const std::vector<std::string>& args,
            DWORD timeout_ms);
    };

} // namespace glassbox
