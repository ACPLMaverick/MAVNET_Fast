package com.example.maverick.mavremote;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;

import com.example.maverick.mavremote.Client.AppClient;

public class ClientActivity extends AppCompatActivity
{

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_client);

        AppClient.GetInstance().Run();
    }
}
