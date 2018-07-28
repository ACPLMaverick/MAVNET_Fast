package com.example.maverick.mavremote;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.IBinder;
import android.support.annotation.Nullable;
import android.util.Log;

import com.example.maverick.mavremote.Server.AppServer;

public class ServerService extends Service {
    public ServerService()
    {
    }

    @Nullable
    @Override
    public IBinder onBind(Intent intent)
    {
        return null;
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId)
    {
        // Create app here.

        Context ctx = getApplication().getApplicationContext();
        final boolean bLaunchedFromActivity = intent.getBooleanExtra(
                "LaunchedFromActivity", false);
        if(!AppServer.GetInstance().IsRunning())
        {
            if(!bLaunchedFromActivity)
            {
                AppServer.GetInstance().SetForceConnect(true);
                AppServer.GetInstance().Run(ctx);
            }
            else
            {
                // Do not override setup done by activity before.
                AppServer.GetInstance().Run();
            }
        }
        else
        {
            if(!bLaunchedFromActivity)
            {
                AppServer.GetInstance().UpdateActivityState(null, ctx);
            }
        }

        Log.d(App.TAG, "[Activity] [onStartCommand]");

        return START_STICKY;
    }

    @Override
    public void onDestroy()
    {
        // Kill the app if service dies.
        if(AppServer.GetInstance().IsRunning())
            AppServer.GetInstance().Stop();

        Log.d(App.TAG, "[Service] [onDestroy]");
    }
}
