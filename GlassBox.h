// GlassBox.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>
#include <print>
#include <string>
#include <vector>   
namespace glassbox {
	class Sandbox {
	private:
		// Private members and methods can be declared here
		int GB_CreateProcess(const std::string& app, const std::vector<std::string>& args);
	public:
		int run(const std::string& path);
		int run(const std::string& path, const std::vector<std::string>& args);
	};
}

