#include <valunpak/json_writer.hpp>

namespace valunpak
{
	std::string to_json(const std::map<std::string, json_variable>& a_map)
	{
		std::string result = "{";

		bool first = true;
		for (auto entry : a_map)
		{
			if (first)
				first = false;
			else
				result += ",";

			// Key
			result += to_json(entry.first);
			result += ": ";

			// Value
			result += to_json(entry.second);
		}

		return result + "}";
	}

	std::string to_json(const json_variable& a_variant)
	{
		std::string result;
		std::visit([&result](auto&& arg)
		{
			using T = std::decay_t<decltype(arg)>;
			if constexpr (std::is_same_v<T, i64>)
				result = to_json(arg);
			else if constexpr (std::is_same_v<T, double>)
				result = to_json(arg);
			else if constexpr (std::is_same_v<T, bool>)
				result = to_json(arg);
			else if constexpr (std::is_same_v<T, std::string>)
				result = to_json(arg);
			else
				static_assert(false, "We're missing a type in the json_variable visitor!");
		}, a_variant);

		return result;
	}

	std::string to_json(const std::string& a_string)
	{
		return "\"" + a_string + "\"";
	}

	std::string to_json(const i64& a_number)
	{
		return std::to_string(a_number);
	}

	std::string to_json(const double& a_double)
	{
		return std::to_string(a_double);
	}

	std::string to_json(const bool& a_bool)
	{
		return a_bool ? "true" : "false";
	}
}

