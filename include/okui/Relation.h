#pragma once

#include "okui/config.h"

namespace okui {

/**
* Describes the types of relationships okui elements such as views, windows, and applications
* may have to eachother.
*/
enum class Relation {
    kAny,
    kHierarchy,
    kAncestor,
    kDescendant,
    kSibling,
    kSelf,
};

} // namespace okui
