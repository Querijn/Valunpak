#pragma once

#include <valunpak/config.hpp>

#include <filesystem>

namespace valunpak
{
	class ue4_utexture2d;
	bool write_image(const ue4_utexture2d& a_texture, const std::filesystem::path& a_output_name);
}