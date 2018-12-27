package com.example.maverick.mavremote.Server.Instrumentation;

import com.example.maverick.mavremote.App;

import org.jetbrains.annotations.NotNull;

public class SendEventWrapper
{
    // Used to load the 'native-lib' library on application startup.
    static {
        java.lang.System.loadLibrary("SendEvent");
    }

    public native boolean SendEventInitialize();
    public native boolean SendEventCleanup();
    public native boolean SendEventSendInputEvent(int strType,
                                                  int strCode,
                                                  int strValue);

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

    public void SendInputEvent(@NotNull final InputDeviceEvent kbEvent)
    {
        final boolean ret = SendEventSendInputEvent(kbEvent.GetEvType(), kbEvent.GetEvCode(), kbEvent.GetEvValue());
        if(!ret)
        {
            App.LogLine("Failed to SetInputEvent!");
        }
    }
}
