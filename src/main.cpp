#include <valunpak/config.hpp>
#include <valunpak/pak_filesystem.hpp>
#include <valunpak/uasset_writer.hpp>
#include <valunpak/simple_writer.hpp>

#include <fstream>
#include <chrono>
namespace fs = std::filesystem;

#include <debugbreak.h>

#define PAK_FILE_DIR "C:/Riot Games/VALORANT/live"

namespace valunpak
{
	struct profiler
	{
		profiler(std::string_view a_name) : name(a_name) { t1 = std::chrono::high_resolution_clock::now(); }
		~profiler()
		{
			auto t2 = std::chrono::high_resolution_clock::now();
			auto time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);

			double count = time_span.count();
			if (count > 0.1f)
				printf("%s took %3.2f seconds\n", name.c_str(), count);
			else
				printf("%s took %3.2f milliseconds\n", name.c_str(), count * 1000);
		}

		std::string name;
		std::chrono::high_resolution_clock::time_point t1;
	};

	pak_filesystem paks;
	bool read_all_files()
	{
		profiler extract_profiler("Loading");
		paks.open(PAK_FILE_DIR);
		return paks.empty() == false;
	}

	bool get_key()
	{
		std::vector<u8> key;
		std::ifstream key_file("key.txt", std::ios::binary | std::ios::ate);
		if (key_file.good() == false)
		{
			printf("unable to open 'key.txt'!\n");
			return false;
		}

		std::streampos key_size = key_file.tellg();
		key.resize(key_size);
		key_file.seekg(0, std::ios_base::beg);
		key_file.read((char*)key.data(), key_size);

		paks.keys.add(key);
		return true;
	}

	std::vector<std::string> files_to_skip =
	{
		"ShooterGame/Content/Characters/_Core/DefaultAttributeValues", // Has a map property with text in it that does not conform to any other map.
	};

	bool test()
	{
		profiler extract_profiler("Extracting");

		bool start = false;
		for (auto& entry : paks.entries)
		{
			// Get file name.
			std::string name = entry.first.generic_string();
			auto dot_index = name.find_last_of('.');

			if (dot_index != std::string::npos)
			{
				auto ext = name.substr(dot_index + 1);
				if (ext == "uasset")
				{
					write_uasset(name.substr(0, dot_index), paks, files_to_skip);
					continue;
				}
				else if (ext.empty() == false && ext != "ubulk" && ext != "uexp")
				{
					write_default_file(name, paks, files_to_skip);
					continue;
				}
				else if (ext == "ubulk" || ext == "uexp")
				{ }
				else
				{
					debug_break();
				}
			}
			else
			{
				debug_break();
			}
		}
		
		return true;
	}
}

int main()
{
	if (valunpak::get_key() == false)
		return -1;

	if (valunpak::read_all_files() == false)
		return -2;

	return valunpak::test() ? 0 : -3;
}
