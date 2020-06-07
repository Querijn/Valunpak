#pragma once

#include <valunpak/config.hpp>
#include <valunpak/bin_file.hpp>
#include <valunpak/aes.hpp>
#include <valunpak/sha1.hpp>

#include <memory>
#include <filesystem>
#include <map>

namespace valunpak
{
	class pak_file : public bin_file
	{
	public:
		pak_file();
		~pak_file();

		static const size_t compression_method_name_length = 32;
		static const size_t compression_method_names_count = 5;

	#pragma pack(push, 1)
		enum class version_type : u32
		{
			initial = 1,
			no_timestamps = 2,
			compression_encryption = 3,
			index_encryption = 4,
			relative_chunk_offsets = 5,
			delete_records = 6,
			encryption_key_guid = 7,
			fname_based_compression_method = 8,
			frozen_index = 9,
			path_hash_index = 10,

			last,
			invalid,
			latest = last - 1
		};

		enum class compression_flags : u32
		{
			none = 0x00,
			zlib = 0x01,
			gzip = 0x02,
			custom = 0x04,
			deprecated_format_flags_mask = 0xF,

			no_flags = 0x00,
			bias_memory = 0x10,
			bias_speed = 0x20,
			source_is_padded = 0x80,
			options_flags_mask = 0xF0,
		};

		struct info
		{
			static const size_t magic_def = 0x5A6F12E1;

			u32 encryption_key_guid[4]; // Encryption key guid. Empty if we should use the embedded key.
			bool encrypted_index; // Flag indicating if the pak index has been encrypted.

			u32 magic;
			version_type version;

			i64 index_offset; // Offset to pak file index.
			i64 index_size; // Size (in bytes) of pak file index.
			sha1 index_hash; // Index SHA1 value.
		};

		struct info_v8 : public info
		{
			char compression_method_names[compression_method_name_length * compression_method_names_count];
			// Compression methods used in this pak file (FNames, saved as FStrings)
		};

		struct info_v9 : public info
		{
			bool frozen_index;
			char compression_method_names[compression_method_name_length * compression_method_names_count];
			// Compression methods used in this pak file (FNames, saved as FStrings)
		};

		struct entry
		{
			struct header_data
			{
				i64 offset;
				i64 size;
				i64 uncompressed_size;
				u32 compression_method_index;
				u8 hash[20];
			} header;

			struct compressed_block
			{
				i64 compressed_start_offset;
				i64 compressed_end_offset;
			};

			u8 flags;
			u32 compression_block_size;
			std::vector<compressed_block> blocks;
			u32 size;
		};

	#pragma pack(pop)

		bool open(std::string_view a_file_name) noexcept override;
		bool open(std::string_view a_file_name, const std::vector<u8>& a_key) noexcept;
		const info* get_info() const;

		using entry_map = std::map<std::filesystem::path, std::unique_ptr<entry>>;
		entry_map::const_iterator begin() const;
		entry_map::const_iterator end() const;

	private:
		bool read_info();

		bool read_legacy_index(const u8* a_index_buffer, size_t a_index_size);
		bool read_index(const u8* a_index_buffer, size_t a_index_size);

		std::unique_ptr<info> m_info = nullptr;
		std::unique_ptr<aes> m_aes = nullptr;
		std::filesystem::path m_mount_point;
		entry_map m_entries;

		static_assert(sizeof(info_v8) == 221, "The PAK info with compression methods has an incorrect size.");
		static_assert(sizeof(info_v9) == 222, "The PAK info with frozen indices has an incorrect size.");
	};
}
