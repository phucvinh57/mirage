#ifndef __MIRAGE_KEYBIND_H__
#define __MIRAGE_KEYBIND_H__

#include <optional>
#include <string>
#include <vector>

#include <miral/toolkit_event.h>
#include <xkbcommon/xkbcommon.h>

#include "mirage_config.h"

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
    void load(MirageConfig *config) override;
    auto handle(
        MirKeyboardEvent const *event,
        miral::MirRunner &runner,
        miral::ExternalClientLauncher &launcher) const -> bool;
};

#endif // __MIRAGE_KEYBIND_H__
