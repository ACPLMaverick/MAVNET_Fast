package com.example.maverick.mavremote.Client;

import android.support.constraint.ConstraintLayout;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

import com.example.maverick.mavremote.App;
import com.example.maverick.mavremote.ClientActivity;
import com.example.maverick.mavremote.R;
import com.example.maverick.mavremote.UI.UIManager;
import com.example.maverick.mavremote.Utility;

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

    public UIManager GetUIManager() { return _uiManager; }


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

        // TODO: Make other threads be able to modify UI.
        TextView tv = _uiManager.GetCurrentMenu().GetTextViews().get(R.id.tvStatus);
//        tv.setText("Chuj");

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

    private InputSystem _inputSystem = null;
    private ClientNetworkSystem _networkSystem = null;

    private boolean _bIsRunning = false;
}
