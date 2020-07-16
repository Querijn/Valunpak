#pragma once

#include <valunpak/config.hpp>
#include <valunpak/ue4_bin_file.hpp>

namespace valunpak
{
	class ue4_uexp;
	class ue4_base : public ue4_bin_file
	{
	public:
		virtual bool is_texture() const { return false; }
	};
}
