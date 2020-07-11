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

		if (read_internal(a_offset) == false)
		{
			reset();
			return false;
		}

		return true;
	}

	void ue4_utexture2d::reset()
	{
		memset(&m_header, 0, sizeof(m_header));
	}

#pragma optimize("", off)
	bool ue4_utexture2d::read_internal(size_t& a_offset)
	{
		if (read(m_header, a_offset) == false)
			return 0;

		// TODO:
		if (m_header.is_cooked)
		{
			debug_break();
		}

		return a_offset;
	}
}

