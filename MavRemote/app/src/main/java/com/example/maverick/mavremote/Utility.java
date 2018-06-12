package com.example.maverick.mavremote;

import android.util.Log;

import java.util.concurrent.Callable;

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
            Log.e(App.TAG, "[Utility] InterruptedException: " + e.getMessage());
            return false;
        }

        return true;
    }

    public static void StartThread(final Runnable func)
    {
        final Thread t = new Thread()
        {
            @Override
            public void run()
            {
                func.run();
            }
        };
        t.start();
    }
}
