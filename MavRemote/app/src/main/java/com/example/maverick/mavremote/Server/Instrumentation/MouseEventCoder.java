package com.example.maverick.mavremote.Server.Instrumentation;

import com.example.maverick.mavremote.Server.Actions.ActionEvent;
import com.example.maverick.mavremote.Server.Actions.Movement;

import java.util.List;

class MouseEventCoder
{
    enum Type
    {
        Up,
        Down,
        Left,
        Right,
        ScrollUp,
        ScrollDown,
        ButtonLeftDown,
        ButtonLeftUp,
        ButtonRightDown,
        ButtonRightUp,
        Commit,
        Unknown
    }


    private static final String[][] _baseCodeArray =
    {
            { "0002 0001 " },
            { "0002 0001 " },
            { "0002 0000 " },
            { "0002 0000 " },
            { "0002 0008 " },
            { "0002 0008 " },
            { "0004 0004 589825", "0001 272 00000001" },
            { "0004 0004 589825", "0001 272 00000000" },
            { "0004 0004 589826", "0001 273 00000001" },
            { "0004 0004 589826", "0001 273 00000000" },
            { "0000 0000 000000" }
    };


    /*
    Screen coordinates for Android:

    (0, 0)                  (MaxX, 0)



    (0, MaxY)               (MaxX, MaxY)
     */


    public void TypeToCodes(Type type, final String codePrefix, List<String> outCodes)
    {
        assert(type.ordinal() < _baseCodeArray.length - 1);

        outCodes.clear();
        String[] codes = _baseCodeArray[type.ordinal()];
        for(int i = 0; i < codes.length; ++i)
            outCodes.add(codePrefix + codes[i]);

        if(type != Type.Commit) // Adding commit after commit is unnecessary.
            outCodes.add(codePrefix + _baseCodeArray[Type.Commit.ordinal()][0]);
    }

    public void TypeToCodes(ActionEvent.MouseClickTypes clickType, final String codePrefix, List<String > outCodes)
    {
        TypeToCodes(MouseClickActionEventToCoder(clickType), codePrefix, outCodes);
    }

    public void MovementToCodes(final Movement movement, final String codePrefix, List<String> outCodes)
    {
        if(movement.IsScroll())
        {
            if(movement.GetY() != 0)
            {
                if(movement.GetY() > 0)
                {
                    GetCodesForDirection(Type.ScrollUp, movement.GetY(), codePrefix, outCodes);
                }
                else
                {
                    GetCodesForDirection(Type.ScrollDown, movement.GetY(), codePrefix, outCodes);
                }
            }
        }
        else
        {
            if(movement.GetX() != 0)
            {
                if(movement.GetX() > 0)
                {
                    GetCodesForDirection(Type.Right, movement.GetX(), codePrefix, outCodes);
                }
                else
                {
                    GetCodesForDirection(Type.Left, movement.GetX(), codePrefix, outCodes);
                }
            }

            if(movement.GetY() != 0)
            {
                if(movement.GetY() > 0)
                {
                    GetCodesForDirection(Type.Down, movement.GetY(), codePrefix, outCodes);
                }
                else
                {
                    GetCodesForDirection(Type.Up, movement.GetY(), codePrefix, outCodes);
                }
            }
        }
    }


    private Type MouseClickActionEventToCoder(ActionEvent.MouseClickTypes type)
    {
        final int shift = Type.values().length - Type.ButtonLeftDown.ordinal();
        assert(ActionEvent.MouseClickTypes.values().length < Type.values().length - shift);
        return Type.values()[type.ordinal() + shift];
    }

    private void GetCodesForDirection(Type dirType, int value, final String codePrefix, List<String> outCodes)
    {
        String movementCode = _baseCodeArray[dirType.ordinal()][0];

        movementCode += Integer.toString(value);

        outCodes.add(codePrefix + movementCode);
        outCodes.add(codePrefix + _baseCodeArray[Type.Commit.ordinal()][0]);
    }
}
