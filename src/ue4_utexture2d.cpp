#include <valunpak/ue4_utexture2d.hpp>
#include <valunpak/ue4_uexp.hpp>

#include <cassert>

#include <debugbreak.h>

namespace valunpak
{
	bool ue4_utexture2d::open(ue4_uexp& a_parent, size_t& a_offset) noexcept
	{
		reset();
		if (ue4_bin_file::open(a_parent, a_offset) == false)
			return false;

		if (m_object.open(a_parent, a_offset) == false)
			return false;

		size_t offset_increment = read_internal();
		if (offset_increment == 0)
		{
			reset();
			return false;
		}

		a_offset += offset_increment;
		return true;
	}

	void ue4_utexture2d::reset()
	{
		memset(&m_header, 0, sizeof(m_header));
	}

#pragma optimize("", off)
	size_t ue4_utexture2d::read_internal()
	{
		size_t offset = 0;
		if (read(m_header, offset) == false)
			return 0;

		return offset;
	}
}

