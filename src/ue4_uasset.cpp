#include <valunpak/ue4_uasset.hpp>
#include <valunpak/ue4_utexture2d.hpp>
#include <valunpak/ue4_uobject.hpp>

namespace valunpak
{
	ue4_uasset::ue4_uasset() : exports(this) {}

	bool ue4_uasset::open(std::string_view a_file_name, read_mode_type a_read_mode) noexcept
	{
		reset();
		VALUNPAK_REQUIRE(ue4_bin_file::open(a_file_name, a_read_mode));

		if (read_internal() == false)
		{
			reset();
			return false;
		}

		return true;
	}

	bool ue4_uasset::open(const std::vector<u8>& a_data) noexcept
	{
		reset();
		VALUNPAK_REQUIRE(ue4_bin_file::open(a_data));

		if (read_internal() == false)
		{
			reset();
			return false;
		}

		return true;
	}
	
	bool ue4_uasset::open(const u8* a_data, size_t a_size) noexcept
	{
		reset();
		VALUNPAK_REQUIRE(ue4_bin_file::open(a_data, a_size));

		if (read_internal() == false)
		{
			reset();
			return false;
		}

		return true;
	}

	bool ue4_uasset::open(bin_file& a_reader, size_t& a_offset) noexcept
	{
		reset();
		VALUNPAK_REQUIRE(ue4_bin_file::open(a_reader, a_offset));

		if (read_internal() == false)
		{
			reset();
			return false;
		}

		return true;
	}

	bool ue4_uasset::get_name(std::string& a_string, size_t index) const
	{
		if (index >= m_names.size())
			return false;

		a_string = m_names[index];
		return true;
	}
	
	bool ue4_uasset::package_version_header::is_valid() const
	{
		return magic == magic_def;
	}
	
	void ue4_uasset::reset()
	{
		m_names.clear();
		memset(&m_version_header, 0, sizeof(m_version_header));
		memset(&m_info_header, 0, sizeof(m_info_header));
	}

	bool ue4_uasset::read_internal()
	{
		size_t offset = 0;
		VALUNPAK_REQUIRE(read(m_version_header, offset) && m_version_header.is_valid());

		offset += (size_t)m_version_header.custom_versions * 5 * sizeof(i32); // Skip beyond custom versions
		VALUNPAK_REQUIRE(read(header_size, offset));

		std::string folder_name;
		read_fstring(folder_name, offset);

		VALUNPAK_REQUIRE(read(m_info_header, offset));

		// parse names
		offset = m_info_header.name_offset;
		for (i32 i = 0; i < m_info_header.name_count; i++)
		{
			std::string name;
			VALUNPAK_REQUIRE(read_fname(name, offset));

			m_names.push_back(name);
		}

		// parse imports
		offset = m_info_header.import_offset;
		for (i32 i = 0; i < m_info_header.import_count; i++)
		{
			package_import imp;
			VALUNPAK_REQUIRE(read_table_name(imp.class_package, *this, offset));
			VALUNPAK_REQUIRE(read_table_name(imp.class_name, *this, offset));
			VALUNPAK_REQUIRE(read(imp.outer_index, offset));
			VALUNPAK_REQUIRE(read_table_name(imp.object_name, *this, offset));
			m_imports.push_back(imp);
		}

		// parse exports
		offset = m_info_header.export_offset;
		VALUNPAK_REQUIRE(read_array(m_exports, m_info_header.export_count, offset));
		return true;
	}
	
	bool ue4_uasset::read_table_name(std::string& a_name, bin_file& a_file, size_t& a_offset) const
	{
		i32 number;
		return read_table_name(a_name, number, a_file, a_offset);
	}

	bool ue4_uasset::read_table_name(std::string& a_name, i32& a_number, bin_file& a_file, size_t& a_offset) const
	{
		i32 name_index;
		VALUNPAK_REQUIRE(a_file.read(name_index, a_offset) && a_file.read(a_number, a_offset));
		return get_name(a_name, name_index);
	}
}

