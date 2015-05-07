#pragma once
#include "bittorrent/ui/config.h"
#include "bittorrent/ui/View.h"
#include "bittorrent/ui/views/TextView.h"
#include "bittorrent/ui/Color.h"

namespace bittorrent {
namespace ui {
namespace views {

class TextFieldView : public bittorrent::ui::View {
public:
	using EnterHandler = std::function<void(const std::string&)>;

	TextFieldView();

	void setEnterHandler(EnterHandler handler) { _enterHandler = std::move(handler); }

	const std::string& text() const            { return _text; }

	TextView& textView()                       { return _textView; }

    virtual void layout() override;

    virtual void focusGained() override;
    virtual void focusLost() override;

    virtual void mouseDown(MouseButton button, int x, int y) override;
    virtual void textInput(const std::string& text) override;
    virtual void keyDown(Keycode key, KeyModifiers mod, bool repeat) override;

private:
	std::string  _text;
	TextView     _textView;
	View         _textViewClip;
	EnterHandler _enterHandler;
};

}}}
