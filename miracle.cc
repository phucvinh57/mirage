#include <miral/runner.h>
#include <miral/append_keyboard_event_filter.h>
#include <miral/config_file.h>
#include <miral/configuration_option.h>
#include <miral/external_client.h>
#include <miral/set_window_management_policy.h>
#include <miral/toolkit_event.h>
#include <miral/decorations.h>
#include <cstdlib>
#include <mir/log.h>
#include <miral/x11_support.h>

#include "miracle_config.h"
#include "miracle_keybind.h"
#include "miracle_window_manager.h"

using namespace miral;
using namespace miral::toolkit;

int main(int argc, char const *argv[])
{
    ExternalClientLauncher launcher;
    MirRunner runner{argc, argv, "miracle.config"};
    MiracleConfig config;
    KeybindConfig keybind_config;
    config.add_dependency(&keybind_config);

    // Load configuration from file, and reload on changes.
    ConfigFile{
        runner,
        runner.config_file(),
        ConfigFile::Mode::reload_on_change,
        [&config](std::istream &stream, std::filesystem::path const &path)
        {
            config.load(stream, path);
        }};

    auto const keybinds = [&launcher, &runner, &keybind_config](MirKeyboardEvent const *ev)
    {
        return keybind_config.handle(ev, runner, launcher);
    };

    auto run_startup_apps = [&launcher, &config]()
    {
        auto apps = config.get("startup-app");
        if (!apps)
            return;
        for (auto const &command : apps.value())
            launcher.launch(command);
    };
    runner.add_start_callback(run_startup_apps);

    return runner.run_with({X11Support{},
                            set_window_management_policy<MiracleWindowManager>(),
                            launcher,
                            miral::AppendKeyboardEventFilter(keybinds),
                            miral::Decorations::prefer_csd()});
}
