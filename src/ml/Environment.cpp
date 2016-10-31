#include <okui/ml/Environment.h>

#include <okui/ml/elements/Button.h>
#include <okui/ml/elements/Image.h>
#include <okui/ml/elements/Popout.h>
#include <okui/ml/elements/Text.h>
#include <okui/ml/elements/View.h>

namespace okui {
namespace ml {

Environment::Environment() {
    defineElementType<elements::Button>("button");
    defineElementType<elements::Image>("image");
    defineElementType<elements::Popout>("popout");
    defineElementType<elements::Text>("text");
    defineElementType<elements::View>("view");
}

}} // namespace okui::ml
