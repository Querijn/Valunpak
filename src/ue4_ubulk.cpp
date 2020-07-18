#include <valunpak/ue4_ubulk.hpp>

namespace valunpak
{
	void ue4_ubulk::set_bulk_offset(size_t a_offset)
	{
		m_offset = a_offset;
	}

	size_t ue4_ubulk::get_bulk_offset() const
	{
		return m_offset;
	}
}