#include "../GlassBox.h"

int main() {
    glassbox::Sandbox sb;
    int code = sb.run_with_timeout("cpu_hog.exe", 5000);
    std::println("Process exited with code {}", code);

    if (auto stats = sb.getLastStats()) {
        std::println("Peak Memory: {} bytes", stats->peakMemoryBytes);
        std::println("User Time: {} (100ns)", stats->userTime100ns);
        std::println("Kernel Time: {} (100ns)", stats->kernelTime100ns);
    }
}
