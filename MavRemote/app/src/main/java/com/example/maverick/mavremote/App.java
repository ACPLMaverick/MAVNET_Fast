package com.example.maverick.mavremote;

import android.support.v7.app.AppCompatActivity;

import com.example.maverick.mavremote.UI.UIManager;

public abstract class App
{
    protected App() { }

    public static App GetInstance()
    {
        Utility.Assert(_instance != null, "App was not yet created. Need to create it via child class ctor.");

        return _instance;
    }

    public final void Run(AppCompatActivity activity)
    {
        _activity = activity;
        AssertActivityType();

        // Cause only main thread can touch its views blah blah blah.
        _uiManager = new UIManager();
        SetupUIManager();

        Utility.StartThread(new Runnable()
        {
            @Override
            public void run()
            {
                InternalRun();
            }
        });
    }

    public AppCompatActivity GetActivity() { return _activity; }

    protected abstract void AssertActivityType();

    protected abstract void InternalRun();

    protected abstract void SetupUIManager();

    public static final String TAG = "MavRemote";

    protected static App _instance = null;

    protected AppCompatActivity _activity = null;
    protected UIManager _uiManager = null;
}
