package com.example.maverick.mavremote;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.KeyEvent;

import com.example.maverick.mavremote.Client.AppClient;
import com.example.maverick.mavremote.Client.InputSystem;

public class ClientActivity extends AppCompatActivity
{

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_client);

        _systemKeyEvents = new EventQueue<>();
        _systemKeyEvents.Init();

        AppClient.GetInstance().Run(this);
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event)
    {
        _systemKeyEvents.Enqueue(event);

        // TODO: Think of a better way to do this (ClientActivity shouldn't be aware of InputSystem).
        for(int blacklistedKey : InputSystem.KEYS_BLACKLISTED_ON_DEVICE)
        {
            if(keyCode == blacklistedKey)
            {
                return true;
            }
        }

        return super.onKeyDown(keyCode, event);
    }


    public EventQueue<KeyEvent> GetSystemKeyEvents()
    {
        return _systemKeyEvents;
    }

    private EventQueue<KeyEvent> _systemKeyEvents;
}
