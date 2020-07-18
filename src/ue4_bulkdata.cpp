#include <valunpak/ue4_bulkdata.hpp>
#include <valunpak/ue4_ubulk.hpp>

namespace valunpak
{
	bool ue4_bulkdata::open(ue4_bin_file& a_parent, ue4_ubulk* a_ubulk, size_t& a_offset) noexcept
	{
		m_ubulk = a_ubulk;
		return open(a_parent, a_offset);
	}

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

	const std::vector<u8>& ue4_bulkdata::as_vector() const
	{
		return m_data;
	}

	void ue4_bulkdata::reset()
	{
		memset(&m_header, 0, sizeof(m_header));
		m_data.clear();
	}

	size_t ue4_bulkdata::read_internal()
	{
		size_t offset = 0;
		VALUNPAK_REQUIRE_RET(read(m_header, offset), 0);
		if (m_header.element_count == 0)
			return 0;

		if (has_flag<flag_type::force_inline_payload>(m_header.flags))
		{
			VALUNPAK_REQUIRE_RET(read_array(m_data, m_header.element_count, offset), 0);
		}
		else if (has_flag<flag_type::payload_in_seperate_file>(m_header.flags))
		{
			size_t offset = (i64)m_ubulk->get_bulk_offset() + (i64)m_header.offset_in_file;
			VALUNPAK_REQUIRE_RET(m_ubulk && m_ubulk->read_array(m_data, m_header.element_count, offset), 0);
		}

		return offset;
	}
}
