package com.example.maverick.mavremote.Server.Instrumentation;

import com.example.maverick.mavremote.App;

public class SendEventWrapper
{
    // Used to load the 'native-lib' library on application startup.
    static {
        java.lang.System.loadLibrary("SendEvent");
    }

    public native boolean SendEventInitialize();
    public native boolean SendEventCleanup();
    public native boolean SendEventSendInputEvent(int deviceIndex,
                                                  char[] strType,
                                                  char[] strCode,
                                                  char[] strValue);
    public native String SendEventGetDeviceName(int deviceIndex);

    public enum DeviceIndex
    {
        Keyboard,
        Mouse,
        NUM
    }

    public SendEventWrapper()
    {
    }

    public void Initialize()
    {
        final boolean ret = SendEventInitialize();
        if(!ret)
        {
            App.LogLine("Failed to initialize SendEvent!");
        }
    }

    public void Cleanup()
    {
        final boolean ret = SendEventCleanup();
        if(!ret)
        {
            App.LogLine("Failed to cleanup SendEvent!");
        }
    }

    public void SendInputEvent(final DeviceIndex index,
                               final String strType, final String strCode, final String strValue)
    {
        int devIndex = index.ordinal();
        char[] sStrType = strType.toCharArray();
        char[] sStrCode = strCode.toCharArray();
        char[] sStrValue = strValue.toCharArray();

        final boolean ret = SendEventSendInputEvent(devIndex, sStrType, sStrCode, sStrValue);
        if(!ret)
        {
            App.LogLine("Failed to SetInputEvent!");
        }
    }

    public String GetDeviceName(DeviceIndex index)
    {
        return SendEventGetDeviceName(index.ordinal());
    }
}
