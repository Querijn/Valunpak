#include <valunpak/ue4_ustringtable.hpp>
#include <valunpak/ue4_ubulk.hpp>
#include <valunpak/ue4_uexp.hpp>
#include <valunpak/ue4_bulkdata.hpp>
#include <valunpak/no_optimise.hpp>

namespace valunpak
{
	bool ue4_ustringtable::open(ue4_uexp& a_uexp, ue4_ubulk* a_ubulk, size_t& a_offset) noexcept
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
	
	void ue4_ustringtable::reset()
	{
		m_entries.clear();
	}

	bool ue4_ustringtable::read_internal(ue4_uexp& a_uexp, ue4_ubulk* a_bulk, size_t& a_offset)
	{
		size_t real_offset = get_debug_offset(a_offset);
		std::string name_space;
		VALUNPAK_REQUIRE(read_fstring(name_space, a_offset));
		m_entries =
		{
			{ name_space, std::map<std::string, std::string>() }
		};

		i32 entries;
		VALUNPAK_REQUIRE(read(entries, a_offset));
		for (int i = 0; i < entries; i++)
		{
			std::string key;
			VALUNPAK_REQUIRE(read_fstring(key, a_offset));
			VALUNPAK_REQUIRE(read_fstring(m_entries[name_space][key], a_offset));
		}
		return true;
	}
}

