package com.example.maverick.mavremote.Client;

import android.support.constraint.ConstraintLayout;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

import com.example.maverick.mavremote.Actions.ActionEvent;
import com.example.maverick.mavremote.App;
import com.example.maverick.mavremote.ClientActivity;
import com.example.maverick.mavremote.NetworkCommon.NetworkSystem;
import com.example.maverick.mavremote.R;
import com.example.maverick.mavremote.Server.ServerNetworkSystem;
import com.example.maverick.mavremote.UI.NotificationManager;
import com.example.maverick.mavremote.UI.UIManager;
import com.example.maverick.mavremote.Utility;

import java.net.InetSocketAddress;
import java.net.SocketAddress;

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


    public ClientActivity GetActivityTyped() { return (ClientActivity)_activity; }

    public ClientNetworkSystem GetNetworkSystem() { return _networkSystem; }

    public InputSystem GetInputSystem() { return _inputSystem; }

    public void OnBackButtonPressed()
    {
        _bBackPressed = true;
    }


    @Override
    protected void AssertActivityType()
    {
        Utility.Assert(_activity != null && _activity instanceof ClientActivity);
    }

    @Override
    protected void InternalRun()
    {
        InternalStart();
        InternalMainLoop();
        InternalFinish();
    }

    @Override
    protected void SetupUIManager()
    {
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

    private void InternalStart()
    {
        _inputSystem = new InputSystem();
        Utility.StartThread(new Runnable()
        {
            @Override
            public void run()
            {
                _inputSystem.Run();
            }
        });

        _networkSystem = new ClientNetworkSystem();
        Utility.StartThread(new Runnable()
        {
            @Override
            public void run()
            {
                _networkSystem.Run();
            }
        });

        App.LogLine("[AppClient] Started.");

        _bIsRunning = true;
    }

    private void InternalMainLoop()
    {
        while(_bIsRunning)
        {
            _uiController.Update();

            if(_bTryingConnect)
            {
                TryConnect();
            }

            if(_bIsConnected && _networkSystem.GetState() != NetworkSystem.State.Connected)
            {
                BackUIToDisconnected();
            }

            ProcessUI();

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

            ProcessQueue();
        }
    }

    private void InternalFinish()
    {
        _uiController.Cleanup();
        _inputSystem.Stop();
        _networkSystem.Stop();

        _activity.finish();
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
    }

    private void BackUIToDisconnected()
    {
        _bIsConnected = false;
        _bTryingConnect = false;

        _uiManager.SetMenuCurrent(UIManager.MenuType.ClientNetwork);
        _uiControllerClient.SetConnectionButtonAvailability(true);
        _notificationMgr.DisplayMessageOneResponse("Connection lost.");
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

        final NotificationManager.MessageState msgState = _notificationMgr.CheckMessageStateAndCleanup();
        if(msgState == NotificationManager.MessageState.None)
        {
            _notificationMgr.DisplayMessageTwoResponses(messgeText);
        }
        else if(msgState == NotificationManager.MessageState.Positive)
        {
            if(destMenu != null)
                _uiManager.SetMenuCurrent(destMenu);
            else
                _bIsRunning = false;

            _bBackPressed = false;
        }
        else if(msgState == NotificationManager.MessageState.Negative)
        {
            _bBackPressed = false;
        }
    }

    private InputSystem _inputSystem = null;
    private ClientNetworkSystem _networkSystem = null;
    private ClientUIController _uiControllerClient = null;

    private boolean _bBackPressed = false;

    private boolean _bTryingConnect = false;
    private boolean _bIsConnected = false;

    private boolean _bIsRunning = false;
}
