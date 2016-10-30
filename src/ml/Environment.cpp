#include <okui/ml/Environment.h>

#include <okui/ml/elements/Image.h>
#include <okui/ml/elements/Text.h>
#include <okui/ml/elements/View.h>

namespace okui {
namespace ml {

Environment::Environment() {
    defineElementType<elements::Image>("image");
    defineElementType<elements::Text>("text");
    defineElementType<elements::View>("view");
}

}} // namespace okui::ml
