#include "miracle_config.h"

#include <mir/log.h>

#include <cstddef>
#include <string_view>
#include <utility>

namespace
{
auto trim(std::string_view value) -> std::string_view
{
    auto const first = value.find_first_not_of(" \t\r\n");
    if (first == std::string_view::npos)
        return {};

    auto const last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1);
}
}

void MiracleConfig::set(std::string key, Values values)
{
    std::lock_guard lock{mutex};
    data[std::move(key)] = std::move(values);
}

void MiracleConfig::set(std::string key, std::string value)
{
    Values values;
    if (!value.empty())
        values.push_back(std::move(value));

    set(std::move(key), std::move(values));
}

auto MiracleConfig::get(std::string const& key) const -> std::optional<Values>
{
    std::lock_guard lock{mutex};

    if (auto const it = data.find(key); it != data.end())
        return it->second;

    return std::nullopt;
}


auto MiracleConfig::get_one(std::string const& key) const -> std::optional<std::string>
{
    auto values = get(key);
    if (values.has_value() && !values.value().empty())
        return values.value().front();

    return std::nullopt;
}

void MiracleConfig::load(std::istream& stream, std::filesystem::path const& path)
{
    Data next;
    std::string line;
    std::size_t line_number = 0;

    while (std::getline(stream, line))
    {
        ++line_number;

        auto const trimmed = trim(line);
        if (trimmed.empty() || trimmed.starts_with('#') || trimmed.starts_with(';'))
            continue;

        auto const separator = trimmed.find('=');
        if (separator == std::string_view::npos)
        {
            mir::log_warning(
                "Ignoring invalid config line %zu in %s: missing '='",
                line_number,
                path.c_str());
            continue;
        }

        auto const key = trim(trimmed.substr(0, separator));
        if (key.empty())
        {
            mir::log_warning(
                "Ignoring invalid config line %zu in %s: empty key",
                line_number,
                path.c_str());
            continue;
        }

        auto const value = trim(trimmed.substr(separator + 1));
        auto& values = next[std::string{key}];

        if (!value.empty())
            values.emplace_back(value);
    }

    std::lock_guard lock{mutex};
    data = std::move(next);
}
