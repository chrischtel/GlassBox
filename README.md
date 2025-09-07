# GlassBox

**GlassBox** is an experimental cross-platform sandboxing library written in modern C++23.  
It provides a clean, developer-friendly API for launching processes with **resource limits, timeouts, and isolation policies**, without exposing raw Windows or POSIX system calls.

---

## ✨ Features (current proof-of-concept)

- Run external processes with arguments
- Per-process **memory limit** (via Windows Job Objects)
- Per-process **CPU cap** (percent throttling)
- **Timeouts** with forced termination
- Automatic cleanup of child processes (via `KILL_ON_JOB_CLOSE`)
- Modern C++ API with RAII safety

---

## 🚀 Example

```cpp
#include "GlassBox.h"

int main() {
    glassbox::Sandbox sb;
    sb.setMemoryLimitMB(256);
    sb.setCpuPercent(25);
    sb.setTimeoutMs(5000); // 5s timeout

    int code = sb.run("notepad.exe");
    std::println("Process exited with code {}", code);
}
````

---

## ⚠️ Security Notes

GlassBox is **not a security product yet**.
At this stage, it is a **process supervisor** with resource caps, not a full sandbox.

* Processes still run with the **same privileges** as the parent.
* **Filesystem and registry isolation** are not enforced yet.
* **Network access** is not blocked.
* On Windows, only Job Object resource limits are applied.
* On Linux/macOS, sandboxing is not yet implemented.

Do not rely on GlassBox to safely execute untrusted code at this stage.
Its purpose right now is educational, experimental, and developer-oriented.

---

## 📍 Roadmap

**Phase 1 — Windows POC (current)**

* ✅ Process spawn with args
* ✅ Timeout & forced termination
* ✅ Job Object with memory + CPU limits
* ⬜ Improved error handling (`std::expected` / enums)
* ⬜ Basic process statistics API (memory, CPU usage)

**Phase 2 — Windows Security Enhancements**

* ⬜ Restricted tokens (drop admin rights)
* ⬜ AppContainer support (filesystem + registry isolation)
* ⬜ Optional network restrictions (via AppContainer or WFP)

**Phase 3 — Linux/macOS Backend**

* ⬜ Fork/exec wrapper with timeout
* ⬜ `setrlimit` for memory/CPU limits
* ⬜ Namespace isolation (`unshare`, `chroot`) on Linux
* ⬜ macOS `sandbox_init` support

**Phase 4 — Cross-Platform Unification**

* ⬜ Unified config system (`SandboxConfig`)
* ⬜ Predefined sandbox profiles (e.g. *Safe Script*, *Network Blocked*, *CI Worker*)
* ⬜ Monitoring API (exit code, resource usage, signals)

---

## 🛠️ Build & Install

```bash
git clone https://github.com/yourname/GlassBox.git
cd GlassBox
cmake -B build
cmake --build build
```

### Examples

```bash
cd build/examples
./timeout_example
./memory_limit_example
./cpu_limit_example
```

---

## 📜 License

GlassBox is released under the \[choose-your-license-here].
(Recommended: Apache-2.0, MPL-2.0, or LGPL-3.0 depending on how you want reuse vs copyleft to work.)

---

## 🤝 Contributing

Contributions are welcome!
Areas where help is especially appreciated:

* Linux/macOS backend implementation
* Security research & AppContainer integration
* API design reviews
* Documentation & examples

---

## 🔮 Vision

The long-term goal of GlassBox is to provide a **portable, modern C++ sandboxing library** that developers can embed into tools, CI/CD systems, educational platforms, and security research environments.
The guiding principles are:

* **Clarity**: no raw WinAPI or syscalls exposed to the user
* **Safety**: RAII, strong types, and modern C++ idioms
* **Extensibility**: pluggable policies and config profiles
* **Cross-platform parity**: similar features across Windows, Linux, macOS

---

```