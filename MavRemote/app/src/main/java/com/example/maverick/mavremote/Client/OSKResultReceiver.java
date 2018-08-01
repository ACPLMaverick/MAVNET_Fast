package com.example.maverick.mavremote.Client;

import android.os.Bundle;
import android.os.Handler;
import android.os.ResultReceiver;

public class OSKResultReceiver extends ResultReceiver
{
    public OSKResultReceiver()
    {
        super(null);
    }

    public int GetResult()
    {
        return _result;
    }

    public boolean IsResultAvailable()
    {
        return _result != -1;
    }

    @Override
    protected void onReceiveResult(int resultCode, Bundle resultData)
    {
        super.onReceiveResult(resultCode, resultData);

        _result = resultCode;
    }

    private int _result = -1;
}
