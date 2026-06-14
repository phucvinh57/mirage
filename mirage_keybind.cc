#include "mirage_keybind.h"

#include <mir/log.h>
#include <miral/external_client.h>
#include <miral/runner.h>

#include <sstream>
#include <string_view>

namespace
{
    constexpr MirInputEventModifiers whitelist_modifiers =
        mir_input_event_modifier_alt |
        mir_input_event_modifier_shift |
        mir_input_event_modifier_ctrl |
        mir_input_event_modifier_meta;

    auto to_modifiers(std::string const &suffix) -> std::optional<MirInputEventModifiers>
    {
        if (suffix.empty())
            return std::nullopt;

        MirInputEventModifiers modifiers = 0;
        std::string token;
        std::istringstream tokens{std::string{suffix}};

        while (std::getline(tokens, token, '_'))
        {
            if (token.empty())
                return std::nullopt;

            if (token == "plain")
            {
                if (suffix != "plain")
                    return std::nullopt;

                return 0;
            }

            if (token == "alt")
            {
                modifiers |= mir_input_event_modifier_alt;
                continue;
            }

            if (token == "ctrl" || token == "control")
            {
                modifiers |= mir_input_event_modifier_ctrl;
                continue;
            }

            if (token == "shift")
            {
                modifiers |= mir_input_event_modifier_shift;
                continue;
            }

            if (token == "meta" || token == "super" || token == "command")
            {
                modifiers |= mir_input_event_modifier_meta;
                continue;
            }

            return std::nullopt;
        }

        return modifiers;
    }

    auto to_key(std::string const &name) -> std::optional<xkb_keysym_t>
    {
        if (auto const key = xkb_keysym_from_name(name.c_str(), XKB_KEYSYM_CASE_INSENSITIVE);
            key != XKB_KEY_NoSymbol)
        {
            return xkb_keysym_to_lower(key);
        }

        return std::nullopt;
    }

    auto matches(Keybind const &binding, MirKeyboardEvent const *event) -> bool
    {
        auto const event_key = xkb_keysym_to_lower(miral::toolkit::mir_keyboard_event_keysym(event));
        if (event_key != binding.key)
            return false;

        auto const event_modifiers = miral::toolkit::mir_keyboard_event_modifiers(event) & whitelist_modifiers;
        auto const allows_shift_fallback =
            binding.modifiers != 0 &&
            !(binding.modifiers & mir_input_event_modifier_shift);

        if (allows_shift_fallback)
            return (event_modifiers & ~mir_input_event_modifier_shift) == binding.modifiers;

        return event_modifiers == binding.modifiers;
    }
}

void KeybindConfig::load(MirageConfig *config)
{
    this->keybinds.clear();
    for (auto const &[name, values] : config->search_prefix("command_"))
    {
        auto const modifiers = to_modifiers(name.substr(std::string{"command_"}.size()));
        if (!modifiers)
        {
            mir::log_warning("Ignoring invalid keybind group: %s", name.c_str());
            continue;
        }

        for (auto const &value : values)
        {
            auto const separator = value.find(':');
            if (separator == std::string::npos || separator == 0 || separator + 1 >= value.size())
            {
                mir::log_warning("Ignoring invalid keybind entry for %s: %s", name.c_str(), value.c_str());
                continue;
            }

            auto const key = to_key(value.substr(0, separator));
            if (!key)
            {
                mir::log_warning("Ignoring keybind with unrecognised key for %s: %s", name.c_str(), value.c_str());
                continue;
            }

            this->keybinds.push_back(Keybind{
                .modifiers = *modifiers,
                .key = *key,
                .action = value.substr(separator + 1),
            });
        }
    }

    auto const loaded_keybind_count = this->keybinds.size();
    mir::log_info("Loaded %zu keybinds", loaded_keybind_count);
}

auto KeybindConfig::action_for(MirKeyboardEvent const *event) const -> std::optional<std::string>
{
    for (auto const &binding : keybinds)
    {
        if (matches(binding, event))
            return binding.action;
    }

    return std::nullopt;
}

auto KeybindConfig::handle(
    MirKeyboardEvent const *event,
    miral::MirRunner &runner,
    miral::ExternalClientLauncher &launcher) const -> bool
{
    if (miral::toolkit::mir_keyboard_event_action(event) != mir_keyboard_action_down)
        return false;

    auto action = action_for(event);
    if (!action)
        return false;

    auto val = action.value();
    if (val.empty())
        return false;

    if (val.starts_with('@'))
    {
        if (val == "@exit")
        {
            runner.stop();
            return true;
        }

        mir::log_warning("Ignoring unknown internal keybind action: %s", val.c_str());
        return false;
    }

    launcher.launch(val);
    return true;
}
