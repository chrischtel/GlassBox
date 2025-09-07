// GlassBox.cpp : Defines the entry point for the application.
//

#include "GlassBox.h"
#include <Windows.h>           // first
#include <processthreadsapi.h> // after
#include <vector>

int glassbox::Sandbox::GB_CreateProcess(const std::string& app,
    const std::vector<std::string>& args)
{
    STARTUPINFOA si{};
    PROCESS_INFORMATION pi{};
    si.cb = sizeof(si);

    // Build command line: "app arg1 arg2 ..."
    std::string cmdLine = app;
    for (const auto& arg : args) {
        cmdLine += " " + arg;
    }

    // Must be mutable
    std::vector<char> buffer(cmdLine.begin(), cmdLine.end());
    buffer.push_back('\0');

    BOOL status = CreateProcessA(
        NULL,           // NULL → exe name is taken from cmdLine
        buffer.data(),  // command line
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        NULL,
        &si,
        &pi
    );

    if (!status) {
        std::cerr << "CreateProcess failed (" << GetLastError() << ").\n";
        return -1;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exitCode = 0;
    GetExitCodeProcess(pi.hProcess, &exitCode);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return static_cast<int>(exitCode);
}
int glassbox::Sandbox::run(const std::string& path) {
	std::println("Running sandbox on path: {}", path);
    return GB_CreateProcess(path, {});
}

int glassbox::Sandbox::run(const std::string& path, const std::vector<std::string>& args) {
    return GB_CreateProcess(path, args);
}