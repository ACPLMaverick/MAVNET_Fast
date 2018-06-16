package com.example.maverick.mavremote;

import android.util.Log;

import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayDeque;
import java.util.concurrent.locks.ReentrantLock;

public final class InstrumentationSystem extends System
{
    public static void Enqueue(ActionEvent ev)
    {
        App.GetInstance().GetInstrumentationSystem().EnqueueActionEvent(ev);
    }

    public void EnqueueActionEvent(ActionEvent ev)
    {
        assert (_lock != null);

        _lock.lock();
        _queue.push(ev);
        _lock.unlock();
    }


    @Override
    protected void Start()
    {
        _queue = new ArrayDeque<>();
        _lock = new ReentrantLock();

        InitRootShell();
    }

    @Override
    protected void Finish()
    {
        CloseRootShell();
    }

    @Override
    protected void MainLoop()
    {
        while(true)
        {
            if(_queue.isEmpty())
            {
                Thread.yield();
                continue;
            }

            _lock.lock();
            ActionEvent ev = _queue.pollLast();
            _lock.unlock();
            PerformActionEvent(ev);

            Thread.yield();
        }
    }


    private void PerformActionEvent(ActionEvent ev)
    {
        if(_shellProc == null)
        {
            Log.w(App.TAG, "Cannot perform action event because shell was closed due to an error.");
            return;
        }

        Log.d(App.TAG, "Performing action event: " + ev.toString());

        assert(_shellStream != null);

        final String command;

        if(ev.GetType() == ActionEvent.Type.Keyboard)
        {
            command = "input keyevent " + String.valueOf(ev.GetKeyboardEv());
        }
        else if(ev.GetType() == ActionEvent.Type.Movement)
        {
            if(ev.GetMovementEv().IsSwipe())
            {
                // TODO: implement.
            }
            else
            {
                // TODO: implement.
            }

            assert(false);
            command = "";
        }
        else
        {
            // Shouldn't be able to get here.
            assert(false);
            command = "";
        }

        try
        {
            _shellStream.writeBytes(command + "\n");
        }
        catch(IOException e)
        {
            Log.e(App.TAG, "Failed to send input keyevent: " + e.getMessage() + ". Closing root shell...");
            CloseRootShell();
            return;
        }

        if(ev.GetDelayMillis() > 0)
        {
            Utility.SleepThread(ev.GetDelayMillis());
        }

        LogRootShellOutput();
    }

    private void InitRootShell()
    {
        assert(_shellProc == null);

        try
        {
//            _shellProc = Runtime.getRuntime().exec(new String[] { "su", "-c", "system/bin/sh"});
            _shellProc = Runtime.getRuntime().exec("su");
        }
        catch(IOException e)
        {
            Log.e(App.TAG, "Failed to initialize root shell: " + e.getMessage());
            return;
        }

        _shellStream = new DataOutputStream(_shellProc.getOutputStream());

        LogRootShellOutput();
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
            Log.e(App.TAG, "Failed to close root shell: " + e.getMessage());
            return;
        }

        try
        {
            _shellProc.waitFor();
            _shellProc = null;
        }
        catch(InterruptedException e)
        {
            Log.e(App.TAG, "Failed to close root shell: " + e.getMessage());
            return;
        }
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
            Log.e(App.TAG, "Failed to close log shell output: " + e.getMessage());
            return;
        }

        if(!out.isEmpty())
        {
            Log.d(App.TAG, out);
        }
    }

    private static final int LOG_OUTPUT_WAIT_FOR_SHELL_MILLIS = 250;

    private ArrayDeque<ActionEvent> _queue = null;
    private ReentrantLock _lock = null;

    private Process _shellProc = null;
    private DataOutputStream _shellStream = null;
}
