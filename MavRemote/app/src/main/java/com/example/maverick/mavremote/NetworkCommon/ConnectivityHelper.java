package com.example.maverick.mavremote.NetworkCommon;

import android.content.Context;
import android.net.ConnectivityManager;
import android.net.Network;
import android.net.NetworkInfo;
import android.util.Log;

import com.example.maverick.mavremote.App;
import com.example.maverick.mavremote.Utility;

import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.InterfaceAddress;
import java.net.NetworkInterface;
import java.net.SocketAddress;
import java.net.SocketException;
import java.util.Calendar;
import java.util.Enumeration;

public final class ConnectivityHelper
{
    public void Init()
    {
        _cm = (ConnectivityManager) App.GetInstance().GetActivity().
                getApplicationContext().getSystemService(Context.CONNECTIVITY_SERVICE);
        Utility.Assert(_cm != null);
        _updateTimer = 0;

        Update();
    }

    public void Update()
    {
        long delta = Calendar.getInstance().getTimeInMillis() - _updateTimer;
        if(delta < UPDATE_PERIOD_MILLIS)
        {
            return;
        }
        _updateTimer = Calendar.getInstance().getTimeInMillis();


        _netInfo = _cm.getActiveNetworkInfo();

        try
        {
            boolean bFoundAddress = false;

            for(Enumeration<NetworkInterface> en = NetworkInterface.getNetworkInterfaces();
                    en.hasMoreElements(); )
            {
                NetworkInterface intf = en.nextElement();

                if(intf.isLoopback()
                        || !intf.isUp()
                        || !intf.getName().contains("lan"))
                {
                    continue;
                }

                for(InterfaceAddress addr : intf.getInterfaceAddresses())
                {
                    final boolean bNotLoopback =  !addr.getAddress().isLoopbackAddress();
                    final boolean bLocal =
                            addr.getAddress().isSiteLocalAddress()
                            || addr.getAddress().isLinkLocalAddress()
                            || addr.getAddress().isAnyLocalAddress();
                    if(bNotLoopback && bLocal)
                    {
                        if(_netAddress == null)
                        {
                            Log.e(App.TAG, "[ConnectivityHelper] Found valid address: "
                                    + addr.getAddress().toString() + " with broadcast: "
                                    + addr.getBroadcast().toString() + " .");
                        }

                        _netInterface = intf;
                        _netAddress = addr.getAddress();
                        _broadcastAddress = addr.getBroadcast();

                        bFoundAddress = true;
                        break;
                    }
                }

                if(bFoundAddress)
                    break;
            }

            if(!bFoundAddress)
            {
                _netInfo = null;
                _netAddress = null;
                _broadcastAddress = null;
                _netInterface = null;

                Log.e(App.TAG, "[ConnectivityHelper] Failed to find valid address. " +
                        "Is device connected to local network?");
            }
        }
        catch(SocketException e)
        {
            Log.e(App.TAG, "[ConnectivityHelper] Error getting network interfaces: " + e.getMessage());
            return;
        }
    }

    public void Cleanup()
    {

    }

    public boolean IsConnectedToLocalNetwork()
    {
        return _netInterface != null && _netInfo != null && _netAddress != null &&
                _netInfo.isConnected() && _netInfo.getType() == ConnectivityManager.TYPE_WIFI;
    }

    public SocketAddress GetMyLocalAddress()
    {
        if(_netAddress != null)
        {
            return new InetSocketAddress(_netAddress, CONNECTION_PORT_BASE);
        }
        else
        {
            return null;
        }
    }

    public SocketAddress GetBroadcastAddress()
    {
        if(_netAddress != null)
        {
            return new InetSocketAddress(_broadcastAddress, BROADCAST_PORT);
        }
        else
        {
            return null;
        }
    }

    public SocketAddress GetLocalhostAddress()
    {
        return new InetSocketAddress("127.0.0.1", LOCALHOST_PORT);
    }


    public static int UPDATE_PERIOD_MILLIS = 1000;
    public static int CONNECTION_PORT_BASE = 4446;
    public static int BROADCAST_PORT = 4445;
    public static int LOCALHOST_PORT = 1234;


    private ConnectivityManager _cm = null;
    private NetworkInfo _netInfo = null;
    private NetworkInterface _netInterface = null;
    private InetAddress _netAddress = null;
    private InetAddress _broadcastAddress = null;

    private long _updateTimer = 0;
}