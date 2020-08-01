#pragma once

#include <valunpak/config.hpp>

#include <vector>
#include <string>

namespace valunpak
{
	class pak_filesystem;
	bool write_uasset(const std::string& name, pak_filesystem& a_paks, const std::vector<std::string>& a_files_to_skip);
}