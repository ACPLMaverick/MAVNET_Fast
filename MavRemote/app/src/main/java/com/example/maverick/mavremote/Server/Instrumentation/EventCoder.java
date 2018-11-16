package com.example.maverick.mavremote.Server.Instrumentation;

import com.example.maverick.mavremote.Actions.ActionEvent;
import com.example.maverick.mavremote.Actions.Movement;
import com.example.maverick.mavremote.Utility;

import java.util.List;

class EventCoder
{
    enum MouseEventType
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


    private static final String[][] _baseCodesMouse =
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

    public void KeyEventToCodes(int keyEvent, List<String> outCodes)
    {

    }

    public void MouseTypeToCodes(MouseEventType type, final String codePrefix, List<String> outCodes)
    {
        assert(type.ordinal() < _baseCodesMouse.length - 1);

        outCodes.clear();
        String[] codes = _baseCodesMouse[type.ordinal()];
        for(int i = 0; i < codes.length; ++i)
            outCodes.add(codePrefix + codes[i]);

        if(type != MouseEventType.Commit) // Adding commit after commit is unnecessary.
            outCodes.add(codePrefix + _baseCodesMouse[MouseEventType.Commit.ordinal()][0]);
    }

    public void MouseTypeToCodes(ActionEvent.MouseClickTypes clickType, final String codePrefix, List<String > outCodes)
    {
        MouseTypeToCodes(MouseClickActionEventToCoder(clickType), codePrefix, outCodes);
    }

    public void MovementToCodes(final Movement movement, final String codePrefix, List<String> outCodes)
    {
        if(movement.IsScroll())
        {
            if(movement.GetY() != 0)
            {
                if(movement.GetY() > 0)
                {
                    GetCodesForDirection(MouseEventType.ScrollUp, movement.GetY(), codePrefix, outCodes);
                }
                else
                {
                    GetCodesForDirection(MouseEventType.ScrollDown, movement.GetY(), codePrefix, outCodes);
                }
            }
        }
        else
        {
            if(movement.GetX() != 0)
            {
                if(movement.GetX() > 0)
                {
                    GetCodesForDirection(MouseEventType.Right, movement.GetX(), codePrefix, outCodes);
                }
                else
                {
                    GetCodesForDirection(MouseEventType.Left, movement.GetX(), codePrefix, outCodes);
                }
            }

            if(movement.GetY() != 0)
            {
                if(movement.GetY() > 0)
                {
                    GetCodesForDirection(MouseEventType.Down, movement.GetY(), codePrefix, outCodes);
                }
                else
                {
                    GetCodesForDirection(MouseEventType.Up, movement.GetY(), codePrefix, outCodes);
                }
            }
        }
    }


    private MouseEventType MouseClickActionEventToCoder(ActionEvent.MouseClickTypes type)
    {
        final int shift = MouseEventType.values().length - MouseEventType.ButtonLeftDown.ordinal();
        assert(ActionEvent.MouseClickTypes.values().length < MouseEventType.values().length - shift);
        return MouseEventType.values()[type.ordinal() + shift];
    }

    private void GetCodesForDirection(MouseEventType dirType, int value, final String codePrefix, List<String> outCodes)
    {
        String movementCode = _baseCodesMouse[dirType.ordinal()][0];

        movementCode += Integer.toString(value);

        outCodes.add(codePrefix + movementCode);
        outCodes.add(codePrefix + _baseCodesMouse[MouseEventType.Commit.ordinal()][0]);
    }
}
