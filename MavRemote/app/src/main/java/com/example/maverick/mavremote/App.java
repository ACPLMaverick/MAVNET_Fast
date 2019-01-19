package com.example.maverick.mavremote;

import android.content.Context;
import android.support.annotation.Nullable;
import android.support.v7.app.AppCompatActivity;

import com.example.maverick.mavremote.UI.NotificationHelper;
import com.example.maverick.mavremote.UI.UIController;
import com.example.maverick.mavremote.UI.UIManager;

import java.util.Calendar;
import java.util.concurrent.locks.ReentrantLock;

public abstract class App
{
    protected App()
    {
        _lockApp = new ReentrantLock();
    }

    public static App GetInstance()
    {
        Utility.Assert(_instance != null, "App was not yet created. Need to create it via child class ctor.");

        return _instance;
    }

    public static void Log(final String str)
    {
        if(GetInstance().CanUseUI())
            GetInstance().GetUIController().Log(str);
    }

    public static void LogLine(final String str)
    {
        if(GetInstance().CanUseUI())
            GetInstance().GetUIController().LogLine(str);
    }

    public final void Run(AppCompatActivity activity)
    {
        _activity = activity;
        _context = null;

        AssertActivityType();

        RunCommon();
    }

    public final void Run(Context context)
    {
        _context = context;
        _activity = null;

        RunCommon();
    }

    public final void Run()
    {
        RunCommon();
    }

    public abstract void Stop();

    @Nullable
    public AppCompatActivity GetActivity()
    {
        _lockApp.lock();
        AppCompatActivity activ = _activity;
        _lockApp.unlock();
        return activ;
    }

    public Context GetContext()
    {
        Context ctx;
        _lockApp.lock();
        if(_activity == null)
        {
            Utility.Assert(_context != null);
            ctx = _context;
        }
        else
        {
            Utility.Assert(_context == null);
            ctx = _activity.getApplicationContext();
        }
        _lockApp.unlock();
        return ctx;
    }

    public boolean HasActivity()
    {
        _lockApp.lock();
        final boolean retVal = _activity != null;
        _lockApp.unlock();
        return retVal;
    }

    public boolean CanUseUI()
    {
        return HasActivity();
    }

    public void UpdateActivityState(AppCompatActivity activity, Context context)
    {
        _lockApp.lock();

        _activity = activity;
        _context = context;

        if(_activity != null);
        {
            if(_uiManager == null)
                SetupUIManager();

            if(_uiController == null)
                SetupUIController();
        }

        _lockApp.unlock();
    }

    public UIManager GetUIManager()
    {
        Utility.Assert(CanUseUI());
        return _uiManager;
    }

    public UIController GetUIController()
    {
        Utility.Assert(CanUseUI());
        return _uiController;
    }

    public Calendar GetCalendar()
    {
        if(_calendar == null)
        {
            _calendar = Calendar.getInstance();
        }
        return _calendar;
    }

    private void RunCommon()
    {
        if(CanUseUI())
        {
            if(_uiManager == null)
                SetupUIManager();
            if(_uiController == null)
                SetupUIController();
        }

        _notificationMgr = new NotificationHelper();

        Utility.StartThread(new Runnable()
        {
            @Override
            public void run()
            {
                InternalRun();
            }
        }
        , getClass().getSimpleName(), 8);
    }


    protected void Cleanup()
    {
        _notificationMgr.Cleanup();
        _notificationMgr = null;

        _activity = null;
        _context = null;
        _uiController = null;
        _uiManager = null;
    }

    protected abstract void AssertActivityType();

    protected abstract void InternalRun();

    protected abstract void SetupUIManager();

    protected abstract void SetupUIController();


    public static final String TAG = "MavRemote";

    protected static App _instance = null;

    private AppCompatActivity _activity = null;
    private Context _context = null;

    protected ReentrantLock _lockApp = null;
    protected UIManager _uiManager = null;
    protected UIController _uiController = null;
    protected NotificationHelper _notificationMgr = null;
    protected Calendar _calendar = null;
}
