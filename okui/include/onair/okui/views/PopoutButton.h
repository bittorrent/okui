#pragma once

#include "onair/okui/config.h"

#include "onair/okui/views/Button.h"
#include "onair/okui/views/ImageButton.h"
#include "onair/okui/views/PopoutFocus.h"

namespace onair {
namespace okui {
namespace views {

template <typename BaseView>
using PopoutButton = PopoutFocus<Button<BaseView>>;

using PopoutImageButton = PopoutFocus<ImageButton>;

}}}
