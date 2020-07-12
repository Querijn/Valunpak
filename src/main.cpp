#include "valunpak/config.hpp"
#include "valunpak/pak_filesystem.hpp"
#include "valunpak/ue4_uasset.hpp"
#include "valunpak/ue4_uexp.hpp"

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

	bool test()
	{
		std::string name = "ShooterGame/Content/UI/InGame/KillBanner/Base/Assets/Base_Emblem";
		auto uasset_pak_entry = paks.get_file(name + ".uasset");
		VALUNPAK_REQUIRE(uasset_pak_entry.first);

		ue4_uasset uasset_bin;
		VALUNPAK_REQUIRE(uasset_pak_entry.first->get_file_data(*uasset_pak_entry.second, &uasset_bin));

		// Ubulk is optional
		auto ubulk_pak_entry = paks.get_file(name + ".ubulk");
		ue4_bin_file ubulk_bin;
		if (ubulk_pak_entry.first != nullptr)
			VALUNPAK_REQUIRE(ubulk_pak_entry.first->get_file_data(*ubulk_pak_entry.second, &ubulk_bin));

		auto uexp_pak_entry = paks.get_file(name + ".uexp");
		VALUNPAK_REQUIRE(uexp_pak_entry.first)

		ue4_uexp uexp_bin(uasset_bin, ubulk_pak_entry.first ? &ubulk_bin : nullptr);
		VALUNPAK_REQUIRE(uexp_pak_entry.first->get_file_data(*uexp_pak_entry.second, &uexp_bin))

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
