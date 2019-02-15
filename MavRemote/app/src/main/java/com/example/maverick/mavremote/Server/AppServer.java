package com.example.maverick.mavremote.Server;

import android.util.Log;

import com.example.maverick.mavremote.Actions.ActionEvent;
import com.example.maverick.mavremote.App;
import com.example.maverick.mavremote.NetworkCommon.NetworkSystem;
import com.example.maverick.mavremote.Server.Instrumentation.InstrumentationSystem;
import com.example.maverick.mavremote.ServerActivity;
import com.example.maverick.mavremote.UI.NotificationHelper;
import com.example.maverick.mavremote.UI.UIManager;
import com.example.maverick.mavremote.Utility;

import java.util.Calendar;
import java.util.concurrent.atomic.AtomicBoolean;

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

    public boolean IsRunning() { return _bIsRunning.get(); }

    public void SetForceConnect(boolean bForceConnect)
    {
        _bForceConnect = bForceConnect;
    }

    public void SetBackPressed(boolean val)
    {
        _lockApp.lock();
        _bBackPressed = val;
        _lockApp.unlock();
    }

    public boolean GetBackPressed()
    {
        _lockApp.lock();
        final boolean bBackPressed = _bBackPressed;
        _lockApp.unlock();
        return bBackPressed;
    }


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
        _bIsRunning.set(false);
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
        }
        , "InstrumentationSystem", 7);

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
            }
            , "InfraredSystem", 5);
            _notificationMgr.DisplayNotificationText("Using IR Remote as client...");
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
            }
            , "ServerNetworkSystem", 5);
            _cachedServerState = _server.GetState();
            _notificationMgr.DisplayNotificationText(_cachedServerState.name());

            if(B_AUTO_START)
            {
                _uiControllerServer.DeactivateStartStopButton();
            }
            else
            {
                _uiControllerServer.RegisterOnConnectionClick(new Runnable()
                {
                    @Override
                    public void run()
                    {
                        _bChangeConnectionState = !_bChangeConnectionState;
                    }
                });
            }
        }

        _cachedServerState = _server.GetState();

        // ++notification test
//        _notificationMgr.DisplayNotificationText("Server running...");
//        appstart = App.GetCurrentTimeMs();
        // --notification test

        _bIsRunning.set(true);

        // ++instrumentation test
        if(B_TESTER)
        {
            _tester = new TestSystem();
            Utility.StartThread(new Runnable()
            {
                @Override
                public void run()
                {
                    _tester.Run();
                }
            }, "Tester");
        }
        // --instrumentation test
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
        while(_bIsRunning.get())
        {
            // ++notification test
//            if(App.GetCurrentTimeMs() - timer > 5000)
//            {
//                timer = App.GetCurrentTimeMs();
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

            if(CanUseUI())
                ProcessUI();

            Utility.SleepThread(1);
        }
    }

    private void ProcessQueueInfrared()
    {
        // TODO: Implement.
    }

    private void ProcessQueueServer()
    {
        if(!_server.IsRunning() || !_instr.IsRunning() || !_server.HasActionEvents())
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


        if(_bForceConnect && _server.GetState() == NetworkSystem.State.NotConnectedIdle)
        {
            _bChangeConnectionState = true;
        }


        if(_bChangeConnectionState)
        {
            if(_server.GetState() == NetworkSystem.State.NotConnectedIdle)
            {
                _server.StartAwaitingConnections();
            }
            else
            {
                _server.Disconnect();
            }

            _bChangeConnectionState = false;
        }

        if(B_AUTO_START)
        {
            if(_server.GetState() == NetworkSystem.State.NotConnectedIdle)
            {
                _bChangeConnectionState = !_bChangeConnectionState;
            }
        }
    }

    private void ProcessUI()
    {
        _uiControllerServer.UpdateCurrentNetworkState(_server.GetState());
        _uiControllerServer.UpdateCurrentMyAddress(_server.GetMyAddress());
        _uiControllerServer.UpdateCurrentClientAddress(_server.GetConnectedAddress());
        _uiControllerServer.UpdateReceived(_server.GetPacketCounter());

        if(GetBackPressed())
        {
            final NotificationHelper.MessageState msgState = _notificationMgr.CheckMessageStateAndCleanup();
            if(msgState == NotificationHelper.MessageState.None)
            {
                final boolean retVal = _notificationMgr.DisplayMessageTwoResponses(
                        "Do you really want to shutdown server?");
                if(!retVal)
                {
                    App.LogLine("WARNING: Failed to create pop-up message!");
                    App.LogLine("Exiting.");
                    _bIsRunning.set(false);
                    SetBackPressed(false);
                }
            }
            else if(msgState == NotificationHelper.MessageState.Positive)
            {
                App.LogLine("Exiting.");
                _bIsRunning.set(false);
                SetBackPressed(false);
            }
            else if(msgState == NotificationHelper.MessageState.Negative)
            {
                App.LogLine("Keeping running.");
                SetBackPressed(false);
            }
        }
    }

    private static final boolean B_USE_INFRARED = false;
    private static final boolean B_AUTO_START = true;
    private static final boolean B_TESTER = false;

    private InstrumentationSystem _instr = null;
    private InfraredSystem _infr = null;
    private ServerNetworkSystem _server;
    private NetworkSystem.State _cachedServerState = null;
    private TestSystem _tester = null;
    private ServerUIController _uiControllerServer = null;
    private AtomicBoolean _bIsRunning = new AtomicBoolean(false);
    private boolean _bForceConnect = false;
    private boolean _bChangeConnectionState = false;
    private boolean _bBackPressed = false;

    // ++notification test
//    private long appstart = 0;
//    private long timer = 0;
    // --notification test
}
