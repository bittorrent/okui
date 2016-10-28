#include <okui/ml/Environment.h>

#include <okui/ml/elements/View.h>
#include <okui/ml/elements/Text.h>

namespace okui {
namespace ml {

Environment::Environment() {
    defineElementType<elements::Text>("text");
    defineElementType<elements::View>("view");
}

}} // namespace okui::ml
