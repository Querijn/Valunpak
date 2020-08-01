#include <valunpak/ue4_uexp.hpp>
#include <valunpak/ue4_utexture2d.hpp>
#include <valunpak/ue4_usoundwave.hpp>
#include <valunpak/ue4_ustringtable.hpp>
#include <valunpak/ue4_ucurvetable.hpp>
#include <valunpak/ue4_udatatable.hpp>
#include <valunpak/ue4_uobject.hpp>
#include <valunpak/ue4_uasset.hpp>
#include <valunpak/ue4_ubulk.hpp>
#include <valunpak/ue4_base.hpp>

#include <debugbreak.h>

namespace valunpak
{
	ue4_uexp::ue4_uexp() : files(this) {}

	ue4_uexp::ue4_uexp(ue4_uasset& a_uasset, ue4_ubulk* a_ubulk) :
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

		if (m_ubulk)
		{
			size_t ubulk_size = 0;
			for (auto& exp : m_uasset->m_exports)
				ubulk_size += exp.serial_size;
			m_ubulk->set_bulk_offset(ubulk_size + m_uasset->header_size);
		}

		// export all files
		for (auto& exp : m_uasset->m_exports)
		{
			size_t offset = exp.serial_offset - m_uasset->header_size;
			size_t start_offset = offset;

			// Indicate file type
			const std::string* type_name = nullptr;
			i32 ind = exp.class_index;
			if (exp.class_index.is_export())
				type_name = &m_uasset->m_names[m_uasset->m_exports[exp.class_index].name_index];
			else if (exp.class_index.is_import())
				type_name = &m_uasset->m_imports[exp.class_index].object_name;
			else
				return false;

			switch (fnv(type_name->c_str()))
			{
			default:
			case fnv(""):
			case fnv("ObjectProperty"):
				parse_object(offset);
				break;

			case fnv("Texture2D"):
				parse_texture(offset);
				break;

			case fnv("CurveTable"):
				parse_curvetable(offset);
				break;

			case fnv("DataTable"):
				parse_datatable(offset);
				break;

			case fnv("FontFace"):
				VALUNPAK_REQUIRE(false);
				break;

			case fnv("SoundWave"):
				parse_soundwave(offset);
				break;

			case fnv("StringTable"):
				parse_stringtable(offset);
				break;
			}

			// VALUNPAK_REQUIRE(start_offset + exp.serial_size == offset);
		}

		return true;
	}

	void ue4_uexp::parse_stringtable(size_t& a_offset)
	{
		auto table = std::make_unique<ue4_ustringtable>();
		if (table->open(*this, m_ubulk, a_offset))
			m_files.push_back(std::move(table));
	}

	void ue4_uexp::parse_curvetable(size_t& a_offset)
	{
		auto table = std::make_unique<ue4_ucurvetable>();
		if (table->open(*this, m_ubulk, a_offset))
			m_files.push_back(std::move(table));
	}

	void ue4_uexp::parse_datatable(size_t& a_offset)
	{
		auto table = std::make_unique<ue4_udatatable>();
		if (table->open(*this, m_ubulk, a_offset))
			m_files.push_back(std::move(table));
	}

	void ue4_uexp::parse_soundwave(size_t& a_offset)
	{
		auto wave = std::make_unique<ue4_usoundwave>();
		if (wave->open(*this, m_ubulk, a_offset))
			m_files.push_back(std::move(wave));
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
		if (obj->open(*this, *this, a_offset))
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

