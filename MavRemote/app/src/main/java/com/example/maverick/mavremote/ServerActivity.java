package com.example.maverick.mavremote;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;

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

        TextView tv = (TextView) findViewById(R.id.sample_text);
        tv.setText("MavRemote v.0.1t");

        if(!AppServer.GetInstance().IsRunning())
            AppServer.GetInstance().Run(this);
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
