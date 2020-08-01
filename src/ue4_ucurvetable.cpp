#include <valunpak/ue4_ucurvetable.hpp>
#include <valunpak/ue4_ubulk.hpp>
#include <valunpak/ue4_uexp.hpp>
#include <valunpak/ue4_bulkdata.hpp>
#include <valunpak/no_optimise.hpp>

namespace valunpak
{
	bool ue4_ucurvetable::open(ue4_uexp& a_uexp, ue4_ubulk* a_ubulk, size_t& a_offset) noexcept
	{
		reset();
		VALUNPAK_REQUIRE(ue4_bin_file::open(a_uexp, a_offset));

		size_t offset = 0;
		VALUNPAK_REQUIRE(m_object.open(a_uexp, *this, offset)); // Open the UObject header

		if (read_internal(a_uexp, a_ubulk, offset) == false)
		{
			reset();
			return false;
		}

		a_offset += offset;
		return true;
	}
	
	void ue4_ucurvetable::reset()
	{
		m_entries.clear();
	}

	bool ue4_ucurvetable::read_internal(ue4_uexp& a_uexp, ue4_ubulk* a_bulk, size_t& a_offset)
	{
		i32 row_count;
		VALUNPAK_REQUIRE(read(row_count, a_offset));

		return true;
	}
}

