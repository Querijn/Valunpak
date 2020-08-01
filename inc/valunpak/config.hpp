#pragma once

#include <debugbreak.h>

namespace valunpak
{
	using i8 = signed char;
	using i16 = signed short;
	using i32 = signed int;
	using i64 = long long;

	using u8 = unsigned char;
	using u16 = unsigned short;
	using u32 = unsigned int;
	using u64 = unsigned long long;

	static_assert(sizeof(i8) == 1, "i8 seems to not match the correct size!");
	static_assert(sizeof(i16) == 2, "i16 seems to not match the correct size!");
	static_assert(sizeof(i32) == 4, "i32 seems to not match the correct size!");
	static_assert(sizeof(i64) == 8, "i64 seems to not match the correct size!");

	static_assert(sizeof(u8) == 1, "u8 seems to not match the correct size!");
	static_assert(sizeof(u16) == 2, "u16 seems to not match the correct size!");
	static_assert(sizeof(u32) == 4, "u32 seems to not match the correct size!");
	static_assert(sizeof(u64) == 8, "u64 seems to not match the correct size!");

	template<unsigned int flag, typename T>
	bool has_flag(T value)
	{
		return (value & flag) != 0;
	}

#ifdef WIN32
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#define VALUNPAK_REQUIRE_RET(exp, ret) do \
{ \
	if((exp) == false) \
	{ \
		printf("%s:%d -> Failed VALUNPAK_REQUIRE (%s)\n", __FILENAME__, __LINE__, #exp); \
		/*debug_break();*/ \
		return (ret); \
	} \
} \
while(0)

#define VALUNPAK_REQUIRE(exp) VALUNPAK_REQUIRE_RET(exp, false)
}

#pragma warning(disable: 4201) // Nameless structs are fine imo
#pragma warning(disable: 4201) // Condition is constant
#pragma warning(disable: 4369) // Issues with using hashes as enum values
