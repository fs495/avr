# -*- ruby coding: utf-8 -*-

module ScanCode
    K_GraveAccent = 0x35 # ` ~
    K_1 = 0x1e # 1 !
    K_2 = 0x1f # 2 @
    K_3 = 0x20 # 3 #
    K_4 = 0x21 # 4 $
    K_5 = 0x22 # 5 %
    K_6 = 0x23 # 6 ^
    K_7 = 0x24 # 7 &
    K_8 = 0x25 # 8 *
    K_9 = 0x26 # 9 (
    K_0 = 0x27 # 0 )

    K_Minus = 0x2d # - _
    K_Equal = 0x2e # = +
    K_Intl3 = 0x89 # (JIS only) ￥ ｜
    K_BackSpace = 0x2a # Del BS

    K_Tab = 0x2b
    K_Q = 0x14
    K_W = 0x1a
    K_E = 0x08
    K_R = 0x15
    K_T = 0x17
    K_Y = 0x1c
    K_U = 0x18
    K_I = 0x0c
    K_O = 0x12
    K_P = 0x13
    K_LBracket = 0x2f # [ {
    K_RBracket = 0x30 # ] }
    K_BSlash   = 0x31 # \ |

    K_Caps = 0x30
    K_A = 0x04
    K_S = 0x16
    K_D = 0x07
    K_F = 0x09
    K_G = 0x0a
    K_H = 0x0b
    K_J = 0x0d
    K_K = 0x0e
    K_L = 0x0f
    K_SemiColon = 0x33 # ; :
    K_Quote	 = 0x34 # ' "
    K_NonUsSign = 0x32 # (Non-US) # ~
    K_Enter     = 0x28 # Enter

    K_LShift = 0xe1
    K_Z = 0x1d
    K_X = 0x1b
    K_C = 0x06
    K_V = 0x19
    K_B = 0x05
    K_N = 0x11
    K_M = 0x10
    K_Comma = 0x36 # , <
    K_Dot = 0x37 # . >
    K_Slash = 0x38 # / ?
    K_Intl1 = 0x87 # (JIS) _ \
    K_RShift = 0xe5

    K_LCtrl = 0xe0
    K_LAlt = 0xe2
    K_Space = 0x2c
    K_RAlt = 0xe6
    K_RCtrl = 0xe4

    K_Insert = 0x49
    K_Home = 0x4a
    K_Delete = 0x4c
    K_End = 0x4d
    K_PageUp = 0x4b
    K_PageDown = 0x4e

    K_PrintScreen = 0x46
    K_ScrollLock = 0x47
    K_Pause = 0x48

    K_LeftArrow = 0x50
    K_UpArrow = 0x52
    K_DownArrow = 0x51
    K_RightArrow = 0x4f

    K_NumLock = 0x53
    K_KPSlash = 0x54 # /
    K_KPStar  = 0x55 # *
    K_KPMinus = 0x56 # -
    K_KPPlus  = 0x57 # +
    K_KPEnter = 0x58 # Enter
    K_KP1 = 0x59 # 1 End
    K_KP2 = 0x5a # 2 Down
    K_KP3 = 0x5b # 3 PageDown
    K_KP4 = 0x5c # 4 Left
    K_KP5 = 0x5d # 5
    K_KP6 = 0x5e # 6 Right
    K_KP7 = 0x5f # 7 Home
    K_KP8 = 0x60 # 8 Up
    K_KP9 = 0x61 # 9 PageUp
    K_KP0 = 0x62 # 0 Insert
    K_KPDOT = 0x63 # . Del

    K_Escape = 0x29
    K_F1 = 0x3a
    K_F2 = 0x3b
    K_F3 = 0x3c
    K_F4 = 0x3d
    K_F5 = 0x3e
    K_F6 = 0x3f
    K_F7 = 0x40
    K_F8 = 0x41
    K_F9 = 0x42
    K_F10 = 0x43
    K_F11 = 0x44
    K_F12 = 0x45

    K_LGui = 0xe3
    K_RGui = 0xe7
    K_App = 0x65

    K_Hiragana = 0x88
    K_Henkan = 0x8a
    K_Muhenkan = 0x8b
    K_Kana = 0x90
    K_Eisu = 0x91

    K_F13 = 0x68
    K_F14 = 0x69
    K_F15 = 0x6a

    M_LCtrl = 1 << 0
    M_LShift = 1 << 1
    M_LAlt = 1 << 2
    M_LGui = 1 << 3
    M_RCtrl = 1 << 4
    M_RShift = 1 << 5
    M_RAlt = 1 << 6
    M_RGui = 1 << 7
end
