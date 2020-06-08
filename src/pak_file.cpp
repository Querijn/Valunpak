#include <valunpak/pak_file.hpp>
#include <valunpak/ue4_bin_file.hpp>

#include <rijndael/rijndael.hpp>
#include <debugbreak.h>

namespace valunpak
{
	namespace fs = std::filesystem;

	bool pak_file::read_info()
	{
		size_t size = get_size();

		// Try the base
		pak_file::info info_base;
		size_t offset = size - sizeof(pak_file::info);
		if (read(info_base, offset) == false)
			return false;

		if (info_base.magic == pak_file::info::magic_def)
		{
			m_info = std::make_unique<pak_file::info>(info_base);
			return true;
		}

		// try v8
		pak_file::info_v8 info_v8;
		offset = size - sizeof(pak_file::info_v8);
		if (read(info_v8, offset) == false)
			return false;

		if (info_v8.magic == pak_file::info::magic_def)
		{
			m_info = std::make_unique<pak_file::info_v8>(info_v8);
			return true;
		}

		// try v9
		pak_file::info_v9 info_v9;
		offset = size - sizeof(pak_file::info_v9);
		if (read(info_v9, offset) == false)
			return false;

		if (info_v8.magic == pak_file::info::magic_def)
		{
			m_info = std::make_unique<pak_file::info_v9>(info_v9);
			return true;
		}

		// Cannot figure out the pak version.
		return false;
	}

	bool pak_file::open(std::string_view a_file_name) noexcept
	{
		m_aes = nullptr;
		m_info = nullptr;
		if (bin_file::open(a_file_name) == false)
			return false;

		if (read_info() == false)
			return false;

		return true;
	}

	bool pak_file::open(std::string_view a_file_name, const std::vector<u8>& a_key) noexcept
	{
		if (pak_file::open(a_file_name) == false)
			return false;

		if (m_info->encrypted_index == false)
			return true;

		size_t offset = m_info->index_offset;
		std::vector<u8> index_buffer;
		if (read_array(index_buffer, m_info->index_size, offset) == false)
			return false;

		// Setup AES
		m_aes = std::make_unique<aes>(a_key.data());
		m_aes->decrypt(index_buffer.data(), index_buffer.size());

		if (m_info->version >= version_type::path_hash_index)
		{
			debug_break(); // TODO: Implement
			if (!read_index(index_buffer.data(), index_buffer.size()))
				return false;
		}
		else
		{
			if (read_legacy_index(index_buffer.data(), index_buffer.size()) == false)
				return false;
		}
	}

	bool pak_file::read_legacy_index(const u8* a_index_data, size_t a_index_size)
	{
		ue4_bin_file mem_file;
		mem_file.open(a_index_data, a_index_size);
		size_t offset = 0;

		mem_file.read_fstring_path(m_mount_point, offset);

		i32 entries; 
		mem_file.read(entries, offset);

		for (i32 i = 0; i < entries; i++)
		{
			fs::path path;
			mem_file.read_fstring_path(path, offset);

			auto start_offset = offset;
			auto cur_entry = std::make_unique<entry>();
			mem_file.read(cur_entry->header, offset);

			if (m_info->version < pak_file::version_type::fname_based_compression_method)
			{
				debug_break(); // TODO: Test
				if (cur_entry->header.compression_method_index == (u32)compression_flags::none)
					cur_entry->header.compression_method_index = 0;
				else if ((cur_entry->header.compression_method_index & (u32)compression_flags::zlib) != 0)
					cur_entry->header.compression_method_index = 1;
				else if ((cur_entry->header.compression_method_index & (u32)compression_flags::gzip) != 0)
					cur_entry->header.compression_method_index = 2;
				else if ((cur_entry->header.compression_method_index & (u32)compression_flags::custom) != 0)
					cur_entry->header.compression_method_index = 3;
			}

			if (cur_entry->header.compression_method_index != 0)
			{
				debug_break(); // TODO: Test
				// Read all compression blocks
				u32 block_count;
				mem_file.read(block_count, offset);
				for (u32 j = 0; j < block_count; j++)
				{
					entry::compressed_block block;
					mem_file.read(block, offset);

					cur_entry->blocks.push_back(block);
				}
				cur_entry->size += block_count * sizeof(entry::compressed_block);
			}

			mem_file.read(cur_entry->flags, offset);
			mem_file.read(cur_entry->compression_block_size, offset);
			cur_entry->size = offset - start_offset;
			m_entries[path] = std::move(cur_entry);
		}

		return true;
	}

	bool pak_file::read_index(const u8* a_index_buffer, size_t a_size)
	{
		return false;
	}
	
	const pak_file::info* pak_file::get_info() const
	{
		return m_info.get();
	}

	pak_file::entry_map::const_iterator pak_file::begin() const
	{
		return m_entries.begin();
	}

	pak_file::entry_map::const_iterator pak_file::end() const
	{
		return m_entries.end();
	}

	pak_file::pak_file() {}
	pak_file::~pak_file() {}
}