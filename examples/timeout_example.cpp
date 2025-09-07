#include "../GlassBox.h"

int main() {
    glassbox::Sandbox sb;
    sb.setTimeoutMs(3000); // 3 seconds
    int code = sb.run("notepad.exe");
    std::println("Process exited with code {}", code);
}
