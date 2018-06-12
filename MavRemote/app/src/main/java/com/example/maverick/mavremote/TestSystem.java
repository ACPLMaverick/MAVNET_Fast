package com.example.maverick.mavremote;

import android.view.KeyEvent;

public final class TestSystem extends System
{
    protected void Start()
    {

    }

    protected void Finish()
    {

    }

    @Override
    protected void MainLoop()
    {
        RunTest01();
        RunTest02();
    }

    private void RunTest01()
    {
        App.GetInstance().GetInstrumentationSystem().EnqueueActionEvent(new ActionEvent(KeyEvent.KEYCODE_DPAD_LEFT));
        Utility.SleepThread(500);
        App.GetInstance().GetInstrumentationSystem().EnqueueActionEvent(new ActionEvent(KeyEvent.KEYCODE_DPAD_RIGHT));
        Utility.SleepThread(500);
        App.GetInstance().GetInstrumentationSystem().EnqueueActionEvent(new ActionEvent(KeyEvent.KEYCODE_DPAD_RIGHT));
        Utility.SleepThread(500);
        App.GetInstance().GetInstrumentationSystem().EnqueueActionEvent(new ActionEvent(KeyEvent.KEYCODE_DPAD_LEFT));
        Utility.SleepThread(500);
        App.GetInstance().GetInstrumentationSystem().EnqueueActionEvent(new ActionEvent(KeyEvent.KEYCODE_DPAD_DOWN));
    }

    private void RunTest02()
    {
        App.GetInstance().GetInstrumentationSystem().EnqueueActionEvent(new ActionEvent(KeyEvent.KEYCODE_BACK));

        // TODO: moving while app out of context.
    }
}
