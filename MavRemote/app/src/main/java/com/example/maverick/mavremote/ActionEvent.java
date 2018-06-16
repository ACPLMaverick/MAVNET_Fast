package com.example.maverick.mavremote;

public class ActionEvent
{
    public enum Type
    {
        Keyboard,
        Movement,
        ENUM_SIZE
    }

    public class Movement
    {
        public Movement(int x, int y, boolean swipe)
        {
            _x = x;
            _y = y;
            _swipe = swipe;
        }

        public int GetX()
        {
            return _x;
        }

        public int GetY()
        {
            return _y;
        }

        public boolean IsSwipe()
        {
            return _swipe;
        }

        private int _x;
        private int _y;
        private boolean _swipe;
    }


    public ActionEvent
    (
            int keyboardEv,
            int delayMillis
    )
    {
        _keyboardEv = keyboardEv;
        _delayMillis = delayMillis;
    }

    public ActionEvent
            (
                    Movement movementEv,
                    int delayMillis
            )
    {
        _movementEv = movementEv;
        _delayMillis = delayMillis;
    }

    public ActionEvent
            (
                    int keyboardEv
            )
    {
        _keyboardEv = keyboardEv;
        _delayMillis = 0;
    }

    public ActionEvent
            (
                    Movement movementEv
            )
    {
        _movementEv = movementEv;
        _delayMillis = 0;
    }

    public int GetKeyboardEv()
    {
        return _keyboardEv;
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
        if(_keyboardEv != -1 && _movementEv == null)
        {
            return Type.Keyboard;
        }
        else if(_keyboardEv == -1 && _movementEv != null)
        {
            return Type.Movement;
        }

        assert(false); // Cant get here.

        return Type.Keyboard;
    }

    private int _keyboardEv = -1;
    private Movement _movementEv = null;
    private int _delayMillis = 0;
}
