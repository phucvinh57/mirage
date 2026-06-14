#include "stage_manager.h"

#include <mir/log.h>

#include <algorithm>

StageManager::StageManager()
    : log_tag{mir::logging::create_tag(mir::logging::window_management(), "StageManager")}
{
}

StageManager::~StageManager() = default;

bool StageManager::is_stageable(miral::WindowInfo const& window_info) const
{
    if (!window_info.window())
        return false;

    if (window_info.parent())
        return false;

    if (!window_info.can_be_active())
        return false;

    if (window_info.depth_layer() != mir_depth_layer_application)
        return false;

    switch (window_info.type()) {
    case mir_window_type_normal:
    case mir_window_type_freestyle:
        break;

    default:
        return false;
    }

    switch (window_info.state()) {
    case mir_window_state_restored:
    case mir_window_state_unknown:
        return true;

    default:
        return false;
    }
}

void StageManager::track(miral::Window const& window)
{
    if (!window) {
        mir::log_info({log_tag}, "Track ignored null window; stack_size={}", staged_windows.size());
        return;
    }

    auto const top_left = window.top_left();
    auto const size = window.size();
    auto const app = window.application();
    auto const app_name = miral::name_of(app);
    auto const app_pid = miral::pid_of(app);

    if (std::find(staged_windows.begin(), staged_windows.end(), window) != staged_windows.end()) {
        mir::log_info({log_tag},
                      "Track skipped already tracked window; app='{}' pid={} "
                      "geometry={}x{}+{}+{} stack_size={}",
                      app_name, app_pid, size.width.as_int(), size.height.as_int(),
                      top_left.x.as_int(), top_left.y.as_int(), staged_windows.size());
        return;
    }

    staged_windows.push_back(window);
    mir::log_info({log_tag},
                  "Track added window; app='{}' pid={} geometry={}x{}+{}+{} "
                  "stack_size={}",
                  app_name, app_pid, size.width.as_int(), size.height.as_int(), top_left.x.as_int(),
                  top_left.y.as_int(), staged_windows.size());
}

void StageManager::untrack(miral::Window const& window)
{
    auto it = std::find(staged_windows.begin(), staged_windows.end(), window);
    if (it != staged_windows.end())
        staged_windows.erase(it);
}

void StageManager::make_current(miral::Window const& window)
{
    auto const it = std::find(staged_windows.begin(), staged_windows.end(), window);
    if (it == staged_windows.end() || it == staged_windows.begin())
        return;

    auto current = *it;
    staged_windows.erase(it);
    staged_windows.insert(staged_windows.begin(), current);
}

void StageManager::update_areas(mir::geometry::Rectangle const& zone)
{
    auto const x = zone.top_left.x.as_int();
    auto const y = zone.top_left.y.as_int();
    auto const width = zone.size.width.as_int();
    auto const height = zone.size.height.as_int();

    auto const sidebar_width = width > 0 ? width / 5 : 0;
    auto const main_width = width > sidebar_width ? width - sidebar_width : 0;

    areas.first = {{x, y}, {sidebar_width, height}};
    areas.second = {{x + sidebar_width, y}, {main_width, height}};
}

auto StageManager::layout(mir::geometry::Rectangle const& zone) const -> std::vector<StagePlacement>
{
    std::vector<StagePlacement> placements;
    if (staged_windows.empty())
        return placements;

    auto const x = zone.top_left.x.as_int();
    auto const y = zone.top_left.y.as_int();
    auto const width = zone.size.width.as_int();
    auto const height = zone.size.height.as_int();

    auto const gap = width >= 800 ? 8 : 0;
    auto const sidebar_width = staged_windows.size() > 1 && width > gap ? width / 5 : 0;
    auto const main_width = width - sidebar_width - (sidebar_width > 0 ? gap : 0);
    auto const sidebar_x = x;
    auto const main_x = x + sidebar_width + (sidebar_width > 0 ? gap : 0);

    miral::WindowSpecification main_spec;
    main_spec.state() = mir_window_state_restored;
    main_spec.top_left() = {main_x, y};
    main_spec.size() = {main_width, height};
    placements.push_back({staged_windows.front(), main_spec});

    auto const sidebar_count = static_cast<int>(staged_windows.size() - 1);
    if (sidebar_count == 0 || sidebar_width <= 0)
        return placements;

    auto const item_gap = 8;
    auto const total_gap = item_gap * (sidebar_count - 1);
    auto const item_height = std::max(1, (height - total_gap) / sidebar_count);

    for (auto i = 0; i != sidebar_count; ++i) {
        miral::WindowSpecification sidebar_spec;
        sidebar_spec.state() = mir_window_state_restored;
        sidebar_spec.top_left() = {sidebar_x, y + i * (item_height + item_gap)};
        sidebar_spec.size() = {sidebar_width, item_height};
        placements.push_back({staged_windows[static_cast<std::size_t>(i + 1)], sidebar_spec});
    }

    return placements;
}
