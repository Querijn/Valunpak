#include <valunpak/bin_file.hpp>

#include <filesystem>
#include <fstream>
#include <unordered_map>
#include <thread>

namespace fs = std::filesystem;

namespace valunpak
{
	struct base_file_impl
	{
		virtual bool get_data(u8* a_buffer, size_t a_size, size_t& a_offset) = 0;
		virtual size_t get_size() const = 0;
	};

	struct bin_file_impl : base_file_impl
	{
		bin_file_impl(std::string_view a_file_name)
		{
			stream.open(a_file_name, std::ios::binary | std::ios::ate);
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

		std::ifstream stream;
		size_t size;
	};

	struct virtual_file_impl : base_file_impl
	{
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
			u8* source = data.data() + a_offset;
			u8* result = (u8*)memcpy(a_buffer, source, a_size);

			a_offset += a_size;
			if (a_offset > get_size())
				a_offset = a_size;

			return &a_buffer != &result;
		}

		size_t get_size() const override
		{
			return size;
		}

		std::vector<u8> data;
		size_t size;
	};

	bool bin_file::open(std::string_view a_file_name) noexcept
	{
		if (!fs::exists(a_file_name))
			return false;

		m_file_name = a_file_name;
		m_impl = std::make_unique<bin_file_impl>(a_file_name); // TODO: Support multithreading
		return true;
	}

	bool bin_file::open(const std::vector<u8>& a_data) noexcept
	{
		m_file_name = "mem0";
		m_impl = std::make_unique<virtual_file_impl>(a_data);

		return true;
	}

	bool bin_file::open(const u8* a_data, size_t a_size) noexcept
	{
		m_file_name = "mem0";
		m_impl = std::make_unique<virtual_file_impl>(a_data, a_size);

		return true;
	}
	
	bool bin_file::read_buffer(char* a_buffer, size_t a_length, size_t& a_offset)
	{
		return m_impl->get_data((u8*)a_buffer, a_length, a_offset);
	}

	size_t bin_file::get_size() const
	{
		return m_impl->get_size();
	}

	bool bin_file::read_string(std::string& a_string, size_t a_size, size_t& a_offset)
	{
		a_string.resize(a_size);
		return read_buffer(a_string.data(), a_size, a_offset);
	}

	// Need to be explicit due to the fact that the unique_ptr has to work with the bin_file_impl forward declaration
	bin_file::bin_file() {}
	bin_file::~bin_file() {}
}

