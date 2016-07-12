#pragma once

#include "onair/okui/config.h"

namespace onair {
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

}}
