#pragma once

#include <valunpak/config.hpp>
#include <valunpak/bin_file.hpp>

#include <filesystem>

namespace valunpak
{
	class ue4_bin_file : public bin_file
	{
	public:
		bool read_fname(std::string& a_string, size_t& a_offset);
		bool read_fstring(std::string& a_string, size_t& a_offset);
		bool read_fstring_path(std::filesystem::path& a_path, size_t& a_offset);
	};
}
