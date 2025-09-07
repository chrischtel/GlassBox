# GlassBox

GlassBox is an experimental sandboxing and process isolation library written in modern C++23.  
It provides a clean abstraction for creating and monitoring external processes under controlled conditions, with support for resource governance and lifecycle management.

---

## Current Capabilities

The Windows proof-of-concept implements the following:

- ✅ Process creation with argument passing
- ✅ Execution timeouts with termination
- ✅ Job Object integration for automatic cleanup
- ✅ Memory limits (per-process, in MB)
- ✅ CPU usage limits (percentage-based throttling)
- ✅ RAII-based handle management in C++

Not yet implemented:

- ⬜ Reduced-privilege execution (restricted tokens)
- ⬜ Filesystem and registry isolation (AppContainer integration)
- ⬜ Network restrictions
- ⬜ Linux/macOS backend (`fork`/`exec`, `setrlimit`, namespaces)
- ⬜ Process statistics API (memory usage, CPU time)

---

## Example Usage

```cpp
#include "GlassBox.h"

int main() {
    glassbox::Sandbox sb;
    sb.setMemoryLimitMB(256);   // 256 MB per process
    sb.setCpuPercent(25);       // 25% CPU cap
    sb.setTimeoutMs(5000);      // 5 second timeout

    int exitCode = sb.run("notepad.exe");
    std::println("Process exited with code {}", exitCode);
}
````

Example programs can be found in the `examples/` directory.

---

## Security Considerations

GlassBox should currently be considered a **resource governance and experimentation tool**, not a hardened sandbox:

* Processes inherit the same privileges as the parent process.
* Filesystem, registry, and network isolation are not enforced yet.
* Only per-process resource limits are applied on Windows.
* Linux and macOS backends are not implemented at this stage.

It should **not** be used to execute untrusted code in production environments.
Its purpose is educational, experimental, and to provide a foundation for further development.

---

## Build

GlassBox uses CMake as its build system.

```bash
git clone https://github.com/chrischtel/GlassBox.git
cd GlassBox
cmake -B build
cmake --build build
```

---

## Contributing

Contributions are welcome in the following areas:

* Linux/macOS backend implementation
* Security enhancements on Windows
* API design reviews
* Documentation and examples
* Unit testing and CI setup

---

## License

GlassBox is released under MPL-2.0 License.

---

## Project Goals

GlassBox aims to become a **portable, modern C++ sandboxing framework**.
By abstracting low-level OS primitives into a consistent, RAII-safe API, the library seeks to provide:

* A reliable foundation for process control and resource governance.
* Extensible configuration for varying use cases (scripts, CI workers, experiments).
* Cross-platform support with feature parity between Windows, Linux, and macOS.
* A clean C++23 interface that hides platform-specific complexity.