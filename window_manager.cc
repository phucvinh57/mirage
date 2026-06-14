#include "window_manager.h"

#include <miral/window_specification.h>

WM::WM(miral::WindowManagerTools const& tools, StageManager& stage_manager)
    : miral::FloatingWindowManager{tools}, stage_manager{stage_manager}
{
}

WM::~WM() = default;

auto WM::place_new_window(miral::ApplicationInfo const& app_info,
                          miral::WindowSpecification const& request) -> miral::WindowSpecification
{
    // Guideline:
    // Start from FloatingWindowManager::place_new_window() so default Miral
    // placement still works, then adjust the returned specification if the
    // initial geometry should be staged before the window appears.
    auto spec = miral::FloatingWindowManager::place_new_window(app_info, request);

    if (!spec.top_left() || !spec.size())
        return spec;

    auto const window_geometry =
        mir::geometry::Rectangle{spec.top_left().value(), spec.size().value()};
    auto const& main_area = stage_manager.main_area();

    if (!main_area.contains(window_geometry)) {
        spec.top_left() = main_area.top_left;
        spec.size() = main_area.size;
        spec.state() = mir_window_state_restored;
    }

    return spec;
}

void WM::advise_end()
{
    if (relayout_pending) {
        relayout_pending = false;
        relayout();
    }
}

void WM::advise_new_window(miral::WindowInfo const& window_info)
{
    miral::FloatingWindowManager::advise_new_window(window_info);

    if (!stage_manager.is_stageable(window_info))
        return;

    stage_manager.track(window_info.window());
    stage_manager.make_current(window_info.window());
    request_relayout();
}

void WM::advise_delete_window(miral::WindowInfo const& window_info)
{
    miral::FloatingWindowManager::advise_delete_window(window_info);

    stage_manager.untrack(window_info.window());
    request_relayout();
}

void WM::advise_focus_gained(miral::WindowInfo const& window_info)
{
    miral::FloatingWindowManager::advise_focus_gained(window_info);

    if (!stage_manager.is_stageable(window_info))
        return;

    stage_manager.track(window_info.window());
    stage_manager.make_current(window_info.window());
    request_relayout();
}

void WM::handle_raise_window(miral::WindowInfo& window_info)
{
    miral::FloatingWindowManager::handle_raise_window(window_info);

    if (!stage_manager.is_stageable(window_info))
        return;

    stage_manager.track(window_info.window());
    stage_manager.make_current(window_info.window());
    request_relayout();
}

void WM::advise_application_zone_create(miral::Zone const& application_zone)
{
    miral::FloatingWindowManager::advise_application_zone_create(application_zone);
    stage_manager.update_areas(application_zone.extents());
}

void WM::advise_application_zone_update(miral::Zone const& updated, miral::Zone const& original)
{
    miral::FloatingWindowManager::advise_application_zone_update(updated, original);
    stage_manager.update_areas(updated.extents());
    request_relayout();
}

void WM::request_relayout()
{
    relayout_pending = true;
}

void WM::relayout()
{
    for (auto const& placement : stage_manager.layout(tools.active_application_zone().extents()))
        tools.modify_window(placement.window, placement.spec);
}
