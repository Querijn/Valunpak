#pragma once

#include <valunpak/config.hpp>

namespace valunpak
{
	// TODO: Remove these from the header.
	constexpr size_t string_length(const char* a_string)
	{
		for (auto c = a_string; true; c++)
			if (*c == 0)
				return c - a_string;
		return -1;
	}

	constexpr char to_lower_case(char a_char)
	{
		return (a_char >= 'A' && a_char <= 'Z') ? a_char + ('a' - 'A') : a_char;
	}

	constexpr u32 fnv(const char* a_string)
	{
		size_t hash = 0x811c9dc5;
		for (size_t i = 0, str_length = string_length(a_string); i < str_length; i++)
			hash = ((hash ^ to_lower_case(a_string[i])) * 0x01000193) % 0x100000000;

		return (u32)hash;
	}
}
