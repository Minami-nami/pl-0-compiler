#include "Compiler.h"
#include <sys/stat.h>
#include <filesystem>

void Compiler::run(const std::string& path) {
	std::filesystem::path sourcePath = std::filesystem::path(path);
	std::filesystem::path codePath = std::filesystem::path(path).replace_extension(".pcode");
	if (std::filesystem::exists(codePath) && std::filesystem::exists(sourcePath)) {
		auto timesource = std::filesystem::last_write_time(sourcePath);
		auto timecode = std::filesystem::last_write_time(codePath);
		if (timesource <= timecode)
			return;
	}
	this->parser.loadFile(path);
	this->parser.analyze();
	this->parser.write();
}