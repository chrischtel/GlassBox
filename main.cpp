#include "GlassBox.h"

int main() {
	glassbox::Sandbox sb;
	sb.run("notepad.exe", {"CMakeCache.txt"});
}