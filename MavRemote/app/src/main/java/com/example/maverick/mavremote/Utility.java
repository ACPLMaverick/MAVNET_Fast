package com.example.maverick.mavremote;

import android.util.Log;

import com.example.maverick.mavremote.Server.AppServer;

import java.net.SocketAddress;
import java.nio.charset.Charset;

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
}
