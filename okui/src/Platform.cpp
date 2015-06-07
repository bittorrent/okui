#include "onair/okui/Platform.h"

#if ONAIR_MAC_OS_X
#import <AppKit/NSOpenPanel.h>
#endif

namespace onair {
namespace okui {

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
