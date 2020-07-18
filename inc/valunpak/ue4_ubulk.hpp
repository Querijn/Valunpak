#pragma once

#include <valunpak/config.hpp>
#include <valunpak/ue4_bin_file.hpp>

namespace valunpak
{
	class ue4_ubulk : public ue4_bin_file
	{
	public:
		void set_bulk_offset(size_t a_offset);
		size_t get_bulk_offset() const;

	private:
		size_t m_offset = 0;
	};
}