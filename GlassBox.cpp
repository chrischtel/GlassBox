#include "GlassBox.h"
#include <Windows.h>
#include <vector>

namespace glassbox {

    int Sandbox::GB_CreateProcess(const std::string& app,
        const std::vector<std::string>& args,
        DWORD timeout_ms)
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

        // Create process
        BOOL status = CreateProcessA(
            NULL,            // use first token from cmdLine
            buffer.data(),   // mutable command line
            NULL,
            NULL,
            FALSE,
            CREATE_SUSPENDED, // start suspended until job assigned
            NULL,
            NULL,
            &si,
            &pi
        );

        if (!status) {
            std::cerr << "CreateProcess failed (" << GetLastError() << ").\n";
            return -1;
        }

        UniqueHandle hProcess(pi.hProcess);
        UniqueHandle hThread(pi.hThread);

        // Create job object
        UniqueHandle hJob(CreateJobObjectA(NULL, NULL));
        if (!hJob) {
            std::cerr << "CreateJobObject failed (" << GetLastError() << ").\n";
            return -1;
        }

        JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli{};
        jeli.BasicLimitInformation.LimitFlags = 0;

        // Apply memory limit if set
        if (config_.memoryLimitMB > 0) {
            jeli.BasicLimitInformation.LimitFlags |= JOB_OBJECT_LIMIT_PROCESS_MEMORY;
            jeli.ProcessMemoryLimit = config_.memoryLimitMB * 1024 * 1024;
        }

        // Apply CPU percent if set
        if (config_.cpuPercent > 0) {
            JOBOBJECT_CPU_RATE_CONTROL_INFORMATION cpuInfo{};
            cpuInfo.ControlFlags = JOB_OBJECT_CPU_RATE_CONTROL_ENABLE |
                JOB_OBJECT_CPU_RATE_CONTROL_HARD_CAP;
            cpuInfo.CpuRate = config_.cpuPercent * 100; // percent * 100
            if (!SetInformationJobObject(hJob.get(), JobObjectCpuRateControlInformation,
                &cpuInfo, sizeof(cpuInfo))) {
                std::cerr << "SetInformationJobObject (CPU) failed: " << GetLastError() << "\n";
            }
        }

        // Finally set extended limits
        if (!SetInformationJobObject(hJob.get(), JobObjectExtendedLimitInformation,
            &jeli, sizeof(jeli))) {
            std::cerr << "SetInformationJobObject (Memory) failed: " << GetLastError() << "\n";
        }

        if (!AssignProcessToJobObject(hJob.get(), hProcess.get())) {
            std::cerr << "AssignProcessToJobObject failed (" << GetLastError() << ").\n";
            return -1;
        }

        // Resume process now that it's in the job
        ResumeThread(hThread.get());

        // Wait
        DWORD waitResult = WaitForSingleObject(hProcess.get(), timeout_ms);
        if (waitResult == WAIT_TIMEOUT) {
            std::cerr << "Process timed out. Terminating...\n";
            TerminateJobObject(hJob.get(), 1); // kills all processes in job
            return 1;
        }

        // Collect exit code
        DWORD exitCode = 0;
        if (!GetExitCodeProcess(hProcess.get(), &exitCode)) {
            std::cerr << "GetExitCodeProcess failed (" << GetLastError() << ").\n";
            return -1;
        }

        return static_cast<int>(exitCode);
    }

    int Sandbox::run(const std::string& path) {
        std::println("Running sandbox on path: {}", path);
        return GB_CreateProcess(path, {}, config_.timeoutMs);
    }

    int Sandbox::run(const std::string& path, const std::vector<std::string>& args) {
        return GB_CreateProcess(path, args, config_.timeoutMs);
    }

    int Sandbox::run_with_timeout(const std::string& path, DWORD timeout_ms) {
        return GB_CreateProcess(path, {}, timeout_ms);
    }

    int Sandbox::run_with_timeout(const std::string& path,
        const std::vector<std::string>& args,
        DWORD timeout_ms) {
        return GB_CreateProcess(path, args, timeout_ms);
    }

} // namespace glassbox
