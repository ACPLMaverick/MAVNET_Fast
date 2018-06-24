package com.example.maverick.mavremote.Actions;

public class Movement
{
    @Override
    public String toString() {
        return "Movement{" +
                "_x=" + _x +
                ", _y=" + _y +
                '}';
    }

    // For scroll, please pass the scroll value as Y. In this case, X is used to store magic value,
    // indicating that this movement is a scroll.
    public Movement(int x, int y, boolean scroll)
    {
        if(scroll)
        {
            _x = SCROLL_MAGIC_X;
        }
        else
        {
            _x = x;
        }
        _y = y;
    }

    public int GetX()
    {
        return _x;
    }

    public int GetY()
    {
        return _y;
    }

    public boolean IsScroll()
    {
        return _x == SCROLL_MAGIC_X;
    }

    private static final int SCROLL_MAGIC_X = 0xDEADBEEF;

    private int _x;
    private int _y;
}
