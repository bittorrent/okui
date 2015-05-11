#include "bittorrent/ui/Platform.h"

#if BT_MAC_OS_X
#import <AppKit/NSOpenPanel.h>
#endif

namespace bittorrent {
namespace ui {

void Platform::async(std::function<void()> task) {
    std::lock_guard<std::mutex> lock(_asyncMutex);
    _asyncTasks.emplace_back(task);
}

void Platform::work() {
    std::vector<std::function<void()>> asyncTasks;
    {
        std::lock_guard<std::mutex> lock(_asyncMutex);
        asyncTasks.swap(_asyncTasks);
    }

    for (auto& task : asyncTasks) {
        task();
    }
}

}}
