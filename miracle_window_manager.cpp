#include "miracle_window_manager.h"

MiracleWindowManager::MiracleWindowManager(miral::WindowManagerTools const& tools) :
    miral::FloatingWindowManager{tools}
{
}

MiracleWindowManager::~MiracleWindowManager() = default;
