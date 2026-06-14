#ifndef __MIRAGE_WINDOW_MANAGER_H__
#define __MIRAGE_WINDOW_MANAGER_H__

#include <miral/floating_window_manager.h>
#include <miral/window_manager_tools.h>

class MirageWindowManager : public miral::FloatingWindowManager
{
public:
    explicit MirageWindowManager(miral::WindowManagerTools const& tools);
    ~MirageWindowManager() override;
};

#endif // __MIRAGE_WINDOW_MANAGER_H__
