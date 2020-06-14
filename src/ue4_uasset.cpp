#include <valunpak/ue4_uasset.hpp>
#include <valunpak/ue4_utexture2d.hpp>
#include <valunpak/ue4_uobject.hpp>

#include <cassert>

#include <cstring>

#include <debugbreak.h>

namespace valunpak
{
	bool ue4_uasset::open(std::string_view a_file_name, read_mode_type a_read_mode) noexcept
	{
		reset();
		if (ue4_bin_file::open(a_file_name, a_read_mode) == false)
			return false;

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
		if (ue4_bin_file::open(a_data) == false)
			return false;

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
		if (ue4_bin_file::open(a_data, a_size) == false)
			return false;

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
		if (ue4_bin_file::open(a_reader, a_offset) == false)
			return false;

		if (read_internal() == false)
		{
			reset();
			return false;
		}

		return true;
	}

	std::string ue4_uasset::get_name(size_t index) const
	{
		if (index < m_names.size())
			return m_names[index];

		return "";
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
		if (read(m_version_header, offset) == false || m_version_header.is_valid() == false)
			return false;

		offset += (size_t)m_version_header.custom_versions * 5 * sizeof(i32); // Skip beyond custom versions
		if (read(header_size, offset) == false)
			return false;

		std::string folder_name;
		read_fstring(folder_name, offset);

		if (read(m_info_header, offset) == false)
			return false;

		// parse names
		offset = m_info_header.name_offset;
		for (i32 i = 0; i < m_info_header.name_count; i++)
		{
			std::string name;
			if (read_fname(name, offset) == false)
				return false;

			m_names.push_back(name);
		}

		// parse imports
		offset = m_info_header.import_offset;
		for (i32 i = 0; i < m_info_header.import_count; i++)
		{
			package_import imp;
			if (read_table_name(imp.class_package, *this, offset) == false ||
				read_table_name(imp.class_name, *this, offset) == false ||
				read(imp.outer_index, offset) == false ||
				read_table_name(imp.object_name, *this, offset) == false)
				return false;

			m_imports.push_back(imp);
		}

		// parse exports
		offset = m_info_header.export_offset;
		if (read_array(m_exports, m_info_header.export_count, offset) == false)
			return false;

		return true;
	}
	
	bool ue4_uasset::read_table_name(std::string& a_name, bin_file& a_file, size_t& a_offset) const
	{
		i32 number;
		return read_table_name(a_name, number, a_file, a_offset);
	}

	bool ue4_uasset::read_table_name(std::string& a_name, i32& a_number, bin_file& a_file, size_t& a_offset) const
	{
		i32 name_index, number;
		if (a_file.read(name_index, a_offset) == false || a_file.read(number, a_offset) == false)
			return false;

		a_name = get_name(name_index);
		return true;
	}
}

