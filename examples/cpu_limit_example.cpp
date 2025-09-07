#include "../GlassBox.h"

int main() {
    glassbox::Sandbox sb;
    sb.setCpuPercent(10); // 10% CPU cap
    int code = sb.run("notepad.exe");
    std::println("Process exited with code {}", code);
}
