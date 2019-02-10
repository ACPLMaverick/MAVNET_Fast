package com.example.maverick.mavremote.Server.Instrumentation;

import com.example.maverick.mavremote.App;

import org.jetbrains.annotations.NotNull;

public class SendEventWrapper
{
    // Used to load the 'native-lib' library on application startup.
    static {
        java.lang.System.loadLibrary("SendEvent");
    }

    public native boolean SendEventInitialize(int[][] jKeyCodes, int[] jMouseCodes,
                                              int[] jMouseCodeTypes, int jBadCode);
    public native boolean SendEventCleanup();
    public native boolean SendEventSendInputEvent(int devType,
                                                  int strType,
                                                  int strCode,
                                                  int strValue);

    public enum DeviceType
    {
        Keyboard,
        Mouse,
        NUM
    }

    public SendEventWrapper()
    {
    }

    public void Initialize(int[][] keyCodes, int[] mouseCodes, int[] mouseCodeTypes, int badCode)
    {
        final boolean ret = SendEventInitialize(keyCodes, mouseCodes, mouseCodeTypes, badCode);
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

    public void SendInputEvent(final DeviceType deviceType, @NotNull final InputDeviceEvent kbEvent)
    {
        final boolean ret = SendEventSendInputEvent(deviceType.ordinal(), kbEvent.GetEvType(),
                kbEvent.GetEvCode(), kbEvent.GetEvValue());
        if(!ret)
        {
            App.LogLine("Failed to SetInputEvent!");
        }
    }
}
