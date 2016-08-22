#pragma once
#include "onair/okui/config.h"
#include "onair/okui/KeyCode.h"

#include <SDL.h>
#include <cstdint>

namespace onair {
namespace okui {

constexpr KeyModifiers ConvertKeyModifiers(uint16_t mods) {
    KeyModifiers result = KeyModifier::kNone;

    if (mods & KMOD_LSHIFT) { result |= KeyModifier::kLShift; }
    if (mods & KMOD_RSHIFT) { result |= KeyModifier::kRShift; }
    if (mods & KMOD_LCTRL)  { result |= KeyModifier::kLControl; }
    if (mods & KMOD_RCTRL)  { result |= KeyModifier::kRControl; }
    if (mods & KMOD_LALT)   { result |= KeyModifier::kLAlt;   }
    if (mods & KMOD_RALT)   { result |= KeyModifier::kRAlt;   }
    if (mods & KMOD_LGUI)   { result |= KeyModifier::kLSuper; }
    if (mods & KMOD_RGUI)   { result |= KeyModifier::kRSuper; }
    if (mods & KMOD_NUM)    { result |= KeyModifier::kNum;    }
    if (mods & KMOD_CAPS)   { result |= KeyModifier::kCaps;   }
    if (mods & KMOD_MODE)   { result |= KeyModifier::kMode;   }

    return result;
}

constexpr KeyCode ConvertKeyCode(SDL_Keycode keyCode) {
    switch (keyCode) {
        case SDLK_UNKNOWN:           return KeyCode::kUnknown;
        case SDLK_RETURN:            return KeyCode::kReturn;
        case SDLK_ESCAPE:            return KeyCode::kEscape;
        case SDLK_BACKSPACE:         return KeyCode::kBackspace;
        case SDLK_TAB:               return KeyCode::kTab;
        case SDLK_SPACE:             return KeyCode::kSpace;
        case SDLK_EXCLAIM:           return KeyCode::kExclaim;
        case SDLK_QUOTEDBL:          return KeyCode::kDoubleQuote;
        case SDLK_HASH:              return KeyCode::kHash;
        case SDLK_PERCENT:           return KeyCode::kPercent;
        case SDLK_DOLLAR:            return KeyCode::kDollar;
        case SDLK_AMPERSAND:         return KeyCode::kAmpersand;
        case SDLK_QUOTE:             return KeyCode::kQuote;
        case SDLK_LEFTPAREN:         return KeyCode::kLeftParen;
        case SDLK_RIGHTPAREN:        return KeyCode::kRightParen;
        case SDLK_ASTERISK:          return KeyCode::kAsterisk;
        case SDLK_PLUS:              return KeyCode::kPlus;
        case SDLK_COMMA:             return KeyCode::kComma;
        case SDLK_MINUS:             return KeyCode::kMinus;
        case SDLK_PERIOD:            return KeyCode::kPeriod;
        case SDLK_SLASH:             return KeyCode::kSlash;
        case SDLK_0:                 return KeyCode::k0;
        case SDLK_1:                 return KeyCode::k1;
        case SDLK_2:                 return KeyCode::k2;
        case SDLK_3:                 return KeyCode::k3;
        case SDLK_4:                 return KeyCode::k4;
        case SDLK_5:                 return KeyCode::k5;
        case SDLK_6:                 return KeyCode::k6;
        case SDLK_7:                 return KeyCode::k7;
        case SDLK_8:                 return KeyCode::k8;
        case SDLK_9:                 return KeyCode::k9;
        case SDLK_COLON:             return KeyCode::kColon;
        case SDLK_SEMICOLON:         return KeyCode::kSemicolon;
        case SDLK_LESS:              return KeyCode::kLess;
        case SDLK_EQUALS:            return KeyCode::kEquals;
        case SDLK_GREATER:           return KeyCode::kGreater;
        case SDLK_QUESTION:          return KeyCode::kQuestion;
        case SDLK_AT:                return KeyCode::kAt;
        case SDLK_LEFTBRACKET:       return KeyCode::kLeftBracket;
        case SDLK_BACKSLASH:         return KeyCode::kBackslash;
        case SDLK_RIGHTBRACKET:      return KeyCode::kRightBracket;
        case SDLK_CARET:             return KeyCode::kCaret;
        case SDLK_UNDERSCORE:        return KeyCode::kUnderscore;
        case SDLK_BACKQUOTE:         return KeyCode::kBackquote;
        case SDLK_a:                 return KeyCode::kA;
        case SDLK_b:                 return KeyCode::kB;
        case SDLK_c:                 return KeyCode::kC;
        case SDLK_d:                 return KeyCode::kD;
        case SDLK_e:                 return KeyCode::kE;
        case SDLK_f:                 return KeyCode::kF;
        case SDLK_g:                 return KeyCode::kG;
        case SDLK_h:                 return KeyCode::kH;
        case SDLK_i:                 return KeyCode::kI;
        case SDLK_j:                 return KeyCode::kJ;
        case SDLK_k:                 return KeyCode::kK;
        case SDLK_l:                 return KeyCode::kL;
        case SDLK_m:                 return KeyCode::kM;
        case SDLK_n:                 return KeyCode::kN;
        case SDLK_o:                 return KeyCode::kO;
        case SDLK_p:                 return KeyCode::kP;
        case SDLK_q:                 return KeyCode::kQ;
        case SDLK_r:                 return KeyCode::kR;
        case SDLK_s:                 return KeyCode::kS;
        case SDLK_t:                 return KeyCode::kT;
        case SDLK_u:                 return KeyCode::kU;
        case SDLK_v:                 return KeyCode::kV;
        case SDLK_w:                 return KeyCode::kW;
        case SDLK_x:                 return KeyCode::kX;
        case SDLK_y:                 return KeyCode::kY;
        case SDLK_z:                 return KeyCode::kZ;
        case SDLK_CAPSLOCK:          return KeyCode::kCapsLock;
        case SDLK_F1:                return KeyCode::kF1;
        case SDLK_F2:                return KeyCode::kF2;
        case SDLK_F3:                return KeyCode::kF3;
        case SDLK_F4:                return KeyCode::kF4;
        case SDLK_F5:                return KeyCode::kF5;
        case SDLK_F6:                return KeyCode::kF6;
        case SDLK_F7:                return KeyCode::kF7;
        case SDLK_F8:                return KeyCode::kF8;
        case SDLK_F9:                return KeyCode::kF9;
        case SDLK_F10:               return KeyCode::kF10;
        case SDLK_F11:               return KeyCode::kF11;
        case SDLK_F12:               return KeyCode::kF12;
        case SDLK_PRINTSCREEN:       return KeyCode::kPrintScreen;
        case SDLK_SCROLLLOCK:        return KeyCode::kScrollLock;
        case SDLK_PAUSE:             return KeyCode::kPause;
        case SDLK_INSERT:            return KeyCode::kInsert;
        case SDLK_HOME:              return KeyCode::kHome;
        case SDLK_PAGEUP:            return KeyCode::kPageUp;
        case SDLK_DELETE:            return KeyCode::kDelete;
        case SDLK_END:               return KeyCode::kEnd;
        case SDLK_PAGEDOWN:          return KeyCode::kPageDown;
        case SDLK_RIGHT:             return KeyCode::kRight;
        case SDLK_LEFT:              return KeyCode::kLeft;
        case SDLK_DOWN:              return KeyCode::kDown;
        case SDLK_UP:                return KeyCode::kUp;
        case SDLK_NUMLOCKCLEAR:      return KeyCode::kNumlockClear;
        case SDLK_KP_DIVIDE:         return KeyCode::kKeypadDivide;
        case SDLK_KP_MULTIPLY:       return KeyCode::kKeypadMultiply;
        case SDLK_KP_MINUS:          return KeyCode::kKeypadMinus;
        case SDLK_KP_PLUS:           return KeyCode::kKeypadPlus;
        case SDLK_KP_ENTER:          return KeyCode::kKeypadEnter;
        case SDLK_KP_1:              return KeyCode::kKeypad1;
        case SDLK_KP_2:              return KeyCode::kKeypad2;
        case SDLK_KP_3:              return KeyCode::kKeypad3;
        case SDLK_KP_4:              return KeyCode::kKeypad4;
        case SDLK_KP_5:              return KeyCode::kKeypad5;
        case SDLK_KP_6:              return KeyCode::kKeypad6;
        case SDLK_KP_7:              return KeyCode::kKeypad7;
        case SDLK_KP_8:              return KeyCode::kKeypad8;
        case SDLK_KP_9:              return KeyCode::kKeypad9;
        case SDLK_KP_0:              return KeyCode::kKeypad0;
        case SDLK_KP_PERIOD:         return KeyCode::kKeypadPeriod;
        case SDLK_APPLICATION:       return KeyCode::kApplication;
        case SDLK_POWER:             return KeyCode::kPower;
        case SDLK_KP_EQUALS:         return KeyCode::kKeypadEquals;
        case SDLK_F13:               return KeyCode::kF13;
        case SDLK_F14:               return KeyCode::kF14;
        case SDLK_F15:               return KeyCode::kF15;
        case SDLK_F16:               return KeyCode::kF16;
        case SDLK_F17:               return KeyCode::kF17;
        case SDLK_F18:               return KeyCode::kF18;
        case SDLK_F19:               return KeyCode::kF19;
        case SDLK_F20:               return KeyCode::kF20;
        case SDLK_F21:               return KeyCode::kF21;
        case SDLK_F22:               return KeyCode::kF22;
        case SDLK_F23:               return KeyCode::kF23;
        case SDLK_F24:               return KeyCode::kF24;
        case SDLK_EXECUTE:           return KeyCode::kExecute;
        case SDLK_HELP:              return KeyCode::kHelp;
        case SDLK_MENU:              return KeyCode::kMenu;
        case SDLK_SELECT:            return KeyCode::kSelect;
        case SDLK_STOP:              return KeyCode::kStop;
        case SDLK_AGAIN:             return KeyCode::kAgain;
        case SDLK_UNDO:              return KeyCode::kUndo;
        case SDLK_CUT:               return KeyCode::kCut;
        case SDLK_COPY:              return KeyCode::kCopy;
        case SDLK_PASTE:             return KeyCode::kPaste;
        case SDLK_FIND:              return KeyCode::kFind;
        case SDLK_MUTE:              return KeyCode::kMute;
        case SDLK_VOLUMEUP:          return KeyCode::kVolumeUp;
        case SDLK_VOLUMEDOWN:        return KeyCode::kVolumeDown;
        case SDLK_KP_COMMA:          return KeyCode::kKeypadComma;
        case SDLK_KP_EQUALSAS400:    return KeyCode::kKeypadEqualsAS400;
        case SDLK_ALTERASE:          return KeyCode::kAltErase;
        case SDLK_SYSREQ:            return KeyCode::kSysReq;
        case SDLK_CANCEL:            return KeyCode::kCancel;
        case SDLK_CLEAR:             return KeyCode::kClear;
        case SDLK_PRIOR:             return KeyCode::kPrior;
        case SDLK_RETURN2:           return KeyCode::kReturn2;
        case SDLK_SEPARATOR:         return KeyCode::kSeparator;
        case SDLK_OUT:               return KeyCode::kOut;
        case SDLK_OPER:              return KeyCode::kOper;
        case SDLK_CLEARAGAIN:        return KeyCode::kClearAgain;
        case SDLK_CRSEL:             return KeyCode::kCrSel;
        case SDLK_EXSEL:             return KeyCode::kExSel;
        case SDLK_KP_00:             return KeyCode::kKeypad00;
        case SDLK_KP_000:            return KeyCode::kKeypad000;
        case SDLK_THOUSANDSSEPARATOR:return KeyCode::kThousandsSeparator;
        case SDLK_DECIMALSEPARATOR:  return KeyCode::kDecimalSeparator;
        case SDLK_CURRENCYUNIT:      return KeyCode::kCurrencyUnit;
        case SDLK_CURRENCYSUBUNIT:   return KeyCode::kCurrencySubunit;
        case SDLK_KP_LEFTPAREN:      return KeyCode::kKeypadLeftParen;
        case SDLK_KP_RIGHTPAREN:     return KeyCode::kKeypadRightParen;
        case SDLK_KP_LEFTBRACE:      return KeyCode::kKeypadLeftBrace;
        case SDLK_KP_RIGHTBRACE:     return KeyCode::kKeypadRightBrace;
        case SDLK_KP_TAB:            return KeyCode::kKeypadTab;
        case SDLK_KP_BACKSPACE:      return KeyCode::kKeypadBackspace;
        case SDLK_KP_A:              return KeyCode::kKeypadA;
        case SDLK_KP_B:              return KeyCode::kKeypadB;
        case SDLK_KP_C:              return KeyCode::kKeypadC;
        case SDLK_KP_D:              return KeyCode::kKeypadD;
        case SDLK_KP_E:              return KeyCode::kKeypadE;
        case SDLK_KP_F:              return KeyCode::kKeypadF;
        case SDLK_KP_XOR:            return KeyCode::kKeypadXOR;
        case SDLK_KP_POWER:          return KeyCode::kKeypadPower;
        case SDLK_KP_PERCENT:        return KeyCode::kKeypadPercent;
        case SDLK_KP_LESS:           return KeyCode::kKeypadLess;
        case SDLK_KP_GREATER:        return KeyCode::kKeypadGreater;
        case SDLK_KP_AMPERSAND:      return KeyCode::kKeypadAmpersand;
        case SDLK_KP_DBLAMPERSAND:   return KeyCode::kKeypadDoubleAmpersand;
        case SDLK_KP_VERTICALBAR:    return KeyCode::kKeypadVerticalBar;
        case SDLK_KP_DBLVERTICALBAR: return KeyCode::kKeypadDoubleVerticalBar;
        case SDLK_KP_COLON:          return KeyCode::kKeypadColon;
        case SDLK_KP_HASH:           return KeyCode::kKeypadHash;
        case SDLK_KP_SPACE:          return KeyCode::kKeypadSpace;
        case SDLK_KP_AT:             return KeyCode::kKeypadAt;
        case SDLK_KP_EXCLAM:         return KeyCode::kKeypadExclaim;
        case SDLK_KP_MEMSTORE:       return KeyCode::kKeypadMemStore;
        case SDLK_KP_MEMRECALL:      return KeyCode::kKeypadMemRecall;
        case SDLK_KP_MEMCLEAR:       return KeyCode::kKeypadMemClear;
        case SDLK_KP_MEMADD:         return KeyCode::kKeypadMemAdd;
        case SDLK_KP_MEMSUBTRACT:    return KeyCode::kKeypadMemSubtract;
        case SDLK_KP_MEMMULTIPLY:    return KeyCode::kKeypadMemMultiply;
        case SDLK_KP_MEMDIVIDE:      return KeyCode::kKeypadMemDivide;
        case SDLK_KP_PLUSMINUS:      return KeyCode::kKeypadPlusMinus;
        case SDLK_KP_CLEAR:          return KeyCode::kKeypadClear;
        case SDLK_KP_CLEARENTRY:     return KeyCode::kKeypadClearEntry;
        case SDLK_KP_BINARY:         return KeyCode::kKeypadBinary;
        case SDLK_KP_OCTAL:          return KeyCode::kKeypadOctal;
        case SDLK_KP_DECIMAL:        return KeyCode::kKeypadDecimal;
        case SDLK_KP_HEXADECIMAL:    return KeyCode::kKeypadHexadecimal;
        case SDLK_LCTRL:             return KeyCode::kLControl;
        case SDLK_LSHIFT:            return KeyCode::kLShift;
        case SDLK_LALT:              return KeyCode::kLAlt;
        case SDLK_LGUI:              return KeyCode::kLSuper;
        case SDLK_RCTRL:             return KeyCode::kRControl;
        case SDLK_RSHIFT:            return KeyCode::kRShift;
        case SDLK_RALT:              return KeyCode::kRAlt;
        case SDLK_RGUI:              return KeyCode::kRSuper;
        case SDLK_MODE:              return KeyCode::kMode;
        case SDLK_AUDIONEXT:         return KeyCode::kAudioNext;
        case SDLK_AUDIOPREV:         return KeyCode::kAudioPrev;
        case SDLK_AUDIOSTOP:         return KeyCode::kAudioStop;
        case SDLK_AUDIOPLAY:         return KeyCode::kAudioPlay;
        case SDLK_AUDIOMUTE:         return KeyCode::kAudioMute;
        case SDLK_MEDIASELECT:       return KeyCode::kMediaSelect;
        case SDLK_WWW:               return KeyCode::kWww;
        case SDLK_MAIL:              return KeyCode::kMail;
        case SDLK_CALCULATOR:        return KeyCode::kCalculator;
        case SDLK_COMPUTER:          return KeyCode::kComputer;
        case SDLK_AC_SEARCH:         return KeyCode::kAppControlSearch;
        case SDLK_AC_HOME:           return KeyCode::kAppControlHome;
        case SDLK_AC_BACK:           return KeyCode::kAppControlBack;
        case SDLK_AC_FORWARD:        return KeyCode::kAppControlForward;
        case SDLK_AC_STOP:           return KeyCode::kAppControlStop;
        case SDLK_AC_REFRESH:        return KeyCode::kAppControlRefresh;
        case SDLK_AC_BOOKMARKS:      return KeyCode::kAppControlBookmarks;
        case SDLK_BRIGHTNESSDOWN:    return KeyCode::kBrightnessDown;
        case SDLK_BRIGHTNESSUP:      return KeyCode::kBrightnessUp;
        case SDLK_DISPLAYSWITCH:     return KeyCode::kDisplaySwitch;
        case SDLK_KBDILLUMTOGGLE:    return KeyCode::kKeyboardIllumToggle;
        case SDLK_KBDILLUMDOWN:      return KeyCode::kKeyboardIllumDown;
        case SDLK_KBDILLUMUP:        return KeyCode::kKeyboardIllumUp;
        case SDLK_EJECT:             return KeyCode::kEject;
        case SDLK_SLEEP:             return KeyCode::kSleep;
        default:                     return KeyCode::kUnknown;
    }
}

}}
