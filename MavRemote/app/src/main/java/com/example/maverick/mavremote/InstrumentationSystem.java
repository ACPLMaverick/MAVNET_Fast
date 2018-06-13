package com.example.maverick.mavremote;

import android.util.Log;

import java.io.DataOutput;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.LinkedList;
import java.util.Queue;
import java.util.concurrent.Semaphore;
import java.util.concurrent.locks.ReentrantLock;

public final class InstrumentationSystem extends System
{
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
        _queue = new LinkedList<>();
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
            ActionEvent ev = _queue.pop();
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

        final String command = "input keyevent " + String.valueOf(ev.KeyboardEv);
        try
        {
            _shellStream.writeBytes(command + "\n");
            _shellStream.flush();
        }
        catch(IOException e)
        {
            Log.e(App.TAG, "Failed to send input keyevent: " + e.getMessage() + ". Closing root shell...");
            CloseRootShell();
            return;
        }

        LogRootShellOutput();
    }

    private void InitRootShell()
    {
        assert(_shellProc == null);

        try
        {
            _shellProc = Runtime.getRuntime().exec(new String[] { "su", "-c", "system/bin/sh"});
//            _shellProc = Runtime.getRuntime().exec("sh");
        }
        catch(IOException e)
        {
            Log.e(App.TAG, "Failed to initialize root shell: " + e.getMessage());
            return;
        }

        // ++test
        /*
        try
        {
            _shellProc.waitFor();
        }
        catch(InterruptedException e)
        {
        }
        */
        // --test

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

        try
        {
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

        Log.d(App.TAG, out);
    }


    private LinkedList<ActionEvent> _queue = null;
    private ReentrantLock _lock = null;

    private Process _shellProc = null;
    private DataOutputStream _shellStream = null;
}
