#ifndef STAGE_MANAGER_H
#define STAGE_MANAGER_H

#include <mir/geometry/forward.h>
#include <mir/geometry/rectangle.h>
#include <mir/logging/logger.h>
#include <miral/window.h>
#include <miral/window_info.h>
#include <miral/window_specification.h>

#include <utility>
#include <vector>

struct StagePlacement
{
    miral::Window window;
    miral::WindowSpecification spec;
};

class StageManager
{
public:
    StageManager();
    ~StageManager();

    bool is_stageable(miral::WindowInfo const& window_info) const;

    void track(miral::Window const& window);
    void untrack(miral::Window const& window);
    void make_current(miral::Window const& window);

    void update_areas(mir::geometry::Rectangle const& zone);
    auto layout(mir::geometry::Rectangle const& zone) const -> std::vector<StagePlacement>;
    auto sidebar_area() const -> miral::Rectangle const&
    {
        return areas.first;
    }
    auto main_area() const -> miral::Rectangle const&
    {
        return areas.second;
    }

private:
    std::vector<miral::Window> staged_windows;
    std::pair<miral::Rectangle, miral::Rectangle> areas;
    mir::logging::Tag const& log_tag;
};

#endif // STAGE_MANAGER_H
