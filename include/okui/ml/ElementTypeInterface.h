#pragma once

#include <memory>

namespace okui {
namespace ml {

class ElementInterface;

class ElementTypeInterface {
public:
    virtual ~ElementTypeInterface() {}

    virtual std::unique_ptr<ElementInterface> create() const = 0;
};

}} // namespace okui::ml
