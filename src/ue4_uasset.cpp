#include <valunpak/ue4_uasset.hpp>

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
		offset += sizeof(i32); // Skip beyond total header size

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
			if (read_fname(imp.class_package, offset) == false ||
				read_fname(imp.class_name, offset) == false ||
				read(imp.outer_index, offset) == false ||
				read_fname(imp.object_name, offset) == false)
				return false;

			m_imports.push_back(imp);
		}

		// parse exports
		offset = m_info_header.export_offset;
		for (i32 i = 0; i < m_info_header.export_count; i++)
		{
			package_export exp;
			if (read(exp, offset) == false)
				return false;

			m_exports.push_back(exp);
		}

		return true;
	}
}

