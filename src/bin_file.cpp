#include <valunpak/bin_file.hpp>

#include <filesystem>
#include <fstream>
#include <cstring>
#include <unordered_map>
#include <thread>

#include <debugbreak.h>

namespace fs = std::filesystem;

namespace valunpak
{
	struct base_file_impl
	{
		virtual bool get_data(u8* a_buffer, size_t a_size, size_t& a_offset) = 0;

		virtual size_t get_size() const = 0;
		virtual size_t get_debug_offset(size_t offset) const = 0;

		bool find_buffer(const u8* a_buffer, size_t a_size, size_t& a_offset)
		{
			// We're going over the whole thing, 1024 elements at a time.
			size_t buffer_size = a_size * 1024;
			std::vector<u8> buffer(buffer_size);
			size_t search = a_offset;
			size_t size = get_size();

			// Keep searching as long as we just searched 1024 items. If it's less, we're at the end.
			size_t search_count = 0;
			do
			{
				size_t room_left = size - search; // Basically bytes until the end of the file.
				size_t search_size = buffer_size < room_left ? buffer_size : room_left; // size in bytes
				VALUNPAK_REQUIRE(get_data(buffer.data(), search_size, search));

				search_count = buffer_size >= room_left ? search_size / a_size : 1024; // Convert back to element count (should be 1024)
				const u8* ptr = buffer.data();
				const u8* end = ptr + buffer_size;
				for (; ptr <= end; ptr += a_size) // And search
				{
					if (memcmp(a_buffer, ptr, a_size) == 0)
					{
						a_offset = search;
						return true;
					}
				}
			} while (search_count == 1024);

			// TODO: Should really verify coverage
			return memcmp(a_buffer, buffer.data() + buffer_size - a_size, a_size) == 0; // Make sure we got the last bytes too
		}
	};

	struct bin_file_impl : base_file_impl
	{
		bin_file_impl(std::string_view a_file_name)
		{
			stream.open(a_file_name.data(), std::ios::binary | std::ios::ate);
			size = stream.tellg();
		}

		bool get_data(u8* a_buffer, size_t a_size, size_t& a_offset) override
		{
			stream.seekg(a_offset, std::ios_base::beg);
			stream.read((char*)a_buffer, a_size);

			size_t read_count = stream.gcount();
			a_offset += read_count;

			return read_count == a_size;
		}

		size_t get_size() const override
		{
			return size;
		}

		size_t get_debug_offset(size_t offset) const override
		{
			return offset;
		}

		std::ifstream stream;
		size_t size;
	};

	struct virtual_file_impl : base_file_impl
	{
		virtual_file_impl(std::string_view a_file_name)
		{
			std::ifstream stream(a_file_name.data(), std::ios::binary | std::ios::ate);
			size = stream.tellg();
			stream.seekg(0, std::ios::beg);
			data.resize(size);
			stream.read((char*)data.data(), size);
		}

		virtual_file_impl(const std::vector<u8>& a_data) :
			data(a_data), size(a_data.size())
		{
		}

		virtual_file_impl(const u8* a_data, size_t a_size) :
			data(a_data, a_data + a_size), size(a_size)
		{
		}

		bool get_data(u8* a_buffer, size_t a_size, size_t& a_offset) override
		{
			VALUNPAK_REQUIRE(a_buffer);
			u8* source = data.data() + a_offset;

			size_t read_end = a_offset + a_size;
			size_t actual_end = get_size();
			VALUNPAK_REQUIRE(read_end <= actual_end);

			u8* result = (u8*)memcpy(a_buffer, source, a_size);

			a_offset += a_size;
			return &a_buffer != &result;
		}

		size_t get_size() const override
		{
			return size;
		}

		size_t get_debug_offset(size_t offset) const override
		{
			return offset;
		}

		std::vector<u8> data;
		size_t size;
	};

	struct sub_file_impl : base_file_impl
	{
		sub_file_impl(std::shared_ptr<base_file_impl>& a_parent, size_t a_offset) :
			parent(a_parent),
			base_offset(a_offset)
		{
			if (a_offset > parent->get_size())
				a_offset = parent->get_size();
		}

		bool get_data(u8* a_buffer, size_t a_size, size_t& a_offset) override
		{
			size_t offset = base_offset + a_offset;
			VALUNPAK_REQUIRE(parent->get_data(a_buffer, a_size, offset));
			a_offset += a_size;
			return true;
		}

		size_t get_size() const override
		{
			return parent->get_size() - base_offset;
		}

		size_t get_debug_offset(size_t offset) const override
		{
			return base_offset + parent->get_debug_offset(offset);
		}

		std::shared_ptr<base_file_impl> parent;
		size_t base_offset;
	};

	bool bin_file::open(std::string_view a_file_name, read_mode_type a_read_mode) noexcept
	{
		VALUNPAK_REQUIRE(fs::exists(a_file_name));

		m_file_name = a_file_name;
		if (a_read_mode == read_mode_type::stream)
			m_impl = std::make_shared<bin_file_impl>(a_file_name); // TODO: Support multithreading
		else
			m_impl = std::make_shared<virtual_file_impl>(a_file_name);
		return true;
	}

	bool bin_file::open(const std::vector<u8>& a_data) noexcept
	{
		m_file_name = "mem0";
		m_impl = std::make_shared<virtual_file_impl>(a_data);

		return true;
	}

	bool bin_file::open(const u8* a_data, size_t a_size) noexcept
	{
		m_file_name = "mem0";
		m_impl = std::make_shared<virtual_file_impl>(a_data, a_size);

		return true;
	}

	bool bin_file::open(bin_file& a_reader, size_t& a_offset) noexcept
	{
		m_file_name = a_reader.m_file_name;
		m_impl = std::make_shared<sub_file_impl>(a_reader.m_impl, a_offset);
		return true;
	}

	bool bin_file::to_file(const char* a_file_name) const noexcept
	{
		size_t size = m_impl->get_size();
		std::vector<u8> bytes(size);
		size_t offset = 0;
		VALUNPAK_REQUIRE(m_impl->get_data(bytes.data(), size, offset));
		return to_file_internal(a_file_name, bytes.data(), offset);
	}
	
	bool bin_file::read_buffer(char* a_buffer, size_t a_length, size_t& a_offset) const
	{
		return m_impl->get_data((u8*)a_buffer, a_length, a_offset);
	}

	size_t bin_file::get_size() const
	{
		return m_impl->get_size();
	}

	size_t bin_file::get_debug_offset(size_t a_offset) const
	{
		return m_impl->get_debug_offset(a_offset);
	}

	bool bin_file::find_buffer(const u8* a_buffer, size_t a_size, size_t& a_offset) const
	{
		return m_impl->find_buffer(a_buffer, a_size, a_offset);
	}

	bool bin_file::to_file_internal(const char* a_file_name, const u8* a_buffer, size_t a_size) const noexcept
	{
		std::ofstream output_file(a_file_name, std::ofstream::binary);
		output_file.write((char*)a_buffer, a_size);
		return output_file.good();
	}

	bool bin_file::read_string(std::string& a_string, size_t a_size, size_t& a_offset) const
	{
		a_string.resize(a_size);
		return read_buffer(a_string.data(), a_size, a_offset);
	}

	// Need to be explicit due to the fact that the unique_ptr has to work with the bin_file_impl forward declaration
	bin_file::bin_file() {}
	bin_file::~bin_file() {}
}

