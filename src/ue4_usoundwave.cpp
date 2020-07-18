#include <valunpak/ue4_usoundwave.hpp>
#include <valunpak/ue4_ubulk.hpp>
#include <valunpak/ue4_uexp.hpp>
#include <valunpak/ue4_bulkdata.hpp>

namespace valunpak
{
	bool ue4_usoundwave::open(ue4_uexp& a_uexp, ue4_ubulk* a_ubulk, size_t& a_offset) noexcept
	{
		reset();
		VALUNPAK_REQUIRE(ue4_bin_file::open(a_uexp, a_offset));

		size_t offset = 0;
		VALUNPAK_REQUIRE(m_object.open(a_uexp, offset)); // Open the UObject header

		if (read_internal(a_uexp, a_ubulk, offset) == false) // Read the soundwave
		{
			reset();
			return false;
		}

		a_offset += offset;
		return true;
	}
	
	void ue4_usoundwave::reset()
	{
		m_is_streaming = false;
		m_is_cooked = false;
		memset(m_guid, 0, sizeof(u32) * 4);
	}

	bool ue4_usoundwave::read_internal(ue4_uexp& a_uexp, ue4_ubulk* a_bulk, size_t& a_offset)
	{
		VALUNPAK_REQUIRE(read(m_is_cooked, a_offset));

		auto prop = m_object.get_prop("bStreaming");
		if (prop != nullptr)
			VALUNPAK_REQUIRE(prop->get_bool_value(m_is_streaming));

		if (m_is_streaming)
		{
			debug_break(); // TODO
			VALUNPAK_REQUIRE(read(m_guid, a_offset));
			i32 chunk_count;
			VALUNPAK_REQUIRE(read(chunk_count, a_offset));

			std::string format;
			VALUNPAK_REQUIRE(a_uexp.read_table_name(format, *this, a_offset));

			std::vector<u8> full_sound;
			for (i32 i = 0; i < chunk_count; i++)
			{
				i32 is_cooked;
				VALUNPAK_REQUIRE(read(is_cooked, a_offset) && is_cooked);

				ue4_bulkdata data;
				VALUNPAK_REQUIRE(data.open(*this, a_bulk, a_offset));

				i32 size;
				VALUNPAK_REQUIRE(data.read(size, a_offset));
				VALUNPAK_REQUIRE(read(size, a_offset));

				auto& chunk_data = data.as_vector();
				full_sound.insert(full_sound.end(), chunk_data.begin(), chunk_data.end());
			}
			m_data_by_format[format] = full_sound;
			return true;
		}

		if (m_is_cooked)
		{
			i32 format_count;
			VALUNPAK_REQUIRE(read(format_count, a_offset));
			
			for (i32 i = 0; i < format_count; i++)
			{
				std::string format;
				VALUNPAK_REQUIRE(a_uexp.read_table_name(format, *this, a_offset));

				ue4_bulkdata data;
				VALUNPAK_REQUIRE(data.open(*this, a_bulk, a_offset));
				m_data_by_format[format] = data.as_vector();
			}

			VALUNPAK_REQUIRE(read(m_guid, a_offset));
			return true;
		}

		debug_break(); // TODO

		VALUNPAK_REQUIRE(read(m_guid, a_offset));
		return true;
	}
}

