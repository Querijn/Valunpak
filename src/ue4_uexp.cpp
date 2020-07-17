#include <valunpak/ue4_uexp.hpp>
#include <valunpak/ue4_utexture2d.hpp>
#include <valunpak/ue4_uobject.hpp>
#include <valunpak/ue4_uasset.hpp>
#include <valunpak/ue4_base.hpp>

#include <cassert>

#include <debugbreak.h>

namespace valunpak
{
	ue4_uexp::ue4_uexp() : files(this) {}

	ue4_uexp::ue4_uexp(ue4_uasset& a_uasset, ue4_bin_file* a_ubulk) :
		files(this), m_uasset(&a_uasset), m_ubulk(a_ubulk)
	{
	}

	ue4_uexp::~ue4_uexp()
	{
	}

	bool ue4_uexp::open(std::string_view a_file_name, read_mode_type a_read_mode) noexcept
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

	bool ue4_uexp::open(const std::vector<u8>& a_data) noexcept
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

	bool ue4_uexp::open(const u8* a_data, size_t a_size) noexcept
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

	bool ue4_uexp::open(bin_file& a_reader, size_t& a_offset) noexcept
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

	void ue4_uexp::reset()
	{
		m_files.clear();
	}

	bool ue4_uexp::read_internal()
	{
		VALUNPAK_REQUIRE(m_uasset);

		// export all files
		for (auto& exp : m_uasset->m_exports)
		{
			size_t offset = exp.serial_offset - m_uasset->header_size;
			size_t start_offset = offset;
			assert(exp.class_index != 0);

			// Indicate file type
			const std::string* type_name = nullptr;
			i32 ind = exp.class_index;
			if (exp.class_index.is_export())
				type_name = &m_uasset->m_names[m_uasset->m_exports[exp.class_index].name_index];
			else if (exp.class_index.is_import())
				type_name = &m_uasset->m_imports[exp.class_index].object_name;
			else 
				return false;

			if (*type_name == "" || *type_name == "ObjectProperty")
				parse_object(offset);
			else if (*type_name == "Texture2D")
				parse_texture(offset);
			/*
			else if (*type_name == "CurveTable")
				type = file_type::curve_table;
			else if (*type_name == "DataTable")
				type = file_type::data_table;
			else if (*type_name == "FontFace")
				type = file_type::font_face;
			else if (*type_name == "SoundWave")
				type = file_type::sound_wave;
			else if (*type_name == "StringTable")
				type = file_type::string_table;
			*/
			else
				parse_object(offset);

			// VALUNPAK_REQUIRE(start_offset + exp.serial_size == offset);
		}

		return true;
	}

	void ue4_uexp::parse_texture(size_t& a_offset)
	{
		auto texture = std::make_unique<ue4_utexture2d>();
		if (texture->open(*this, m_ubulk, a_offset))
			m_files.push_back(std::move(texture));
	}

	void ue4_uexp::parse_object(size_t& a_offset)
	{
		auto obj = std::make_unique<ue4_uobject>();
		if (obj->open(*this, a_offset))
			m_files.push_back(std::move(obj));
	}

	bool ue4_uexp::read_table_name(std::string& a_name, bin_file& a_file, size_t& a_offset) const
	{
		return m_uasset && m_uasset->read_table_name(a_name, a_file, a_offset);
	}

	bool ue4_uexp::read_table_name(std::string& a_name, i32& a_number, bin_file& a_file, size_t& a_offset) const
	{
		return m_uasset && m_uasset->read_table_name(a_name, a_number, a_file, a_offset);
	}
}

