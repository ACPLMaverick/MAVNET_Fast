package com.example.maverick.mavremote;

import android.util.Log;
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
        Utility.SleepThread(5000);
        Log.d(App.TAG, "Test 01 started...");
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
