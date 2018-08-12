package com.example.maverick.mavremote.Server.Instrumentation;

import android.util.Log;

import com.example.maverick.mavremote.Actions.ActionEvent;
import com.example.maverick.mavremote.App;
import com.example.maverick.mavremote.EventQueue;
import com.example.maverick.mavremote.Server.AppServer;
import com.example.maverick.mavremote.System;
import com.example.maverick.mavremote.Utility;

import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;

public final class InstrumentationSystem extends System
{
    public static void Enqueue(ActionEvent ev)
    {
        AppServer.GetInstance().GetInstrumentationSystem().EnqueueActionEvent(ev);
    }


    public void EnqueueActionEvent(ActionEvent ev)
    {
        _queue.Enqueue(ev);
    }

    public boolean IsShellCreated() {
        return _bShellCreated;
    }

    @Override
    protected void Start()
    {
        _queue = new EventQueue<>();
        _queue.Init();

        _mouseEventCoder = new MouseEventCoder();

        _bShellCreated = InitRootShell();
        if(!_bShellCreated)
        {
            App.LogLine("InstrumentationSystem: Failed to create root shell. Instrumentation will not work.");
            return;
        }
        RetrieveMouseDeviceName();
        MakeMouseDeviceBufferWritable();
    }

    @Override
    protected void Finish()
    {
        CloseRootShell();
    }

    @Override
    protected void MainLoop()
    {
        if(!_bShellCreated)
            return;

        if(_queue.IsEmpty())
        {
            Thread.yield();
            return;
        }

        ActionEvent ev = _queue.Dequeue();
        while(ev != null)
        {
            PerformActionEvent(ev);
            ev = _queue.Dequeue();
        }
    }


    private void PerformActionEvent(ActionEvent ev)
    {
        if(_shellProc == null)
        {
            App.LogLine("Cannot perform action event because shell was closed due to an error.");
            return;
        }

        App.LogLine("Performing action event: " + ev.toString());

        assert(_shellStream != null);

        ArrayList<String> commands = new ArrayList<>();

        final ActionEvent.Type evType = ev.ResolveType();

        if(evType == ActionEvent.Type.Text)
        {
            commands.add("input text \"" + ev.GetText() + "\"");
        }
        else if(evType == ActionEvent.Type.Keyboard)
        {
            commands.add("input keyevent " + String.valueOf(ev.GetKeyboardEv()));
        }
        else if(evType == ActionEvent.Type.MouseClicks)
        {
            _mouseEventCoder.TypeToCodes(ev.GetMouseEv(), "sendevent " + _mouseDeviceName + " " , commands);
        }
        else if(evType == ActionEvent.Type.Movement)
        {
            _mouseEventCoder.MovementToCodes(ev.GetMovementEv(), "sendevent " + _mouseDeviceName + " " , commands);
        }
        else
        {
            // Shouldn't be able to get here.
            Utility.Assert(false);
        }

        for(String command : commands)
        {
            ExecuteRootShellCommand(command);
        }

        if(ev.GetDelayMillis() > 0)
        {
            Utility.SleepThread(ev.GetDelayMillis());
        }
    }

    private boolean InitRootShell()
    {
        assert(_shellProc == null);

        try
        {
            _shellProc = Runtime.getRuntime().exec(new String[] { "su", "-c", "system/bin/sh"});
//            _shellProc = Runtime.getRuntime().exec("su");
        }
        catch(IOException e)
        {
            Log.e(AppServer.TAG, "Failed to initialize root shell: " + e.getMessage());
            return false;
        }

        _shellStream = new DataOutputStream(_shellProc.getOutputStream());

        return true;
    }

    private void CloseRootShell()
    {
        assert (_shellProc != null);

        try
        {
            _shellStream.writeBytes("exit \n");
            _shellStream.close();
            _shellStream = null;
        }
        catch(IOException e)
        {
            Log.e(AppServer.TAG, "Failed to close root shell: " + e.getMessage());
            return;
        }

        try
        {
            _shellProc.waitFor();
            _shellProc = null;
        }
        catch(InterruptedException e)
        {
            Log.e(AppServer.TAG, "Failed to close root shell: " + e.getMessage());
            return;
        }
    }

    private void ExecuteRootShellCommand(String command)
    {
        try
        {
            App.LogLine("Sending command: " + command);
            _shellStream.writeChars(command + "\n");
        }
        catch(IOException e)
        {
            Log.e(AppServer.TAG, "Failed to send input keyevent: " + e.getMessage() + ". Closing root shell...");
            CloseRootShell();
            return;
        }
    }

    private void MakeMouseDeviceBufferWritable()
    {
        final String command = "chmod 666 " + _mouseDeviceName;
        ExecuteRootShellCommand(command);
        LogRootShellOutput();
    }

    private void RetrieveMouseDeviceName()
    {
        // TODO: Obtain this based on getevent command.
        _mouseDeviceName = "/dev/input/event0"; // vmouse
    }

    private void LogRootShellOutput()
    {
        final int bufferLength = 1024;
        InputStream stdout = _shellProc.getInputStream();
        byte[] buffer = new byte[bufferLength];
        int read = 0;
        String out = new String();

//        Utility.SleepThread(LOG_OUTPUT_WAIT_FOR_SHELL_MILLIS);
        // Do not wait because this introduces unwanted delay and possibly desync.

        try
        {
            if(stdout.available() == 0)
            {
                return;
            }

            while(true)
            {
                read = stdout.read(buffer, 0, bufferLength);
                if(read == -1)
                    break;
                out += new String(buffer, 0, read);
                if(read < bufferLength)
                {
                    // We have read everything.
                    break;
                }
            }
        }
        catch(IOException e)
        {
            Log.e(AppServer.TAG, "Failed to close log shell output: " + e.getMessage());
            return;
        }

        if(!out.isEmpty())
        {
            App.LogLine(out);
        }
    }

    private static final int LOG_OUTPUT_WAIT_FOR_SHELL_MILLIS = 250;
    private static final int ROOT_SHELL_OUTPUT_BUFFER_SIZE_TO_FLUSH = 2048;

    /*
        add device 1: /dev/input/event3
          name:     "USB Optical Mouse USB Optical Mouse"
        could not get driver version for /dev/input/mouse1, Not a typewriter
        add device 2: /dev/input/event4
          name:     "sun7i-ir"
        add device 3: /dev/input/event2
          name:     "axp20-supplyer"
        add device 4: /dev/input/event1
          name:     "sw-keyboard"
        could not get driver version for /dev/input/mouse0, Not a typewriter
        add device 5: /dev/input/event0
          name:     "vmouse"
        could not get driver version for /dev/input/mice, Not a typewriter
    */

    private EventQueue<ActionEvent> _queue = null;

    private Process _shellProc = null;
    private DataOutputStream _shellStream = null;

    private MouseEventCoder _mouseEventCoder = null;

    private String _mouseDeviceName = "unknown ";

    private boolean _bShellCreated = false;
}
