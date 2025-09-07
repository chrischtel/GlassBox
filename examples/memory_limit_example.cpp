#include "../GlassBox.h"

int main() {
    glassbox::Sandbox sb;
    sb.setMemoryLimitMB(64); // 64 MB
    int code = sb.run("notepad.exe");
    std::println("Process exited with code {}", code);
}
