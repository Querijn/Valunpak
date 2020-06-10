#pragma once

#include <valunpak/config.hpp>
#include <valunpak/ue4_bin_file.hpp>

namespace valunpak
{
	class ue4_uasset : public ue4_bin_file
	{
	public:
		virtual bool open(std::string_view a_file_name, read_mode_type a_read_mode = read_mode_type::stream) noexcept override;
		virtual bool open(const std::vector<u8>& a_data) noexcept override;
		virtual bool open(const u8* a_data, size_t a_size) noexcept override;

	#pragma pack(push, 1)
		struct package_version_header
		{
			static const i32 magic_def = 0x9E2A83C1;

			i32 magic;
			i32 version;
			i32 unknown;
			i32 file_version;
			i32 unknown2;
			i32 custom_versions;

			bool is_valid() const;
		};

		struct package_info_header
		{
			u32 flags;
			i32 name_count;
			i32 name_offset;

			i32 text_data_count;
			i32 text_data_offset;

			i32 export_count;
			i32 export_offset;

			i32 import_count;
			i32 import_offset;

			i32 depends_offset;

			i32 soft_package_references_count;
			i32 soft_package_references_offset;

			i32 searchable_names_offset;

			i32 thumbnail_table_offset;
			i32 guid[4];
		};
		
		struct package_export
		{
			i32 class_index;
			i32 super_index;
			i32 template_index;

			i32 outer_index;

			i32 name_index;
			i32 name_number;

			u32 object_flags;

			i64 serial_size;
			i64 serial_offset;

			i32 is_forced_export;
			i32 not_for_client;
			i32 not_for_server;

			u32 package_guid[4];
			u32 package_flags;

			i32 not_always_loaded_for_editor;

			i32 is_asset;

			i32 first_export_dependency;
			i32 sbs_deps;
			i32 cbs_deps;
			i32 sbc_deps;
			i32 cbc_deps;
		};

	#pragma pack(pop)

		struct package_import
		{
			std::string class_package;
			std::string class_name;
			i32 outer_index;
			std::string object_name;
		};

	private:
		package_version_header m_version_header;
		package_info_header m_info_header;
		std::vector<std::string> m_names;
		std::vector<package_import> m_imports;
		std::vector<package_export> m_exports;

		void reset();
		bool read_internal();
	};
}
