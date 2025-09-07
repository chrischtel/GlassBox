#include "GlassBox.h"
#include <Windows.h>
#include <vector>
#include <Psapi.h>

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

        DWORD exitCode = 0;

        if (waitResult == WAIT_TIMEOUT) {
            // Try to read exit code even though process is still running.
            if (!GetExitCodeProcess(hProcess.get(), &exitCode)) {
                exitCode = static_cast<DWORD>(-1); // mark unknown
            }

            collectStats(hProcess.get(), exitCode, /*timedOut=*/true);

            std::cerr << "Process timed out. Terminating...\n";
            TerminateJobObject(hJob.get(), 1);
            return 1;
        }

        if (!GetExitCodeProcess(hProcess.get(), &exitCode)) {
            std::cerr << "GetExitCodeProcess failed (" << GetLastError() << ").\n";
            return -1;
        }

        collectStats(hProcess.get(), exitCode, /*timedOut=*/false);
        return static_cast<int>(exitCode);
    }

    std::optional<ProcessStats> Sandbox::getLastStats() const
    {
        return stats_;
    }

    void Sandbox::collectStats(HANDLE hProcess, DWORD exitCode, bool timedOut)
    {
        stats_.exitCode = exitCode;
        stats_.timedOut = timedOut;

        PROCESS_MEMORY_COUNTERS pmc{};
        if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
            stats_.peakMemoryBytes = pmc.PeakWorkingSetSize;
        }

        FILETIME createTime, exitTime, kernelTime, userTime;
        if (GetProcessTimes(hProcess, &createTime, &exitTime, &kernelTime, &userTime)) {
            ULARGE_INTEGER kt{}, ut{};
            kt.LowPart = kernelTime.dwLowDateTime;
            kt.HighPart = kernelTime.dwHighDateTime;
            ut.LowPart = userTime.dwLowDateTime;
            ut.HighPart = userTime.dwHighDateTime;
            stats_.kernelTime100ns = kt.QuadPart;
            stats_.userTime100ns = ut.QuadPart;
        }
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
