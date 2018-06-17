package com.example.maverick.mavremote.Client;

import com.example.maverick.mavremote.Utility;

public class AppClient
{
    public static AppClient GetInstance()
    {
        if(_instance == null)
            _instance = new AppClient();

        return _instance;
    }

    public void Run()
    {
        Utility.StartThread(new Runnable()
        {
            @Override
            public void run()
            {
                InternalRun();
            }
        });
    }


    private void InternalRun()
    {
        InternalStart();
        InternalMainLoop();
        InternalFinish();
    }

    private void InternalStart()
    {
        _bIsRunning = true;
    }

    private void InternalMainLoop()
    {
        while(_bIsRunning)
        {

        }
    }

    private void InternalFinish()
    {

    }


    private static AppClient _instance = null;

    private boolean _bIsRunning = false;
}
