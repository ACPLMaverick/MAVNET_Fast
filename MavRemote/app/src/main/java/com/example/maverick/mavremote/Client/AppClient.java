package com.example.maverick.mavremote.Client;

import android.support.constraint.ConstraintLayout;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;

import com.example.maverick.mavremote.ClientActivity;
import com.example.maverick.mavremote.R;
import com.example.maverick.mavremote.UI.UIManager;
import com.example.maverick.mavremote.Utility;

public class AppClient
{
    private AppClient() { }

    public static AppClient GetInstance()
    {
        if(_instance == null)
            _instance = new AppClient();

        return _instance;
    }

    public void Run(ClientActivity activity)
    {
        _activity = activity;
        Utility.StartThread(new Runnable()
        {
            @Override
            public void run()
            {
                InternalRun();
            }
        });
    }

    public ClientActivity GetActivity() { return _activity; }

    public ClientNetworkSystem GetNetworkSystem() { return _networkSystem; }

    public InputSystem GetInputSystem() { return _inputSystem; }

    public UIManager GetUIManager() { return _uiManager; }


    private void InternalRun()
    {
        InternalStart();
        InternalMainLoop();
        InternalFinish();
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

        _uiManager = new UIManager();
        _uiManager.InitMenu(UIManager.MenuType.ClientNetwork);
        _uiManager.InitMenu(UIManager.MenuType.ClientRemote);
        _uiManager.SetMenuCurrent(UIManager.MenuType.ClientNetwork);

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


    public static final String TAG = "MavRemote_Client";

    private static AppClient _instance = null;

    private ClientActivity _activity = null;

    private InputSystem _inputSystem = null;
    private ClientNetworkSystem _networkSystem = null;

    private UIManager _uiManager = null;

    private boolean _bIsRunning = false;
}
