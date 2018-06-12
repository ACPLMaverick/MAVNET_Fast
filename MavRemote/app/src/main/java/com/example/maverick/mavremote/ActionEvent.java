package com.example.maverick.mavremote;

import android.view.KeyEvent;

public class ActionEvent
{
    public ActionEvent(
            int keyboardEv
    )
    {
        KeyboardEv = keyboardEv;
    }

    @Override
    public String toString()
    {
        return "ActionEvent{" +
                "KeyboardEv=" + KeyboardEv +
                '}';
    }

    public int KeyboardEv;

}
