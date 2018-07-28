package com.example.maverick.mavremote.Server;

import com.example.maverick.mavremote.Actions.ActionEvent;
import com.example.maverick.mavremote.App;
import com.example.maverick.mavremote.NetworkCommon.NetworkSystem;
import com.example.maverick.mavremote.Server.Instrumentation.InstrumentationSystem;
import com.example.maverick.mavremote.ServerActivity;
import com.example.maverick.mavremote.UI.UIManager;
import com.example.maverick.mavremote.Utility;

import java.util.Calendar;

public final class AppServer extends App
{
    private AppServer() { }

    public static AppServer GetInstance()
    {
        if(_instance == null)
            _instance = new AppServer();

        return (AppServer)_instance;
    }


    public InstrumentationSystem GetInstrumentationSystem() { return _instr; }

    public InfraredSystem GetInfraredSystem() { return _infr; }

    public boolean IsRunning() { return _bIsRunning; }


    @Override
    protected void AssertActivityType()
    {
        Utility.Assert((GetActivity() != null && GetActivity() instanceof ServerActivity)
                        || !HasActivity());
    }

    @Override
    protected void InternalRun()
    {
        Start();
        MainLoop();
        Finish();

        Cleanup();
    }

    @Override
    protected void SetupUIManager()
    {
        _uiManager = new UIManager();
        _uiManager.InitMenu(UIManager.MenuType.ServerNetwork);
        _uiManager.SetMenuCurrent(UIManager.MenuType.ServerNetwork);
    }

    @Override
    protected void SetupUIController()
    {
        _uiController = new ServerUIController();
        _uiControllerServer = (ServerUIController)_uiController;
        _uiControllerServer.Init(_uiManager);
    }

    @Override
    public void Stop()
    {
        _bIsRunning = false;
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

        _cachedServerState = _server.GetState();

        // ++notification test
//        _notificationMgr.DisplayNotificationText("Server running...");
//        appstart = Calendar.getInstance().getTimeInMillis();
        // --notification test

        if(_server != null)
        {
            _notificationMgr.DisplayNotificationText(_cachedServerState.name());
        }
        else
        {
            _notificationMgr.DisplayNotificationText("Using IR Remote as client...");
        }

        _bIsRunning = true;
    }

    private void Finish()
    {
        _instr.Stop();
        if(B_USE_INFRARED)
        {
            _infr.Stop();
        }
        else
        {
            _server.Stop();
        }

        if(_tester != null)
        {
            _tester.Stop();
        }

        if(_uiController != null)
            _uiController.Cleanup();

        if(HasActivity())
            GetActivity().finish();
    }

    private void MainLoop()
    {
        while(_bIsRunning)
        {
            // ++notification test
//            if(Calendar.getInstance().getTimeInMillis() - timer > 5000)
//            {
//                timer = Calendar.getInstance().getTimeInMillis();
//                long delta = timer - appstart;
//
//                _notificationMgr.DisplayNotificationText("App running: " + Long.toString(delta));
//            }
            // --notification test
            if(CanUseUI())
                _uiController.Update();

            if(B_USE_INFRARED)
            {
                ProcessQueueInfrared();
            }
            else
            {
                ProcessQueueServer();
                ProcessServerState();
            }
        }
    }

    private void ProcessQueueInfrared()
    {
        // TODO: Implement.
    }

    private void ProcessQueueServer()
    {
        if(!_server.IsRunning() || !_instr.IsRunning())
            return;

        ActionEvent ev = _server.PopActionEvent();
        while(ev != null)
        {
            _instr.EnqueueActionEvent(ev);
            ev = _server.PopActionEvent();
        }
    }

    private void ProcessServerState()
    {
        if(_cachedServerState != _server.GetState())
        {
            _cachedServerState = _server.GetState();
            _notificationMgr.DisplayNotificationText(_cachedServerState.name());
        }
    }

    private static final boolean B_USE_INFRARED = false;

    private InstrumentationSystem _instr = null;
    private InfraredSystem _infr = null;
    private ServerNetworkSystem _server;
    private NetworkSystem.State _cachedServerState = null;
    private TestSystem _tester = null;
    private ServerUIController _uiControllerServer = null;
    private boolean _bIsRunning = false;

    // ++notification test
//    private long appstart = 0;
//    private long timer = 0;
    // --notification test
}
