#include <valunpak/ue4_bulkdata.hpp>

#include <debugbreak.h>

namespace valunpak
{
	bool ue4_bulkdata::open(ue4_bin_file& a_parent, size_t& a_offset) noexcept
	{
		reset();
		if (ue4_bin_file::open(a_parent, a_offset) == false)
			return false;

		size_t offset = read_internal();
		if (offset == 0)
		{
			reset();
			return false;
		}

		a_offset += offset;
		return true;
	}

	const u8* ue4_bulkdata::data() const
	{
		return m_data.data();
	}

	size_t ue4_bulkdata::size() const
	{
		return m_data.size();
	}

	void ue4_bulkdata::reset()
	{
		memset(&m_header, 0, sizeof(m_header));
		m_data.clear();
	}

	size_t ue4_bulkdata::read_internal()
	{
		size_t offset = 0;
		if (read(m_header, offset) == false)
			return false;

		if (has_flag<flag_type::force_inline_payload>(m_header.flags) && m_header.element_count > 0)
		{
			if (read_array(m_data, m_header.element_count, offset) == false)
				return 0;
		}

		if (has_flag<flag_type::payload_in_seperate_file>(m_header.flags))
		{
			debug_break();
		}

		return offset;
	}
}
