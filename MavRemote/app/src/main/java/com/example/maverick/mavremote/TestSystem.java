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
        RunTest03();
    }

    private void RunTest01()
    {
        Utility.SleepThread(5000);
        Log.d(App.TAG, "Test 01 started...");
        final int delayTime = 500;
        
        if(_version == 1)
        {
            InstrumentationSystem.Enqueue(new ActionEvent(KeyEvent.KEYCODE_DPAD_LEFT));
            Utility.SleepThread(delayTime);
            InstrumentationSystem.Enqueue(new ActionEvent(KeyEvent.KEYCODE_DPAD_RIGHT));
            Utility.SleepThread(delayTime);
            InstrumentationSystem.Enqueue(new ActionEvent(KeyEvent.KEYCODE_DPAD_RIGHT));
            Utility.SleepThread(delayTime);
            InstrumentationSystem.Enqueue(new ActionEvent(KeyEvent.KEYCODE_DPAD_LEFT));
            Utility.SleepThread(delayTime);
            InstrumentationSystem.Enqueue(new ActionEvent(KeyEvent.KEYCODE_DPAD_DOWN));
            Utility.SleepThread(delayTime);
        }
        else if(_version == 2)
        {
            InstrumentationSystem.Enqueue(new ActionEvent(KeyEvent.KEYCODE_DPAD_LEFT, delayTime));
            InstrumentationSystem.Enqueue(new ActionEvent(KeyEvent.KEYCODE_DPAD_RIGHT, delayTime));
            InstrumentationSystem.Enqueue(new ActionEvent(KeyEvent.KEYCODE_DPAD_RIGHT, delayTime));
            InstrumentationSystem.Enqueue(new ActionEvent(KeyEvent.KEYCODE_DPAD_LEFT, delayTime));
            InstrumentationSystem.Enqueue(new ActionEvent(KeyEvent.KEYCODE_DPAD_DOWN, delayTime));
        }
    }

    private void RunTest02()
    {
        if(_version == 1)
        {
            InstrumentationSystem.Enqueue(new ActionEvent(KeyEvent.KEYCODE_BACK));
            Utility.SleepThread(1500);
            InstrumentationSystem.Enqueue(new ActionEvent(KeyEvent.KEYCODE_HOME));
            Utility.SleepThread(500);
        }
        else if(_version == 2)
        {
            InstrumentationSystem.Enqueue(new ActionEvent(KeyEvent.KEYCODE_BACK, 1500));
            InstrumentationSystem.Enqueue(new ActionEvent(KeyEvent.KEYCODE_HOME, 500));
        }
    }

    private void RunTest03()
    {
        final int sleepTime = 1250;

        if(_version == 1)
        {
            Utility.SleepThread(sleepTime);
            InstrumentationSystem.Enqueue(new ActionEvent(KeyEvent.KEYCODE_DPAD_DOWN));
            Utility.SleepThread(sleepTime);
            InstrumentationSystem.Enqueue(new ActionEvent(KeyEvent.KEYCODE_DPAD_DOWN));
            Utility.SleepThread(sleepTime);
            InstrumentationSystem.Enqueue(new ActionEvent(KeyEvent.KEYCODE_DPAD_DOWN));
            Utility.SleepThread(sleepTime);
            InstrumentationSystem.Enqueue(new ActionEvent(KeyEvent.KEYCODE_DPAD_RIGHT));
            Utility.SleepThread(sleepTime);
            InstrumentationSystem.Enqueue(new ActionEvent(KeyEvent.KEYCODE_DPAD_DOWN));
            Utility.SleepThread(sleepTime);
            InstrumentationSystem.Enqueue(new ActionEvent(KeyEvent.KEYCODE_ENTER));
            Utility.SleepThread(1500);
        }
        else if(_version == 2)
        {
            Utility.SleepThread(sleepTime);
            InstrumentationSystem.Enqueue(new ActionEvent(KeyEvent.KEYCODE_DPAD_DOWN, sleepTime));
            InstrumentationSystem.Enqueue(new ActionEvent(KeyEvent.KEYCODE_DPAD_DOWN, sleepTime));
            InstrumentationSystem.Enqueue(new ActionEvent(KeyEvent.KEYCODE_DPAD_DOWN, sleepTime));
            InstrumentationSystem.Enqueue(new ActionEvent(KeyEvent.KEYCODE_DPAD_RIGHT, sleepTime));
            InstrumentationSystem.Enqueue(new ActionEvent(KeyEvent.KEYCODE_DPAD_DOWN, sleepTime));
            InstrumentationSystem.Enqueue(new ActionEvent(KeyEvent.KEYCODE_ENTER, 1500));
        }

        for(int i = 0; i < 12; ++i)
        {
            InstrumentationSystem.Enqueue(new ActionEvent(KeyEvent.KEYCODE_DPAD_DOWN));
        }
    }

    private static final int _version = 2;
}
