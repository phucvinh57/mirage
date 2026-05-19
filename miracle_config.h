#ifndef MIRACLE_CONFIG_H
#define MIRACLE_CONFIG_H

#include <filesystem>
#include <iosfwd>
#include <map>
#include <optional>
#include <string>
#include <vector>

class MiracleConfig;

class DependencyConfig
{
public:
    virtual void load(MiracleConfig*) = 0;
};

class MiracleConfig
{
public:
    using Values = std::vector<std::string>;
    using Data = std::map<std::string, Values>;

    void set(std::string key, Values values);
    void set(std::string key, std::string value);

    auto get(std::string const &key) const -> std::optional<Values>;
    auto get_one(std::string const &key) const -> std::optional<std::string>;
    auto search_prefix(std::string const &prefix) const -> std::vector<std::pair<std::string, Values>>;

    void load(std::istream &stream, std::filesystem::path const &path);
    auto add_dependency(DependencyConfig* dependency) -> MiracleConfig*;

private:
    std::vector<DependencyConfig*> dependencies;
    Data data;
};

#endif // MIRACLE_CONFIG_H
