#include "valunpak/config.hpp"
#include "valunpak/pak_file.hpp"

#include <filesystem>
#include <fstream>
#include <chrono>
namespace fs = std::filesystem;

#include <debugbreak.h>

#define PAK_FILE_DIR "C:/Riot Games/VALORANT/live"

namespace valunpak
{
	struct profiler
	{
		profiler(std::string_view a_name): name(a_name) { t1 = std::chrono::high_resolution_clock::now(); }
		~profiler() 
		{ 
			auto t2 = std::chrono::high_resolution_clock::now();
			auto time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);

			printf("%s took %3.2f seconds\n", name.c_str(), time_span.count());
		}

		std::string name;
		std::chrono::high_resolution_clock::time_point t1;
	};

	std::vector<std::shared_ptr<pak_file>> files;
	int read_all_files(const std::vector<u8>& key, bin_file::read_mode_type a_read_mode)
	{
		profiler extract_profiler("Loading");
		valunpak::pak_file pak;
		for (auto& dir_entry : fs::recursive_directory_iterator(PAK_FILE_DIR))
		{
			auto path = dir_entry.path();
			if (path.has_extension() == false || path.extension() != ".pak")
				continue;

			std::shared_ptr<pak_file> pak = std::make_shared<pak_file>();
			if (pak->open(path.generic_string(), key, a_read_mode) == false)
				continue;
			files.push_back(pak);

			auto mountpoint = pak->get_mount_point();
			for (auto& entry_pair : *pak)
			{
				// The full filename
				auto filename = (mountpoint / entry_pair.first).generic_string();

				pak_file::entry& entry = *(entry_pair.second);
				size_t size = pak->get_file_size(entry);
			}
		}
	}

	bool get_key(std::vector<u8>& a_key)
	{
		std::ifstream key_file("key.txt", std::ios::binary | std::ios::ate);
		if (key_file.good() == false)
		{
			printf("unable to open 'key.txt'!\n");
			return false;
		}

		std::streampos key_size = key_file.tellg();
		a_key.resize(key_size);
		key_file.seekg(0, std::ios_base::beg);
		key_file.read((char*)a_key.data(), key_size);
		return true;
	}
}

int main() 
{
	std::vector<valunpak::u8> key;
	if (valunpak::get_key(key) == false)
		return -1;

	return valunpak::read_all_files(key, valunpak::bin_file::read_mode_type::stream) ? 0 : -2;
}
