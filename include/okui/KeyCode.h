#pragma once

#include <okui/config.h>

#include <cstdint>

namespace okui {

/**
 * Bitfield of KeyModifiers OR'd together
 */
using KeyModifiers = uint16_t;

/**
 * Key modifiers (possibly OR'd together)
 */
enum KeyModifier : KeyModifiers {
    kNone                   = 0b00000000000,
    kLShift                 = 0b00000000001,
    kRShift                 = 0b00000000010,
    kLControl               = 0b00000000100,
    kRControl               = 0b00000001000,
    kLAlt                   = 0b00000010000,
    kRAlt                   = 0b00000100000,
    kLSuper                 = 0b00001000000, // https://en.wikipedia.org/wiki/Super_key_%28keyboard_button%29
    kRSuper                 = 0b00010000000,
    kNum                    = 0b00100000000,
    kCaps                   = 0b01000000000,
    kMode                   = 0b10000000000, // https://en.wikipedia.org/wiki/AltGr_key

    kControl                = kLControl | kRControl,
    kShift                  = kLShift   | kRShift,
    kAlt                    = kLAlt     | kRAlt,
    kSuper                  = kLSuper   | kRSuper
};

/**
 * Virtual key codes
 */
enum class KeyCode {
    kNone                   = 0,
    kUnknown                = ~0,
    kReturn                 = '\r',
    kEscape                 = '\033',
    kBackspace              = '\b',
    kTab                    = '\t',
    kSpace                  = ' ',
    kExclaim                = '!',
    kDoubleQuote            = '"',
    kHash                   = '#',
    kPercent                = '%',
    kDollar                 = '$',
    kAmpersand              = '&',
    kQuote                  = '\'',
    kLeftParen              = '(',
    kRightParen             = ')',
    kAsterisk               = '*',
    kPlus                   = '+',
    kComma                  = ',',
    kMinus                  = '-',
    kPeriod                 = '.',
    kSlash                  = '/',
    k0                      = '0',
    k1                      = '1',
    k2                      = '2',
    k3                      = '3',
    k4                      = '4',
    k5                      = '5',
    k6                      = '6',
    k7                      = '7',
    k8                      = '8',
    k9                      = '9',
    kColon                  = ':',
    kSemicolon              = ';',
    kLess                   = '<',
    kEquals                 = '=',
    kGreater                = '>',
    kQuestion               = '?',
    kAt                     = '@',
    kLeftBracket            = '[',
    kBackslash              = '\\',
    kRightBracket           = ']',
    kCaret                  = '^',
    kUnderscore             = '_',
    kBackquote              = '`',
    kA                      = 'a',
    kB                      = 'b',
    kC                      = 'c',
    kD                      = 'd',
    kE                      = 'e',
    kF                      = 'f',
    kG                      = 'g',
    kH                      = 'h',
    kI                      = 'i',
    kJ                      = 'j',
    kK                      = 'k',
    kL                      = 'l',
    kM                      = 'm',
    kN                      = 'n',
    kO                      = 'o',
    kP                      = 'p',
    kQ                      = 'q',
    kR                      = 'r',
    kS                      = 's',
    kT                      = 't',
    kU                      = 'u',
    kV                      = 'v',
    kW                      = 'w',
    kX                      = 'x',
    kY                      = 'y',
    kZ                      = 'z',
    kDelete                 = '\177',
    kCapsLock               = 256, // outside of the ascii and extended ascii range
    kF1,
    kF2,
    kF3,
    kF4,
    kF5,
    kF6,
    kF7,
    kF8,
    kF9,
    kF10,
    kF11,
    kF12,
    kF13,
    kF14,
    kF15,
    kF16,
    kF17,
    kF18,
    kF19,
    kF20,
    kF21,
    kF22,
    kF23,
    kF24,
    kLControl,
    kLShift,
    kLAlt,
    kLSuper,
    kRControl,
    kRShift,
    kRAlt,
    kRSuper,
    kMode,
    kPrintScreen,
    kScrollLock,
    kPause,
    kInsert,
    kHome,
    kPageUp,
    kEnd,
    kPageDown,
    kRight,
    kLeft,
    kDown,
    kUp,
    kNumlockClear,
    kKeypadDivide,
    kKeypadMultiply,
    kKeypadMinus,
    kKeypadPlus,
    kKeypadEnter,
    kKeypad1,
    kKeypad2,
    kKeypad3,
    kKeypad4,
    kKeypad5,
    kKeypad6,
    kKeypad7,
    kKeypad8,
    kKeypad9,
    kKeypad0,
    kKeypad00,
    kKeypad000,
    kKeypadPeriod,
    kKeypadComma,
    kKeypadEquals,
    kKeypadEqualsAS400,
    kKeypadLeftParen,
    kKeypadRightParen,
    kKeypadLeftBrace,
    kKeypadRightBrace,
    kKeypadTab,
    kKeypadBackspace,
    kKeypadA,
    kKeypadB,
    kKeypadC,
    kKeypadD,
    kKeypadE,
    kKeypadF,
    kKeypadXOR,
    kKeypadPower,
    kKeypadPercent,
    kKeypadLess,
    kKeypadGreater,
    kKeypadAmpersand,
    kKeypadDoubleAmpersand,
    kKeypadVerticalBar,
    kKeypadDoubleVerticalBar,
    kKeypadColon,
    kKeypadHash,
    kKeypadSpace,
    kKeypadAt,
    kKeypadExclaim,
    kKeypadMemStore,
    kKeypadMemRecall,
    kKeypadMemClear,
    kKeypadMemAdd,
    kKeypadMemSubtract,
    kKeypadMemMultiply,
    kKeypadMemDivide,
    kKeypadPlusMinus,
    kKeypadClear,
    kKeypadClearEntry,
    kKeypadBinary,
    kKeypadOctal,
    kKeypadDecimal,
    kKeypadHexadecimal,
    kApplication,
    kPower,
    kExecute,
    kHelp,
    kMenu,
    kSelect,
    kStop,
    kAgain,
    kUndo,
    kCut,
    kCopy,
    kPaste,
    kFind,
    kMute,
    kVolumeUp,
    kVolumeDown,
    kAltErase,
    kSysReq,
    kCancel,
    kClear,
    kPrior,
    kReturn2,
    kSeparator,
    kOut,
    kOper,
    kClearAgain,
    kCrSel,
    kExSel,
    kThousandsSeparator,
    kDecimalSeparator,
    kCurrencyUnit,
    kCurrencySubunit,
    kAudioNext,
    kAudioPrev,
    kAudioStop,
    kAudioPlay,
    kAudioMute,
    kMediaSelect,
    kWww,
    kMail,
    kCalculator,
    kComputer,
    kAppControlSearch,
    kAppControlHome,
    kAppControlBack,
    kAppControlForward,
    kAppControlStop,
    kAppControlRefresh,
    kAppControlBookmarks,
    kBrightnessDown,
    kBrightnessUp,
    kDisplaySwitch,
    kKeyboardIllumToggle,
    kKeyboardIllumDown,
    kKeyboardIllumUp,
    kEject,
    kSleep
};

} // namespace okui
