package com.example.maverick.mavremote.Actions;

import android.view.KeyEvent;

public class ActionEvent
{
    @Override
    public String toString() {
        return "ActionEvent{" +
                "_keyboardMouseEv=" + _keyboardMouseEv +
                ", _movementEv=" + _movementEv +
                ", _delayMillis=" + _delayMillis +
                '}';
    }

    public enum Type
    {
        Keyboard,
        MouseClicks,
        Movement
    }

    public enum MouseClickTypes
    {
        LMBDown,
        LMBUp,
        RMBDown,
        RMBUp
    }

    public enum SpecialKeyEvent
    {
        InvalidValue,
        TaskManager
    }


    public static int GetIntFromSpecialKeyEvent(SpecialKeyEvent keyEvent)
    {
        if(_specialKeyEventBaseValue == -1)
        {
            _specialKeyEventBaseValue = KeyEvent.getMaxKeyCode();
        }

        return _specialKeyEventBaseValue + keyEvent.ordinal();
    }

    // Returns InvalidValue if code is not a special key event.
    public static SpecialKeyEvent GetSpecialKeyEventFromInt(int code)
    {
        if(_specialKeyEventBaseValue == -1)
        {
            _specialKeyEventBaseValue = KeyEvent.getMaxKeyCode();
        }

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


    public ActionEvent
    (
            int keyboardEv,
            int delayMillis
    )
    {
        Init(keyboardEv, null, delayMillis, false);
    }

    public ActionEvent
            (
                    int keyboardEv
            )
    {
        Init(keyboardEv, null, 0, false);
    }

    public ActionEvent
            (
                    MouseClickTypes mouseEv,
                    int delayMillis
            )
    {
        Init(mouseEv.ordinal(), null, delayMillis, true);
    }

    public ActionEvent
            (
                    MouseClickTypes mouseEv
            )
    {
        Init(mouseEv.ordinal(), null, 0, true);
    }

    public ActionEvent
            (
                    Movement movementEv,
                    int delayMillis
            )
    {
        Init(KEYBOARD_EV_UNUSED, movementEv, delayMillis, false);
    }

    public ActionEvent
            (
                    Movement movementEv
            )
    {
        Init(KEYBOARD_EV_UNUSED, movementEv, 0, false);
    }

    public int GetKeyboardEv()
    {
        return _keyboardMouseEv;
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

    public int GetDelayMillis()
    {
        return _delayMillis;
    }

    public Type GetType()
    {
        if(_keyboardMouseEv != KEYBOARD_EV_UNUSED && _movementEv == null)
        {
            if(_keyboardMouseEv < 0)
            {
                return Type.MouseClicks;
            }
            else
            {
                return Type.Keyboard;
            }
        }
        else if(_keyboardMouseEv == KEYBOARD_EV_UNUSED && _movementEv != null)
        {
            return Type.Movement;
        }

        assert(false); // Cant get here.

        return Type.Keyboard;
    }


    private void Init(int keyboardMouseEv, Movement movement, int delayMillis, boolean isMouseEv)
    {
        if(isMouseEv)
        {
            SetMouseEv(keyboardMouseEv);
        }
        else
        {
            _keyboardMouseEv = keyboardMouseEv;
        }
        _movementEv = movement;
        _delayMillis = delayMillis;
    }

    private void SetMouseEv(int value)
    {
        ++value;
        _keyboardMouseEv = ~value;
        assert(_keyboardMouseEv < 0);
    }



    private static final int KEYBOARD_EV_UNUSED = 0xFADEBEEF;

    private static int _specialKeyEventBaseValue = -1;


    private int _keyboardMouseEv = KEYBOARD_EV_UNUSED;
    private Movement _movementEv = null;
    private int _delayMillis = 0;
}
