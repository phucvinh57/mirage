#ifndef __MIRACLE_WINDOW_MANAGER_H__
#define __MIRACLE_WINDOW_MANAGER_H__

#include <miral/floating_window_manager.h>
#include <miral/window_manager_tools.h>

class MiracleWindowManager : public miral::FloatingWindowManager
{
public:
    explicit MiracleWindowManager(miral::WindowManagerTools const& tools);
    ~MiracleWindowManager() override;
};

#endif // __MIRACLE_WINDOW_MANAGER_H__
