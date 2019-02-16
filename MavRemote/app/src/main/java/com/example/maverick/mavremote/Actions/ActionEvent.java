package com.example.maverick.mavremote.Actions;

import android.view.KeyEvent;

import com.example.maverick.mavremote.Utility;

import java.io.Serializable;

public class ActionEvent implements Serializable
{
    @Override
    public String toString() {
        return "ActionEvent{" +
                "_keyboardMouseEv=" + GetKeyboardEv() +
                ", _movementEv=" + _movementEv +
                ", _text=" + _text +
                '}';
    }

    public enum Type
    {
        KeyClick,
        KeyDown,
        KeyUp,
        MouseClicks,
        Movement,
        Text
    }

    public enum MouseClickTypes
    {
        LMBClick,
        RMBClick,
        LMBDown,
        LMBUp,
        RMBDown,
        RMBUp
    }

    public enum SpecialKeyEvent
    {
        InvalidValue,
        TaskManager,
        Statistics,
        Subtitles
    }


    public static int GetIntFromSpecialKeyEvent(SpecialKeyEvent keyEvent)
    {
        return _specialKeyEventBaseValue + keyEvent.ordinal();
    }

    // Returns InvalidValue if code is not a special key event.
    public static SpecialKeyEvent GetSpecialKeyEventFromInt(int code)
    {
        if(code < _specialKeyEventBaseValue || code >= _specialKeyEventBaseValue + SpecialKeyEvent.values().length)
        {
            return SpecialKeyEvent.InvalidValue;
        }
        else
        {
            code = code - _specialKeyEventBaseValue;
            return SpecialKeyEvent.values()[code];
        }
    }

    public static boolean IsKeyboardEvent(Type evType)
    {
        return evType == Type.KeyClick || evType == Type.KeyDown || evType == Type.KeyUp;
    }


    public ActionEvent
    (
            Type keyboardEvType,
            int keyboardEv
    )
    {
        Init(keyboardEv, keyboardEvType, null, false);
    }

    public ActionEvent
            (
                    int keyboardEv
            )
    {
        Init(keyboardEv, null, null, false);
    }

    public ActionEvent
            (
                    MouseClickTypes mouseEv
            )
    {
        Init(mouseEv.ordinal(), null, null, true);
    }

    public ActionEvent
            (
                    Movement movementEv
            )
    {
        Init(KEYBOARD_EV_UNUSED, null, movementEv, false);
    }

    public ActionEvent(final String text)
    {
        _text = text;
    }

    public int GetKeyboardEv()
    {
        return _keyboardMouseEv & 0x0000FFFF;
    }

    public MouseClickTypes GetMouseEv()
    {
        int value = ~_keyboardMouseEv;
        --value;
        assert(value < MouseClickTypes.values().length);
        return MouseClickTypes.values()[value];
    }

    public Movement GetMovementEv()
    {
        return _movementEv;
    }

    public String GetText()
    {
        return _text;
    }

    public Type ResolveType()
    {
        if(!_text.equals(""))
        {
            return Type.Text;
        }

        if(_keyboardMouseEv != KEYBOARD_EV_UNUSED && _movementEv == null)
        {
            if(_keyboardMouseEv < 0)
            {
                return Type.MouseClicks;
            }
            else
            {
                return GetKeyboardEventType();
            }
        }
        else if(_keyboardMouseEv == KEYBOARD_EV_UNUSED && _movementEv != null)
        {
            return Type.Movement;
        }

        Utility.Assert(false); // Cant get here.

        return Type.KeyClick;
    }


    private void Init(int keyboardMouseEv, Type keyboardEvType, Movement movement, boolean isMouseEv)
    {
        if(isMouseEv)
        {
            SetMouseEv(keyboardMouseEv);
        }
        else if(keyboardMouseEv != KEYBOARD_EV_UNUSED)
        {
            Utility.Assert((keyboardMouseEv & 0xFFFF0000) == 0);
            _keyboardMouseEv = keyboardMouseEv;
            if(keyboardEvType == null)
            {
                keyboardEvType = Type.KeyClick;
            }
            final int masked = keyboardEvType.ordinal() << 16;
            _keyboardMouseEv |= masked;
        }
        _movementEv = movement;
    }

    private void SetMouseEv(int value)
    {
        ++value;
        _keyboardMouseEv = ~value;
        Utility.Assert(_keyboardMouseEv < 0);
    }

    private ActionEvent.Type GetKeyboardEventType()
    {
        final int masked = _keyboardMouseEv >>> 16;
        Utility.Assert(masked >= Type.KeyClick.ordinal() && masked <= Type.KeyUp.ordinal());
        return Type.values()[masked];
    }


    private static final int KEYBOARD_EV_UNUSED = 0xFADEBEEF;

    private static int _specialKeyEventBaseValue = 0x0000FFFF - SpecialKeyEvent.values().length;


    private int _keyboardMouseEv = KEYBOARD_EV_UNUSED;
    private Movement _movementEv = null;
    private String _text = "";
}
