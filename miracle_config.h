#ifndef MIRACLE_CONFIG_H
#define MIRACLE_CONFIG_H

#include <filesystem>
#include <istream>
#include <map>
#include <mutex>
#include <string>
#include <vector>
#include <optional>

class MiracleConfig
{
public:
    using Values = std::vector<std::string>;
    using Data = std::map<std::string, Values>;

    void set(std::string key, Values values);
    void set(std::string key, std::string value);

    auto get(std::string const &key) const -> std::optional<Values>;
    auto get_one(std::string const &key) const -> std::optional<std::string>;

    void load(std::istream &stream, std::filesystem::path const &path);

private:
    mutable std::mutex mutex;
    Data data;
};

#endif // MIRACLE_CONFIG_H
