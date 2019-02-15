package com.example.maverick.mavremote.Client;

import com.example.maverick.mavremote.Actions.ActionEvent;
import com.example.maverick.mavremote.App;
import com.example.maverick.mavremote.ClientActivity;
import com.example.maverick.mavremote.NetworkCommon.NetworkSystem;
import com.example.maverick.mavremote.UI.NotificationHelper;
import com.example.maverick.mavremote.UI.UIManager;
import com.example.maverick.mavremote.Utility;

import java.net.InetSocketAddress;
import java.net.SocketAddress;
import java.util.concurrent.atomic.AtomicBoolean;

public final class AppClient extends App
{
    private AppClient() { }

    public static AppClient GetInstance()
    {
        if(_instance == null)
        {
            _instance = new AppClient();
        }
        return (AppClient)_instance;
    }


    public ClientActivity GetActivityTyped()
    {
        Utility.Assert(HasActivity());
        return (ClientActivity)GetActivity();
    }

    public ClientNetworkSystem GetNetworkSystem() { return _networkSystem; }

    public InputSystem GetInputSystem() { return _inputSystem; }

    public void OnBackButtonPressed() { _bBackPressed = true; }

    public boolean IsRunning() { return _bIsRunning.get(); }


    @Override
    protected void AssertActivityType()
    {
        Utility.Assert(GetActivity() != null && GetActivity() instanceof ClientActivity);
    }

    @Override
    protected void InternalRun()
    {
        InternalStart();
        InternalMainLoop();
        InternalFinish();

        Cleanup();
    }

    @Override
    protected void SetupUIManager()
    {
        _uiManager = new UIManager();
        _uiManager.InitMenu(UIManager.MenuType.ClientNetwork);
        _uiManager.InitMenu(UIManager.MenuType.ClientRemote);
        _uiManager.SetMenuCurrent(UIManager.MenuType.ClientNetwork);
    }

    @Override
    protected void SetupUIController()
    {
        _uiController = new ClientUIController();
        _uiControllerClient = (ClientUIController)_uiController;
        _uiControllerClient.Init(_uiManager);
        _uiControllerClient.RegisterOnConnectionClick(new Runnable()
        {
            @Override
            public void run()
            {
                _bTryingConnect = true;
            }
        });
    }

    @Override
    public void Stop()
    {
        _bIsRunning.set(false);
    }

    private void InternalStart()
    {
        // Client MUST use UI.
        Utility.Assert(CanUseUI());

        _inputSystem = new InputSystem();
        Utility.StartThread(new Runnable()
        {
            @Override
            public void run()
            {
                _inputSystem.Run();
            }
        }
        , "InputSystem");

        _networkSystem = new ClientNetworkSystem();
        Utility.StartThread(new Runnable()
        {
            @Override
            public void run()
            {
                _networkSystem.Run();
            }
        }
        , "NetworkSystem");

        _notificationMgr.DisplayNotificationText("Touch to return to the remote.");

        App.LogLine("[AppClient] Started.");

        if(B_TEST_MODE)
        {
            _uiManager.SetMenuCurrent(UIManager.MenuType.ClientRemote);
        }

        _bIsRunning.set(true);
    }

    private void InternalMainLoop()
    {
        while(_bIsRunning.get())
        {
            if(CanUseUI())
                _uiController.Update();

            if(_bTryingConnect)
            {
                TryConnect();
            }

            ProcessUI();

            ProcessQueue();

            Utility.SleepThread(1);
        }
    }

    private void InternalFinish()
    {
        if(_uiController != null)
            _uiController.Cleanup();
        _inputSystem.Stop();
        _networkSystem.Stop();

        if(HasActivity())
            GetActivity().finish();
    }

    private void TryConnect()
    {
        if(_networkSystem.GetState() == NetworkSystem.State.NotConnectedIdle)
        {
            boolean ret = _networkSystem.StartAwaitingConnections();
            if(ret)
            {
                _uiControllerClient.SetConnectionButtonAvailability(false);
            }
            else
            {
                App.LogLine("Device is not connected to local network.");
                _bTryingConnect = false;
                _bIsConnected = false;
            }
        }
        else if(_networkSystem.GetState() == NetworkSystem.State.WaitingForConnection)
        {

        }
        else if(_networkSystem.GetState() == NetworkSystem.State.Connected)
        {
            _bTryingConnect = false;
            _bIsConnected = true;
        }
    }

