#pragma once

#include "onair/okui/config.h"

namespace onair {
namespace okui {

enum class CursorType {
    kArrow,
    kText,
    kWait,
    kCrosshair,
    kWaitArrow,
    kResizeNWSE,
    kResizeNESW,
    kResizeWE,
    kResizeNS,
    kResizeAll,
    kNo,
    kHand
};

} } //namespace onair::okui
