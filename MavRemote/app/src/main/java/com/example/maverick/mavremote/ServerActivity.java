package com.example.maverick.mavremote;

import android.content.pm.ActivityInfo;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;

import com.example.maverick.mavremote.Server.AppServer;

public class ServerActivity extends AppCompatActivity
{
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_server);
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);

        // Service is disabled for now because it would not work on API 17.

        if(!AppServer.GetInstance().IsRunning())
        {
            //AppServer.GetInstance().UpdateActivityState(this, null);
            AppServer.GetInstance().Run(this);
        }

        /*
        Intent intent = new Intent(this, ServerService.class);
        intent.putExtra("LaunchedFromActivity", true);
        startService(intent);
        */
        Log.d(App.TAG, "[Activity] [onCreate]");
    }

    @Override
    protected void onDestroy()
    {
        super.onDestroy();

        if(AppServer.GetInstance().IsRunning())
            AppServer.GetInstance().Stop();

        // This is disabled now as would not want to work on API 17.

        // Do not stop app, as this is a service.
        // Only clear reference to this activty to avoid memory leak.
        //AppServer.GetInstance().UpdateActivityState(null, getApplicationContext());

        Log.d(App.TAG, "[Activity] [onDestroy]");
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event)
    {
        if(keyCode != KeyEvent.KEYCODE_BACK)
            return super.onKeyDown(keyCode, event);

        if(AppServer.GetInstance().IsRunning())
        {
            AppServer.GetInstance().SetBackPressed(true);
        }

        return true;
    }
}
