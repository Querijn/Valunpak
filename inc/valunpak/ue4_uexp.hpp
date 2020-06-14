#pragma once

#include <valunpak/config.hpp>
#include <valunpak/ue4_bin_file.hpp>

namespace valunpak
{
	class ue4_uasset;
	class ue4_uexp : public ue4_bin_file
	{
	public:
		ue4_uexp() {}
		ue4_uexp(ue4_uasset& a_uasset);

		virtual bool open(std::string_view a_file_name, read_mode_type a_read_mode = read_mode_type::stream) noexcept override;
		virtual bool open(const std::vector<u8>& a_data) noexcept override;
		virtual bool open(const u8* a_data, size_t a_size) noexcept override;
		virtual bool open(bin_file& a_reader, size_t& a_offset) noexcept override;

	#pragma pack(push, 1)
		
	#pragma pack(pop)

		friend class ue4_uobject;
	private:
		ue4_uasset* m_uasset = nullptr;
		std::vector<std::unique_ptr<ue4_bin_file>> m_files;

		void reset();
		bool read_internal();

		void parse_texture(size_t& a_offset);
		void parse_object(size_t& a_offset);

		bool read_table_name(std::string& a_name, bin_file& a_file, size_t& a_offset) const;
		bool read_table_name(std::string& a_name, i32& a_number, bin_file& a_file, size_t& a_offset) const;
	};
}
