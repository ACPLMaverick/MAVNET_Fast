package com.example.maverick.mavremote.Server.Instrumentation;

import android.app.Instrumentation;
import android.os.SystemClock;
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

        _eventCoder = new EventCoder();

        _bShellCreated = InitRootShell();
        if(!_bShellCreated)
        {
            App.LogLine("InstrumentationSystem: Failed to create root shell. Instrumentation will not work.");
            return;
        }

        _sendEventWrapper = new SendEventWrapper();
        MakeDeviceWritable();
        _sendEventWrapper.Initialize(_eventCoder.GetKeycodes(), _eventCoder.GetMouseCodes(),
                _eventCoder.GetMouseCodeTypes(), _eventCoder.GetBadCode());

        _movementHelper = new MouseMovementHelper();
        _movementHelper.Start(_sendEventWrapper, _eventCoder);
    }

    @Override
    protected void Finish()
    {
        _movementHelper.Stop();
        _sendEventWrapper.Cleanup();

        CloseRootShell();
    }

    @Override
    protected void MainLoop()
    {
        if(!_bShellCreated)
            return;

        if(_queue.IsEmpty())
        {
//            Thread.yield();
            return;
        }

        ActionEvent ev = _queue.Dequeue();
        while(ev != null)
        {
            PerformActionEvent(ev);
            ev = _queue.Dequeue();
        }
    }


    // Returns: How many ms of delay was performed.
    private void PerformActionEvent(ActionEvent ev)
    {
        if(_shellProc == null)
        {
            App.LogLine("Cannot perform action event because shell was closed due to an error.");
            return;
        }

        //App.LogLine("Performing action event: " + ev.toString());

        assert(_shellStream != null);

        int millisWaited = 0;

        final ActionEvent.Type evType = ev.ResolveType();

        long now = SystemClock.uptimeMillis();

        if(evType == ActionEvent.Type.Text)
        {
            // Can use input for such matters, because it doesn't require speed.
            ExecuteRootShellCommand("input text \"" + ev.GetText() + "\"");
        }
        else if(evType == ActionEvent.Type.Movement && !ev.GetMovementEv().IsScroll())
        {
            _movementHelper.UpdateMouseOffset(ev.GetMovementEv());
        }
        else
        {
            // Use event coder for any other case.
            ArrayList<InputDeviceEvent> deviceEvents = new ArrayList<>();
            final boolean bCoded = _eventCoder.ActionEventToCodes(ev, evType, deviceEvents);
            if(bCoded)
            {
                final SendEventWrapper.DeviceType deviceType =
                        evType == ActionEvent.Type.Keyboard
                                ? SendEventWrapper.DeviceType.Keyboard : SendEventWrapper.DeviceType.Mouse;
                for(InputDeviceEvent inputDeviceEvent : deviceEvents)
                {
                    _sendEventWrapper.SendInputEvent(deviceType, inputDeviceEvent);
                    millisWaited = PerformDelay();    // Delay after each sendinputevent.
                }
            }
            else
            {
                // If coder has failed, resort to the legacy slower methods.
                if(evType == ActionEvent.Type.Keyboard)
                {
                    ExecuteRootShellCommand("input keyevent " + String.valueOf(ev.GetKeyboardEv()));
                }
                else
                {
                    App.LogLine("Instrumentation couldn't resolve an ActionEvent!");
                }
            }
        }

        // TODO: What to do with this?
        final int millisToWait = ev.GetDelayMillis() - millisWaited;
        if(millisToWait > 0)
        {
            Utility.SleepThread(millisToWait);
        }
    }

    private int PerformDelay()
    {
        final int timeToWait = WAIT_AFTER_EACH_EVENT_MICROS;
        if(timeToWait > 0)
            Utility.SleepThreadUs(timeToWait);
        return timeToWait;
    }

    private boolean InitRootShell()
    {
        assert(_shellProc == null);

        try
        {
            _shellProc = Runtime.getRuntime().exec(new String[] { "su", "-c", "system/bin/sh"});
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

    private void MakeDeviceWritable()
    {
        final String command = "chmod 666 /dev/uinput";
        ExecuteRootShellCommand(command);

        Utility.SleepThread(1500); // Make sure the commands have executed.
    }

    private void LogRootShellOutput()
    {
        final int bufferLength = 1024;
        InputStream stdout = _shellProc.getInputStream();
        byte[] buffer = new byte[bufferLength];
        int read = 0;
        String out = "";

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


    // NEW implementation, more low-level.
    private void WriteBytesToDevice(DataOutputStream device, final byte[] bytes)
    {
        try
        {
            device.write(bytes);
        }
        catch(IOException e)
        {
            App.LogLine("Error writing to the device: " + e.getMessage());
        }
    }

    private static final int LOG_OUTPUT_WAIT_FOR_SHELL_MILLIS = 250;
    private static final int ROOT_SHELL_OUTPUT_BUFFER_SIZE_TO_FLUSH = 2048;
    private static final int WAIT_AFTER_EACH_EVENT_MICROS = 0;

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

    private EventCoder _eventCoder = null;
    private MouseMovementHelper _movementHelper = null;
    private SendEventWrapper _sendEventWrapper = null;

    private boolean _bShellCreated = false;
}
