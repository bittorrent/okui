#pragma once
#include "bittorrent/ui/config.h"
#include "bittorrent/ui/Keycode.h"
#include <SDL2/SDL.h>
#include <cstdint>

namespace bittorrent {
namespace ui {

constexpr KeyModifiers ConvertKeyModifier(uint16_t mod) {
    KeyModifiers result = KeyModifier::kNone;

    if (mod & KMOD_LSHIFT) { result |= KeyModifier::kLShift; }
    if (mod & KMOD_RSHIFT) { result |= KeyModifier::kRShift; }
    if (mod & KMOD_LCTRL)  { result |= KeyModifier::kLCtrl;  }
    if (mod & KMOD_RCTRL)  { result |= KeyModifier::kRCtrl;  }
    if (mod & KMOD_LALT)   { result |= KeyModifier::kLAlt;   }
    if (mod & KMOD_RALT)   { result |= KeyModifier::kRAlt;   }
    if (mod & KMOD_LGUI)   { result |= KeyModifier::kLSuper; }
    if (mod & KMOD_RGUI)   { result |= KeyModifier::kRSuper; }
    if (mod & KMOD_NUM)    { result |= KeyModifier::kNum;    }
    if (mod & KMOD_CAPS)   { result |= KeyModifier::kCaps;   }
    if (mod & KMOD_MODE)   { result |= KeyModifier::kMode;   }

    return result;
}

constexpr Keycode ConvertKeycode(SDL_Keycode keycode) {
    switch (keycode) {
        case SDLK_UNKNOWN:           return Keycode::kUnknown;
        case SDLK_RETURN:            return Keycode::kReturn;
        case SDLK_ESCAPE:            return Keycode::kEscape;
        case SDLK_BACKSPACE:         return Keycode::kBackspace;
        case SDLK_TAB:               return Keycode::kTab;
        case SDLK_SPACE:             return Keycode::kSpace;
        case SDLK_EXCLAIM:           return Keycode::kExclaim;
        case SDLK_QUOTEDBL:          return Keycode::kDoubleQuote;
        case SDLK_HASH:              return Keycode::kHash;
        case SDLK_PERCENT:           return Keycode::kPercent;
        case SDLK_DOLLAR:            return Keycode::kDollar;
        case SDLK_AMPERSAND:         return Keycode::kAmpersand;
        case SDLK_QUOTE:             return Keycode::kQuote;
        case SDLK_LEFTPAREN:         return Keycode::kLeftParen;
        case SDLK_RIGHTPAREN:        return Keycode::kRightParen;
        case SDLK_ASTERISK:          return Keycode::kAsterisk;
        case SDLK_PLUS:              return Keycode::kPlus;
        case SDLK_COMMA:             return Keycode::kComma;
        case SDLK_MINUS:             return Keycode::kMinus;
        case SDLK_PERIOD:            return Keycode::kPeriod;
        case SDLK_SLASH:             return Keycode::kSlash;
        case SDLK_0:                 return Keycode::k0;
        case SDLK_1:                 return Keycode::k1;
        case SDLK_2:                 return Keycode::k2;
        case SDLK_3:                 return Keycode::k3;
        case SDLK_4:                 return Keycode::k4;
        case SDLK_5:                 return Keycode::k5;
        case SDLK_6:                 return Keycode::k6;
        case SDLK_7:                 return Keycode::k7;
        case SDLK_8:                 return Keycode::k8;
        case SDLK_9:                 return Keycode::k9;
        case SDLK_COLON:             return Keycode::kColon;
        case SDLK_SEMICOLON:         return Keycode::kSemicolon;
        case SDLK_LESS:              return Keycode::kLess;
        case SDLK_EQUALS:            return Keycode::kEquals;
        case SDLK_GREATER:           return Keycode::kGreater;
        case SDLK_QUESTION:          return Keycode::kQuestion;
        case SDLK_AT:                return Keycode::kAt;
        case SDLK_LEFTBRACKET:       return Keycode::kLeftBracket;
        case SDLK_BACKSLASH:         return Keycode::kBackslash;
        case SDLK_RIGHTBRACKET:      return Keycode::kRightBracket;
        case SDLK_CARET:             return Keycode::kCaret;
        case SDLK_UNDERSCORE:        return Keycode::kUnderscore;
        case SDLK_BACKQUOTE:         return Keycode::kBackquote;
        case SDLK_a:                 return Keycode::kA;
        case SDLK_b:                 return Keycode::kB;
        case SDLK_c:                 return Keycode::kC;
        case SDLK_d:                 return Keycode::kD;
        case SDLK_e:                 return Keycode::kE;
        case SDLK_f:                 return Keycode::kF;
        case SDLK_g:                 return Keycode::kG;
        case SDLK_h:                 return Keycode::kH;
        case SDLK_i:                 return Keycode::kI;
        case SDLK_j:                 return Keycode::kJ;
        case SDLK_k:                 return Keycode::kK;
        case SDLK_l:                 return Keycode::kL;
        case SDLK_m:                 return Keycode::kM;
        case SDLK_n:                 return Keycode::kN;
        case SDLK_o:                 return Keycode::kO;
        case SDLK_p:                 return Keycode::kP;
        case SDLK_q:                 return Keycode::kQ;
        case SDLK_r:                 return Keycode::kR;
        case SDLK_s:                 return Keycode::kS;
        case SDLK_t:                 return Keycode::kT;
        case SDLK_u:                 return Keycode::kU;
        case SDLK_v:                 return Keycode::kV;
        case SDLK_w:                 return Keycode::kW;
        case SDLK_x:                 return Keycode::kX;
        case SDLK_y:                 return Keycode::kY;
        case SDLK_z:                 return Keycode::kZ;
        case SDLK_CAPSLOCK:          return Keycode::kCapsLock;
        case SDLK_F1:                return Keycode::kF1;
        case SDLK_F2:                return Keycode::kF2;
        case SDLK_F3:                return Keycode::kF3;
        case SDLK_F4:                return Keycode::kF4;
        case SDLK_F5:                return Keycode::kF5;
        case SDLK_F6:                return Keycode::kF6;
        case SDLK_F7:                return Keycode::kF7;
        case SDLK_F8:                return Keycode::kF8;
        case SDLK_F9:                return Keycode::kF9;
        case SDLK_F10:               return Keycode::kF10;
        case SDLK_F11:               return Keycode::kF11;
        case SDLK_F12:               return Keycode::kF12;
        case SDLK_PRINTSCREEN:       return Keycode::kPrintScreen;
        case SDLK_SCROLLLOCK:        return Keycode::kScrollLock;
        case SDLK_PAUSE:             return Keycode::kPause;
        case SDLK_INSERT:            return Keycode::kInsert;
        case SDLK_HOME:              return Keycode::kHome;
        case SDLK_PAGEUP:            return Keycode::kPageUp;
        case SDLK_DELETE:            return Keycode::kDelete;
        case SDLK_END:               return Keycode::kEnd;
        case SDLK_PAGEDOWN:          return Keycode::kPageDown;
        case SDLK_RIGHT:             return Keycode::kRight;
        case SDLK_LEFT:              return Keycode::kLeft;
        case SDLK_DOWN:              return Keycode::kDown;
        case SDLK_UP:                return Keycode::kUp;
        case SDLK_NUMLOCKCLEAR:      return Keycode::kNumlockClear;
        case SDLK_KP_DIVIDE:         return Keycode::kKeypad_Divide;
        case SDLK_KP_MULTIPLY:       return Keycode::kKeypad_Multiply;
        case SDLK_KP_MINUS:          return Keycode::kKeypad_Minus;
        case SDLK_KP_PLUS:           return Keycode::kKeypad_Plus;
        case SDLK_KP_ENTER:          return Keycode::kKeypad_Enter;
        case SDLK_KP_1:              return Keycode::kKeypad_1;
        case SDLK_KP_2:              return Keycode::kKeypad_2;
        case SDLK_KP_3:              return Keycode::kKeypad_3;
        case SDLK_KP_4:              return Keycode::kKeypad_4;
        case SDLK_KP_5:              return Keycode::kKeypad_5;
        case SDLK_KP_6:              return Keycode::kKeypad_6;
        case SDLK_KP_7:              return Keycode::kKeypad_7;
        case SDLK_KP_8:              return Keycode::kKeypad_8;
        case SDLK_KP_9:              return Keycode::kKeypad_9;
        case SDLK_KP_0:              return Keycode::kKeypad_0;
        case SDLK_KP_PERIOD:         return Keycode::kKeypad_Period;
        case SDLK_APPLICATION:       return Keycode::kApplication;
        case SDLK_POWER:             return Keycode::kPower;
        case SDLK_KP_EQUALS:         return Keycode::kKeypad_Equals;
        case SDLK_F13:               return Keycode::kF13;
        case SDLK_F14:               return Keycode::kF14;
        case SDLK_F15:               return Keycode::kF15;
        case SDLK_F16:               return Keycode::kF16;
        case SDLK_F17:               return Keycode::kF17;
        case SDLK_F18:               return Keycode::kF18;
        case SDLK_F19:               return Keycode::kF19;
        case SDLK_F20:               return Keycode::kF20;
        case SDLK_F21:               return Keycode::kF21;
        case SDLK_F22:               return Keycode::kF22;
        case SDLK_F23:               return Keycode::kF23;
        case SDLK_F24:               return Keycode::kF24;
        case SDLK_EXECUTE:           return Keycode::kExecute;
        case SDLK_HELP:              return Keycode::kHelp;
        case SDLK_MENU:              return Keycode::kMenu;
        case SDLK_SELECT:            return Keycode::kSelect;
        case SDLK_STOP:              return Keycode::kStop;
        case SDLK_AGAIN:             return Keycode::kAgain;
        case SDLK_UNDO:              return Keycode::kUndo;
        case SDLK_CUT:               return Keycode::kCut;
        case SDLK_COPY:              return Keycode::kCopy;
        case SDLK_PASTE:             return Keycode::kPaste;
        case SDLK_FIND:              return Keycode::kFind;
        case SDLK_MUTE:              return Keycode::kMute;
        case SDLK_VOLUMEUP:          return Keycode::kVolumeUp;
        case SDLK_VOLUMEDOWN:        return Keycode::kVolumeDown;
        case SDLK_KP_COMMA:          return Keycode::kKeypad_Comma;
        case SDLK_KP_EQUALSAS400:    return Keycode::kKeypad_EqualsAS400;
        case SDLK_ALTERASE:          return Keycode::kAltErase;
        case SDLK_SYSREQ:            return Keycode::kSysReq;
        case SDLK_CANCEL:            return Keycode::kCancel;
        case SDLK_CLEAR:             return Keycode::kClear;
        case SDLK_PRIOR:             return Keycode::kPrior;
        case SDLK_RETURN2:           return Keycode::kReturn2;
        case SDLK_SEPARATOR:         return Keycode::kSeparator;
        case SDLK_OUT:               return Keycode::kOut;
        case SDLK_OPER:              return Keycode::kOper;
        case SDLK_CLEARAGAIN:        return Keycode::kClearAgain;
        case SDLK_CRSEL:             return Keycode::kCrSel;
        case SDLK_EXSEL:             return Keycode::kExSel;
        case SDLK_KP_00:             return Keycode::kKeypad_00;
        case SDLK_KP_000:            return Keycode::kKeypad_000;
        case SDLK_THOUSANDSSEPARATOR:return Keycode::kThousandsSeparator;
        case SDLK_DECIMALSEPARATOR:  return Keycode::kDecimalSeparator;
        case SDLK_CURRENCYUNIT:      return Keycode::kCurrencyUnit;
        case SDLK_CURRENCYSUBUNIT:   return Keycode::kCurrencySubunit;
        case SDLK_KP_LEFTPAREN:      return Keycode::kKeypad_LeftParen;
        case SDLK_KP_RIGHTPAREN:     return Keycode::kKeypad_RightParen;
        case SDLK_KP_LEFTBRACE:      return Keycode::kKeypad_LeftBrace;
        case SDLK_KP_RIGHTBRACE:     return Keycode::kKeypad_RightBrace;
        case SDLK_KP_TAB:            return Keycode::kKeypad_Tab;
        case SDLK_KP_BACKSPACE:      return Keycode::kKeypad_Backspace;
        case SDLK_KP_A:              return Keycode::kKeypad_A;
        case SDLK_KP_B:              return Keycode::kKeypad_B;
        case SDLK_KP_C:              return Keycode::kKeypad_C;
        case SDLK_KP_D:              return Keycode::kKeypad_D;
        case SDLK_KP_E:              return Keycode::kKeypad_E;
        case SDLK_KP_F:              return Keycode::kKeypad_F;
        case SDLK_KP_XOR:            return Keycode::kKeypad_XOR;
        case SDLK_KP_POWER:          return Keycode::kKeypad_Power;
        case SDLK_KP_PERCENT:        return Keycode::kKeypad_Percent;
        case SDLK_KP_LESS:           return Keycode::kKeypad_Less;
        case SDLK_KP_GREATER:        return Keycode::kKeypad_Greater;
        case SDLK_KP_AMPERSAND:      return Keycode::kKeypad_Ampersand;
        case SDLK_KP_DBLAMPERSAND:   return Keycode::kKeypad_DoubleAmpersand;
        case SDLK_KP_VERTICALBAR:    return Keycode::kKeypad_VerticalBar;
        case SDLK_KP_DBLVERTICALBAR: return Keycode::kKeypad_DoubleVerticalBar;
        case SDLK_KP_COLON:          return Keycode::kKeypad_Colon;
        case SDLK_KP_HASH:           return Keycode::kKeypad_Hash;
        case SDLK_KP_SPACE:          return Keycode::kKeypad_Space;
        case SDLK_KP_AT:             return Keycode::kKeypad_At;
        case SDLK_KP_EXCLAM:         return Keycode::kKeypad_Exclaim;
        case SDLK_KP_MEMSTORE:       return Keycode::kKeypad_MemStore;
        case SDLK_KP_MEMRECALL:      return Keycode::kKeypad_MemRecall;
        case SDLK_KP_MEMCLEAR:       return Keycode::kKeypad_MemClear;
        case SDLK_KP_MEMADD:         return Keycode::kKeypad_MemAdd;
        case SDLK_KP_MEMSUBTRACT:    return Keycode::kKeypad_MemSubtract;
        case SDLK_KP_MEMMULTIPLY:    return Keycode::kKeypad_MemMultiply;
        case SDLK_KP_MEMDIVIDE:      return Keycode::kKeypad_MemDivide;
        case SDLK_KP_PLUSMINUS:      return Keycode::kKeypad_PlusMinus;
        case SDLK_KP_CLEAR:          return Keycode::kKeypad_Clear;
        case SDLK_KP_CLEARENTRY:     return Keycode::kKeypad_ClearEntry;
        case SDLK_KP_BINARY:         return Keycode::kKeypad_Binary;
        case SDLK_KP_OCTAL:          return Keycode::kKeypad_Octal;
        case SDLK_KP_DECIMAL:        return Keycode::kKeypad_Decimal;
        case SDLK_KP_HEXADECIMAL:    return Keycode::kKeypad_Hexadecimal;
        case SDLK_LCTRL:             return Keycode::kLCtrl;
        case SDLK_LSHIFT:            return Keycode::kLShift;
        case SDLK_LALT:              return Keycode::kLAlt;
        case SDLK_LGUI:              return Keycode::kLSuper;
        case SDLK_RCTRL:             return Keycode::kRCtrl;
        case SDLK_RSHIFT:            return Keycode::kRShift;
        case SDLK_RALT:              return Keycode::kRAlt;
        case SDLK_RGUI:              return Keycode::kRSuper;
        case SDLK_MODE:              return Keycode::kMode;
        case SDLK_AUDIONEXT:         return Keycode::kAudio_Next;
        case SDLK_AUDIOPREV:         return Keycode::kAudio_Prev;
        case SDLK_AUDIOSTOP:         return Keycode::kAudio_Stop;
        case SDLK_AUDIOPLAY:         return Keycode::kAudio_Play;
        case SDLK_AUDIOMUTE:         return Keycode::kAudio_Mute;
        case SDLK_MEDIASELECT:       return Keycode::kMediaSelect;
        case SDLK_WWW:               return Keycode::kWww;
        case SDLK_MAIL:              return Keycode::kMail;
        case SDLK_CALCULATOR:        return Keycode::kCalculator;
        case SDLK_COMPUTER:          return Keycode::kComputer;
        case SDLK_AC_SEARCH:         return Keycode::kAppControl_Search;
        case SDLK_AC_HOME:           return Keycode::kAppControl_Home;
        case SDLK_AC_BACK:           return Keycode::kAppControl_Back;
        case SDLK_AC_FORWARD:        return Keycode::kAppControl_Forward;
        case SDLK_AC_STOP:           return Keycode::kAppControl_Stop;
        case SDLK_AC_REFRESH:        return Keycode::kAppControl_Refresh;
        case SDLK_AC_BOOKMARKS:      return Keycode::kAppControl_Bookmarks;
        case SDLK_BRIGHTNESSDOWN:    return Keycode::kBrightnessDown;
        case SDLK_BRIGHTNESSUP:      return Keycode::kBrightnessUp;
        case SDLK_DISPLAYSWITCH:     return Keycode::kDisplaySwitch;
        case SDLK_KBDILLUMTOGGLE:    return Keycode::kKeyboard_IllumToggle;
        case SDLK_KBDILLUMDOWN:      return Keycode::kKeyboard_IllumDown;
        case SDLK_KBDILLUMUP:        return Keycode::kKeyboard_IllumUp;
        case SDLK_EJECT:             return Keycode::kEject;
        case SDLK_SLEEP:             return Keycode::kSleep;
        default:                     return Keycode::kUnknown;
    }
}

}}
