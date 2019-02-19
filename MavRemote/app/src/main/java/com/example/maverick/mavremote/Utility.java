package com.example.maverick.mavremote;

import android.content.Context;
import android.os.Build;
import android.os.PowerManager;
import android.os.VibrationEffect;
import android.os.Vibrator;
import android.util.Log;

import com.example.maverick.mavremote.Server.AppServer;

import java.net.SocketAddress;
import java.nio.charset.Charset;

public final class Utility
{
    public static boolean SleepThread(long millis)
    {
        if(millis == 0)
        {
            return true;
        }

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

    public static boolean SleepThreadUs(int us)
    {
        if(us == 0)
        {
            return true;
        }

        try
        {
            final int millis = us / 1000;
            final int ns = 1000 * (us % 1000);
            Thread.sleep(millis, ns);
        }
        catch(InterruptedException e)
        {
            Log.e(AppServer.TAG, "[Utility] InterruptedException: " + e.getMessage());
            return false;
        }

        return true;
    }

    public static boolean SleepThreadNs(int ns)
    {
        if(ns == 0)
        {
            return true;
        }

        try
        {
            final int millis = ns / 1000000;
            final int nsD = ns % 1000000;
            Thread.sleep(millis, nsD);
        }
        catch(InterruptedException e)
        {
            Log.e(AppServer.TAG, "[Utility] InterruptedException: " + e.getMessage());
            return false;
        }

        return true;
    }

    public static Thread StartThread(final Runnable func, String name)
    {
        final Thread t = new Thread(func, name);
        t.start();
        return t;
    }

    public static Thread StartThread(final Runnable func, String name, int priority)
    {
        final Thread t = new Thread(func, name);
        t.setPriority(priority);
        t.start();
        return t;
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

    public static byte[] StringToBytes(final String str)
    {
        // two letters == one byte.
        // Android is big endian so set bytes in that order.

        String tmpStr = str.toUpperCase();

        Utility.Assert(str.length() % 2 == 0);
        final int bytesLength = str.length() / 2;
        byte[] bytes = new byte[bytesLength];
        byte[] strBytes = tmpStr.getBytes(Charset.forName("UTF-8"));
        byte[] charsThisByte = new byte[2];
        for(int i = 0; i < bytesLength; ++i)
        {
            charsThisByte[0] = strBytes[2 * i];
            charsThisByte[1] = strBytes[2 * i + 1];

            for(int j = 0; j < 2; ++j)
            {
                if(charsThisByte[j] <= '9')
                {
                    charsThisByte[j] -= 48;
                }
                else
                {
                    charsThisByte[j] -= 55;
                }
            }

            bytes[i] = Integer.valueOf(charsThisByte[0] << 4 | charsThisByte[1]).byteValue();
        }

        return bytes;
    }

    public static void Vibrate(long vibrationTimeMs)
    {
        if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.O)
        {
            Vibrate(vibrationTimeMs, VibrationEffect.DEFAULT_AMPLITUDE);
        }
        else
        {
            Vibrate(vibrationTimeMs, -1);
        }
    }

    public static void Vibrate(long vibrationTimeMs, int amplitude)
    {
        try
        {
            Vibrator vibrator = (Vibrator) App.GetInstance().GetActivity().getSystemService(Context.VIBRATOR_SERVICE);
            if(vibrator == null)
            {
                return;
            }

            if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.O)
            {
                vibrator.vibrate(VibrationEffect.createOneShot(vibrationTimeMs, amplitude));
            }
            else
            {
                vibrator.vibrate(vibrationTimeMs);
            }
        }
        catch (Exception e)
        {
            App.LogLine("Error creating vibration for " + String.valueOf(vibrationTimeMs) + " ms.");
        }
    }

    public static boolean IsScreenOn()
    {
        PowerManager powerManager = (PowerManager)App.GetInstance().GetActivity().getSystemService(Context.POWER_SERVICE);
        if(powerManager == null)
        {
            return true;
        }

        return powerManager.isScreenOn();
    }
}
