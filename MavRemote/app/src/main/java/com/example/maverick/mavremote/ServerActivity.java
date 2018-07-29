package com.example.maverick.mavremote;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;

import com.example.maverick.mavremote.Server.AppServer;

public class ServerActivity extends AppCompatActivity
{

    // C++ unnecessary at this point.
    /*
    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }
    */

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_server);

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

    // C++ unnecessary at this point.
    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    /*
    public native String stringFromJNI();
    */
}