    private void ProcessQueue()
    {
        ActionEvent ev = _inputSystem.PopEvent();
        while(ev != null)
        {
            _networkSystem.PushActionEvent(ev);
            ev = _inputSystem.PopEvent();
        }
    }

    private void ProcessUI()
    {
        if(!CanUseUI())
            return;

        if(_bIsConnected && _networkSystem.GetState() != NetworkSystem.State.Connected)
        {
            BackUIToDisconnected();
        }


        _uiControllerClient.NetworkUpdateConnectionStatus(_networkSystem.GetState());
        SocketAddress addr = null;
        if(_networkSystem.GetState() == NetworkSystem.State.Connected
                && (addr = _networkSystem.GetConnectedAddress()) != null)
        {
            _uiControllerClient.NetworkUpdateConnectionAddress(addr);
        }
        else
        {
            _uiControllerClient.NetworkUpdateConnectionAddress(new InetSocketAddress("0.0.0.0", 0));
        }


        if(_bIsConnected
                && _uiManager.GetCurrentMenu().GetMyType() != UIManager.MenuType.ClientRemote)
        {
            Utility.SleepThread(1000);
            _uiManager.SetMenuCurrent(UIManager.MenuType.ClientRemote);
        }

        if(_bBackPressed)
        {
            ProcessBackPressed();
        }
    }

    private void BackUIToDisconnected()
    {
        _bIsConnected = false;
        _bTryingConnect = false;

        _uiManager.SetMenuCurrent(UIManager.MenuType.ClientNetwork);
        _uiControllerClient.SetConnectionButtonAvailability(true);
        final boolean retVal = _notificationMgr.DisplayMessageOneResponse("Connection lost.");
        if(!retVal)
        {
            App.LogLine("WARNING: Failed to create pop-up message on connection lost.");
        }
    }

    private void ProcessBackPressed()
    {
        String messgeText = "Error";
        UIManager.MenuType destMenu = null;

        if(_uiManager.GetCurrentMenu().GetMyType() == UIManager.MenuType.ClientRemote)
        {
            messgeText = "Do you really want to disconnect?";
            destMenu = UIManager.MenuType.ClientNetwork;
        }
        else if(_uiManager.GetCurrentMenu().GetMyType() == UIManager.MenuType.ClientNetwork)
        {
            messgeText = "Do you really want to exit?";
            destMenu = null;
        }
        else
        {
            Utility.Assert(false);
        }

        final NotificationHelper.MessageState msgState = _notificationMgr.CheckMessageStateAndCleanup();
        if(msgState == NotificationHelper.MessageState.None)
        {
            final boolean retVal = _notificationMgr.DisplayMessageTwoResponses(messgeText);
            if(!retVal)
            {
                // Just leave without this notification.

                App.LogLine("WARNING: Failed to create pop-up message!");

                if(destMenu != null)
                {
                    _networkSystem.Disconnect();
                    _uiManager.SetMenuCurrent(destMenu);
                    _uiControllerClient.SetConnectionButtonAvailability(true);
                    _bIsConnected = false;
                }
                else
                {
                    _bIsRunning.set(false);
                }

                _bBackPressed = false;
            }
        }
        else if(msgState == NotificationHelper.MessageState.Positive)
        {
            if(destMenu != null)
            {
                _networkSystem.Disconnect();
                _uiManager.SetMenuCurrent(destMenu);
                _uiControllerClient.SetConnectionButtonAvailability(true);
                _bIsConnected = false;
            }
            else
            {
                _bIsRunning.set(false);
            }

            _bBackPressed = false;
        }
        else if(msgState == NotificationHelper.MessageState.Negative)
        {
            _bBackPressed = false;
        }
    }


    private static final boolean B_TEST_MODE = false;

    private InputSystem _inputSystem = null;
    private ClientNetworkSystem _networkSystem = null;
    private ClientUIController _uiControllerClient = null;

    private boolean _bBackPressed = false;

    private boolean _bTryingConnect = false;
    private boolean _bIsConnected = false;

    private AtomicBoolean _bIsRunning = new AtomicBoolean(false);
}
