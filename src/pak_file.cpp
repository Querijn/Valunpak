#include <valunpak/pak_file.hpp>
#include <valunpak/pak_filesystem.hpp>
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
			m_info = std::make_shared<pak_file::info>(info_base);
			return true;
		}

		// try v8
		pak_file::info_v8 info_v8;
		offset = size - sizeof(pak_file::info_v8);
		if (read(info_v8, offset) == false)
			return false;

		if (info_v8.magic == pak_file::info::magic_def)
		{
			m_info = std::make_shared<pak_file::info_v8>(info_v8);
			return true;
		}

		// try v9
		pak_file::info_v9 info_v9;
		offset = size - sizeof(pak_file::info_v9);
		if (read(info_v9, offset) == false)
			return false;

		if (info_v8.magic == pak_file::info::magic_def)
		{
			m_info = std::make_shared<pak_file::info_v9>(info_v9);
			return true;
		}

		// Cannot figure out the pak version.
		return false;
	}

	bool pak_file::open(std::string_view a_file_name, bin_file::read_mode_type a_read_mode) noexcept
	{
		m_info = nullptr;
		VALUNPAK_REQUIRE(bin_file::open(a_file_name, a_read_mode));

		if (read_info() == false) // Not a pak file. This is fine for specific files (CEF.PAK for instance)
			return false;

		if (m_info->encrypted_index == false)
			return true;

		size_t offset = m_info->index_offset;
		std::vector<u8> index_buffer;
		VALUNPAK_REQUIRE(read_array(index_buffer, m_info->index_size, offset));

		// Setup AES
		for (auto& key : pak_filesystem::keys)
		{
			key->decrypt(index_buffer.data(), index_buffer.size());
			if (m_info->version > version_type::latest)
				continue;

			if (m_info->version >= version_type::path_hash_index)
			{
				debug_break(); // TODO: Implement
				VALUNPAK_REQUIRE(!read_index(index_buffer.data(), index_buffer.size()));
			}
			else
			{
				VALUNPAK_REQUIRE(read_legacy_index(index_buffer.data(), index_buffer.size()));
			}

			return true;
		}

		return false;
	}

	bool pak_file::read_legacy_index(const u8* a_index_data, size_t a_index_size)
	{
		ue4_bin_file mem_file;
		mem_file.open(a_index_data, a_index_size);
		size_t offset = 0;

		mem_file.read_fstring_path(m_mount_point, offset);
		fs::path root = fs::path(m_file_name).parent_path();
		m_mount_point = fs::absolute(root / m_mount_point); // Make it an absolute path.

		i32 entries; 
		mem_file.read(entries, offset);

		for (i32 i = 0; i < entries; i++)
		{
			fs::path path;
			mem_file.read_fstring_path(path, offset);

			auto start_offset = offset;
			auto cur_entry = std::make_shared<entry>();
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
				cur_entry->header_size += block_count * sizeof(entry::compressed_block);
			}

			mem_file.read(cur_entry->flags, offset);
			mem_file.read(cur_entry->compression_block_size, offset);
			cur_entry->header_size = static_cast<u32>(offset - start_offset);
			m_entries[path] = std::move(cur_entry);
		}

		return true;
	}

	bool pak_file::read_index(const u8* a_index_buffer, size_t a_size)
	{
		debug_break(); // TODO: Implement
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

	pak_file::entry_map::const_iterator pak_file::get_entry(std::string_view a_file_name) const
	{
		auto mount_point = m_mount_point.generic_string();
		auto mount_point_offset = a_file_name.find(mount_point);

		const char* file_name = a_file_name.data();
		if (mount_point_offset == 0)
			file_name += mount_point.length();

		return m_entries.find(file_name);
	}

	bool pak_file::get_file_data(std::string_view a_file_name, std::vector<u8>& a_buffer) const
	{
		a_buffer.clear();
		auto entry_index = get_entry(a_file_name);
		if (entry_index == end())
			return false;

		auto& entry = *entry_index->second;
		size_t size = get_file_size(entry);
		a_buffer.resize(size);
		return get_file_data(entry, a_buffer.data(), size);
	}

	bool pak_file::get_file_data(std::string_view a_file_name, bin_file* a_bin) const
	{
		std::vector<u8> buffer;
		if (get_file_data(a_file_name, buffer) == false)
			return false;
		return a_bin ? a_bin->open(buffer) : false;
	}

	bool pak_file::get_file_data(std::string_view a_file_name, u8* a_buffer, size_t a_size) const
	{
		auto entry_index = get_entry(a_file_name);
		if (entry_index == end())
			return false;

		auto& entry = *entry_index->second;
		return get_file_data(entry, a_buffer, a_size);
	}

	bool pak_file::get_file_data(const entry& a_entry, std::vector<u8>& a_buffer) const
	{
		a_buffer.clear();
		size_t size = get_file_size(a_entry);
		a_buffer.resize(size);
		return get_file_data(a_entry, a_buffer.data(), size);
	}

	bool pak_file::get_file_data(const entry& a_entry, bin_file* a_bin) const
	{
		std::vector<u8> buffer;
		if (get_file_data(a_entry, buffer) == false)
			return false;
		return a_bin ? a_bin->open(buffer) : false;
	}

	size_t pak_file::get_file_size(std::string_view a_file_name) const
	{
		auto entry_index = get_entry(a_file_name);
		if (entry_index == end())
			return false;

		auto& entry = *entry_index->second;
		return get_file_size(entry);
	}

	size_t pak_file::get_file_size(const pak_file::entry& a_entry) const
	{
		if (a_entry.header.compression_method_index == 0)
		{
			if (has_flag<entry::flag_type::encrypted>(a_entry.flags) == false)
				return a_entry.header.uncompressed_size;

			return (a_entry.header.size & 15) == 0 ? a_entry.header.size : (a_entry.header.size / 16 + 1) * 16;
		}

		debug_break(); // TODO: implement
		return 0;
	}

	bool pak_file::get_file_data(const entry& a_entry, u8* a_buffer, size_t a_size) const
	{
		size_t data_size = get_file_size(a_entry);
		if (a_size < data_size)
			return false;

		if (a_entry.header.compression_method_index == 0)
		{
			size_t offset = a_entry.header.offset + a_entry.header_size;

			if (has_flag<entry::flag_type::encrypted>(a_entry.flags) == false)
			{
				read_array(a_buffer, a_entry.header.uncompressed_size, offset);
				return true;
			}

			read_array(a_buffer, data_size, offset);
			for (auto& key : pak_filesystem::keys)
			{
				debug_break(); // TODO: Validate and test
				key->decrypt(a_buffer, data_size);
				return true;
			}
		}

		debug_break(); // TODO: implement
		return false;
	}

	std::filesystem::path pak_file::get_mount_point() const
	{
		return m_mount_point;
	}

	pak_file::pak_file() {}
	pak_file::~pak_file() {}
}