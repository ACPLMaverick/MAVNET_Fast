package com.example.maverick.mavremote.Server;

import android.os.Environment;
import android.util.Log;
import android.view.KeyEvent;

import com.example.maverick.mavremote.Actions.ActionEvent;
import com.example.maverick.mavremote.Actions.Movement;
import com.example.maverick.mavremote.App;
import com.example.maverick.mavremote.Server.Instrumentation.InstrumentationSystem;
import com.example.maverick.mavremote.System;
import com.example.maverick.mavremote.Utility;

import java.io.DataOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

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
        Utility.SleepThread(5000);

//        RunTestFile();

//        RunTestKb01();
//        RunTestKb02();
        //RunTestKb03();

//        RunTestMouseMinus01();
//        RunTestMouse00();
        RunTestMouse01();
//        RunTestMouse02();

        Stop();
    }

    private void RunTestFile()
    {
        final String path = Environment.getExternalStorageDirectory().getAbsolutePath() + "/Download/test/fileTest.bin";
        DataOutputStream stream = null;

        try
        {
            File file = new File(path);
            boolean ret = file.createNewFile();
            ret = file.canWrite();

            ret = false;
        }
        catch(IOException e)
        {
            App.LogLine("Dupa");
            return;
        }

        try
        {
            stream = new DataOutputStream(new FileOutputStream(path, false));
        }
        catch(FileNotFoundException e)
        {
            App.LogLine("Dupa");
            return;
        }

        try
        {
            final byte[] bytes = Utility.StringToBytes("DEADBEEF1234");

            stream.write(bytes);
            stream.close();
        }
        catch(IOException e)
        {
            App.LogLine("Dupa");
            return;
        }

        App.LogLine("File written properly.");
    }

    private void RunTestKb01()
    {
        Log.d(AppServer.TAG, "Test 01 started...");
        final int delayTime = 500;

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

    private void RunTestKb02()
    {
        App.LogLine("Test 02 started...");

//        InstrumentationSystem.Enqueue(new ActionEvent(KeyEvent.KEYCODE_HOME));
        InstrumentationSystem.Enqueue(new ActionEvent(KeyEvent.KEYCODE_BACK));
        Utility.SleepThread(2500);
        InstrumentationSystem.Enqueue(new ActionEvent(KeyEvent.KEYCODE_DPAD_RIGHT));
        Utility.SleepThread(500);
        InstrumentationSystem.Enqueue(new ActionEvent(KeyEvent.KEYCODE_ENTER));
        Utility.SleepThread(500);
    }

    private void RunTestKb03()
    {
        final int sleepTime = 1250;

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

        for(int i = 0; i < 12; ++i)
        {
            InstrumentationSystem.Enqueue(new ActionEvent(KeyEvent.KEYCODE_DPAD_DOWN));
        }

        InstrumentationSystem.Enqueue(new ActionEvent(KeyEvent.KEYCODE_BACK));
        InstrumentationSystem.Enqueue(new ActionEvent(ActionEvent.MouseClickTypes.LMBDown));
        InstrumentationSystem.Enqueue(new ActionEvent(ActionEvent.MouseClickTypes.LMBUp));
    }

    private void RunTestMouseMinus01()
    {
        InstrumentationSystem.Enqueue(new ActionEvent(new Movement(50, 150)));
        Utility.SleepThread(1000);
        InstrumentationSystem.Enqueue(new ActionEvent(new Movement(-550, 550)));
    }

    private void RunTestMouse00()
    {
        final int diagMulX = 2;
        final int diagMovement = 250;
        final int diagDiv = 25;
        final int diagMovePerLoop = diagMovement / diagDiv;

        final boolean multiple = false;

        if(multiple)
        {
            for(int i = 0; i < diagDiv; ++i)
            {
                InstrumentationSystem.Enqueue(new ActionEvent(new Movement(diagMulX * diagMovePerLoop, diagMovePerLoop)));
            }
        }
        else
        {
            InstrumentationSystem.Enqueue(new ActionEvent(new Movement(diagMulX * diagMovement, diagMovement)));
        }
    }

    private void RunTestMouse01()
    {
        while(true)
        {
            final double movementAngleRadians = Math.toRadians(12);
            final int movementRadius = 50;
            final int movementNum = 30;
            final int movementDelay = 100;
            double currentAngle = 0;

            for (int i = 0; i < movementNum; ++i)
            {
                currentAngle += movementAngleRadians;

                int deltaX = (int) (movementRadius * Math.cos(currentAngle));
                int deltaY = (int) (movementRadius * Math.sin(currentAngle));

                InstrumentationSystem.Enqueue(new ActionEvent(new Movement(deltaX, deltaY)));
                Utility.SleepThread(movementDelay);
            }
        }
    }

    private void RunTestMouse02()
    {
        InstrumentationSystem.Enqueue(new ActionEvent(KeyEvent.KEYCODE_BACK));
        Utility.SleepThread(1500);
        final int oneMovementLength = 5;
        final int oneSideMovementCount = 25;
        final int movementDelay = 100;

        for(int i = 0; i < 4; ++i)
        {
            int x, y;
            if(i == 0)
            {
                x = oneMovementLength;
                y = 0;
            }
            else if(i == 1)
            {
                x = 0;
                y = -oneMovementLength;
            }
            else if(i == 2)
            {
                x = -oneMovementLength;
                y = 0;
            }
            else
            {
                x = 0;
                y = oneMovementLength;
            }

            for(int j = 0; j < oneSideMovementCount; ++j)
            {
                InstrumentationSystem.Enqueue(new ActionEvent(new Movement(x, y)));
            }
        }
    }
}
