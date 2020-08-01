#pragma once

#include <valunpak/config.hpp>
#include <valunpak/ue4_base.hpp>
#include <valunpak/ue4_uobject.hpp>
#include <valunpak/ue4_bulkdata.hpp>

#include <map>
#include <string>

namespace valunpak
{
	class ue4_uexp;
	class ue4_ubulk;
	class ue4_ucurvetable : public ue4_base
	{
	public:
		bool open(ue4_uexp& a_uexp, ue4_ubulk* a_ubulk, size_t& a_offset) noexcept;

	private:
		ue4_uobject m_object;

		std::string name;
		std::map<std::string, std::map<std::string, std::string>> m_entries;

		void reset();
		bool read_internal(ue4_uexp& a_uexp, ue4_ubulk* a_bulk, size_t& a_offset);
	};
}
