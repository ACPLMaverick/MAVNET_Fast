package com.example.maverick.mavremote;

import android.util.Log;

import com.example.maverick.mavremote.Server.AppServer;

import java.net.SocketAddress;

public final class Utility
{
    public static boolean SleepThread(long millis)
    {
        try
        {
            Thread.sleep(millis);
        }
        catch(InterruptedException e)
        {
            Log.e(AppServer.TAG, "[Utility] InterruptedException: " + e.getMessage());
            return false;
        }

        return true;
    }

    public static void StartThread(final Runnable func, String name)
    {
        final Thread t = new Thread(func, name);
        t.start();
    }

    public static void StartThread(final Runnable func, String name, int priority)
    {
        final Thread t = new Thread(func, name);
        t.setPriority(priority);
        t.start();
    }

    public static void Assert(boolean evaluatedValue)
    {
        Assert(evaluatedValue, "Assertion error!");
    }

    public static void Assert(boolean evaluatedValue, String message)
    {
        if(!evaluatedValue)
        {
            throw new AssertionError(message);
        }
    }
}
