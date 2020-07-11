#pragma once

#include <valunpak/config.hpp>

#include <variant>
#include <string>
#include <map>

namespace valunpak
{
	using json_variable = std::variant<std::string, i64, double, bool>;
	std::string to_json(const std::map<std::string, json_variable>& a_map);

	std::string to_json(const json_variable& a_variant);
	std::string to_json(const std::string& a_string);
	std::string to_json(const i64& a_number);
	std::string to_json(const double& a_double);
	std::string to_json(const bool& a_string);
}
