#include <miral/runner.h>
#include <miral/append_keyboard_event_filter.h>
#include <miral/config_file.h>
#include <miral/configuration_option.h>
#include <miral/external_client.h>
#include <miral/minimal_window_manager.h>
#include <miral/set_window_management_policy.h>
#include <miral/toolkit_event.h>
#include <miral/decorations.h>
#include <mir/log.h>

#include "miracle_config.h"

#include <cstdlib>

using namespace miral;
using namespace miral::toolkit;

int main(int argc, char const *argv[])
{
    ExternalClientLauncher launcher;
    MirRunner runner{argc, argv, "miracle.config"};
    MiracleConfig config;

    // Load configuration from file, and reload on changes.
    ConfigFile{
        runner,
        runner.config_file(),
        ConfigFile::Mode::reload_on_change,
        [&config](std::istream &stream, std::filesystem::path const &path)
        {
            config.load(stream, path);
        }};

    auto const keybinds = [&launcher, &runner, &config](MirKeyboardEvent const *ev)
    {
        // Skip anything but down presses
        if (mir_keyboard_event_action(ev) != mir_keyboard_action_down)
            return false;

        // Ctrl+Alt must be pressed
        MirInputEventModifiers mods = mir_keyboard_event_modifiers(ev);
        if (!(mods & mir_input_event_modifier_alt) || !(mods & mir_input_event_modifier_ctrl))
            return false;

        switch (mir_keyboard_event_keysym(ev))
        {
        case XKB_KEY_BackSpace:
            runner.stop();
            return true;
        case XKB_KEY_t:
        case XKB_KEY_T:
            launcher.launch(config.get_one("terminal").value_or("x-terminal-emulator"));
            return true;
        default:
            return false;
            break;
        }
    };

    auto run_startup_apps = [&launcher, &config](std::vector<std::string> const &commands)
    {
        auto startup_apps = config.get("startup-app");
        if (!startup_apps.has_value())
            return;
        for (auto const &command : startup_apps.value())
            launcher.launch(command);
    };

    return runner.run_with({set_window_management_policy<MinimalWindowManager>(),
                            launcher,
                            miral::AppendKeyboardEventFilter(keybinds),
                            ConfigurationOption{run_startup_apps, "startup-app", "Commands to run on startup (may be specified multiple times)"},
                            miral::Decorations::always_csd()});
}
