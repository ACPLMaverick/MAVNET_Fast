package com.example.maverick.mavremote.Server;

import android.support.v7.app.AppCompatActivity;

import com.example.maverick.mavremote.App;
import com.example.maverick.mavremote.Server.Instrumentation.InstrumentationSystem;
import com.example.maverick.mavremote.ServerActivity;
import com.example.maverick.mavremote.UI.UIManager;
import com.example.maverick.mavremote.Utility;

public final class AppServer extends App
{
    private AppServer() { }

    public static AppServer GetInstance()
    {
        if(_instance == null)
            _instance = new AppServer();

        return (AppServer)_instance;
    }

    public ServerActivity GetActivityTyped() { return (ServerActivity)_activity; }

    public InstrumentationSystem GetInstrumentationSystem() { assert(_instr != null); return _instr; }

    public InfraredSystem GetInfraredSystem() { assert(_infr != null); return _infr; }


    @Override
    protected void AssertActivityType()
    {
        Utility.Assert(_activity != null && _activity instanceof ServerActivity);
    }

    @Override
    protected void InternalRun()
    {
        Start();
        MainLoop();
        Finish();
    }

    @Override
    protected void SetupUIManager()
    {
        _uiManager.InitMenu(UIManager.MenuType.ServerNetwork);
        _uiManager.SetMenuCurrent(UIManager.MenuType.ServerNetwork);
    }

    private void Start()
    {
        _instr = new InstrumentationSystem();
        Utility.StartThread(new Runnable()
        {
            @Override
            public void run()
            {
                _instr.Run();
            }
        });

        if(B_USE_INFRARED)
        {
            _infr = new InfraredSystem();
            Utility.StartThread(new Runnable()
            {
                @Override
                public void run()
                {
                    _infr.Run();
                }
            });
        }
        else
        {
            _server = new ServerNetworkSystem();
            Utility.StartThread(new Runnable()
            {
                @Override
                public void run()
                {
                    _server.Run();
                }
            });
        }

        // Startup instrumentation tests!
//        _tester = new TestSystem();
//        Utility.StartThread(new Runnable()
//        {
//            @Override
//            public void run()
//            {
//                _tester.Run();
//            }
//        });

        _isRunning = true;
    }

    private void Finish()
    {
        _isRunning = false;
    }

    private void MainLoop()
    {
        while(_isRunning)
        {
            //Log.d(AppServer.TAG, "Debug message, hay!");
            Utility.SleepThread(1000);
        }
    }

    private static final boolean B_USE_INFRARED = false;

    private InstrumentationSystem _instr = null;
    private InfraredSystem _infr = null;
    private ServerNetworkSystem _server;
    private TestSystem _tester = null;
    private boolean _isRunning = false;
}
