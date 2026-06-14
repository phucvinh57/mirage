#include "mirage_window_manager.h"

MirageWindowManager::MirageWindowManager(miral::WindowManagerTools const& tools) :
    miral::FloatingWindowManager{tools}
{
}

MirageWindowManager::~MirageWindowManager() = default;
