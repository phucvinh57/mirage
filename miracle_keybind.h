#ifndef __MIRACLE_KEYBIND_H__
#define __MIRACLE_KEYBIND_H__

#include <optional>
#include <string>
#include <vector>

#include <miral/toolkit_event.h>
#include <xkbcommon/xkbcommon.h>

#include "miracle_config.h"

namespace miral
{
    class ExternalClientLauncher;
    class MirRunner;
}

struct Keybind
{
    MirInputEventModifiers modifiers{0};
    xkb_keysym_t key{XKB_KEY_NoSymbol};
    std::string action;
};

class KeybindConfig : public DependencyConfig
{
private:
    std::vector<Keybind> keybinds;
    auto action_for(MirKeyboardEvent const *event) const -> std::optional<std::string>;

public:
    void load(MiracleConfig *config) override;
    auto handle(
        MirKeyboardEvent const *event,
        miral::MirRunner &runner,
        miral::ExternalClientLauncher &launcher) const -> bool;
};

#endif // __MIRACLE_KEYBIND_H__
