package com.example.maverick.mavremote.Actions;

import java.io.Serializable;

public class Movement implements Serializable
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
    public Movement(int x, int y)
    {
        _x = x;
        _y = y;
    }

    public Movement(int scroll)
    {
        _x = SCROLL_MAGIC_X;
        _y = scroll;
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

    protected static final int SCROLL_MAGIC_X = 0xDEADBEEF;

    protected int _x;
    protected int _y;
}
