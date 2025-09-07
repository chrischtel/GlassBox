#include "GlassBox.h"

int main() {
    glassbox::Sandbox sb;


    // Launch Notepad with a file and 5s timeout
    sb.run_with_timeout("notepad.exe", { "CMakeCache.txt" }, 5000);

    return 0;
}