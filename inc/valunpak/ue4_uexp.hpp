#pragma once

#include <valunpak/config.hpp>
#include <valunpak/ue4_bin_file.hpp>

#include <valunpak/vector_getter.hpp>

namespace valunpak
{
	class ue4_uasset;
	class ue4_base;
	class ue4_ubulk;
	class ue4_uexp : public ue4_bin_file
	{
	public:
		ue4_uexp();
		ue4_uexp(ue4_uasset& a_uasset, ue4_ubulk* a_ubulk);
		~ue4_uexp();

		virtual bool open(std::string_view a_file_name, read_mode_type a_read_mode = read_mode_type::stream) noexcept override;
		virtual bool open(const std::vector<u8>& a_data) noexcept override;
		virtual bool open(const u8* a_data, size_t a_size) noexcept override;
		virtual bool open(bin_file& a_reader, size_t& a_offset) noexcept override;

		using file_array = std::vector<std::unique_ptr<ue4_base>>;
		VALUNPAK_VECTOR_GETTER(ue4_uexp, file_array, m_files) files;

		const ue4_uasset& get_uasset() const { return *m_uasset; }
		const ue4_ubulk* get_ubulk() const { return m_ubulk; }

		friend class ue4_uobject;
		friend class ue4_utexture2d;
	private:
		ue4_uasset* m_uasset = nullptr;
		ue4_ubulk* m_ubulk = nullptr;
		file_array m_files;

		void reset();
		bool read_internal();

		void parse_texture(size_t& a_offset);
		void parse_object(size_t& a_offset);

		bool read_table_name(std::string& a_name, bin_file& a_file, size_t& a_offset) const;
		bool read_table_name(std::string& a_name, i32& a_number, bin_file& a_file, size_t& a_offset) const;
	};
}
