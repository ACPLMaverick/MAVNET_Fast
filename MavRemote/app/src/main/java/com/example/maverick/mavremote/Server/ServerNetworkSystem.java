package com.example.maverick.mavremote.Server;

import android.util.Log;

import com.example.maverick.mavremote.App;
import com.example.maverick.mavremote.NetworkCommon.ConnectivityHelper;
import com.example.maverick.mavremote.NetworkCommon.EndpointDatagram;
import com.example.maverick.mavremote.NetworkCommon.EndpointServer;
import com.example.maverick.mavremote.NetworkCommon.NetworkSystem;

import java.nio.ByteBuffer;
import java.util.Calendar;

public class ServerNetworkSystem extends NetworkSystem
{
    @Override
    public boolean StartAwaitingConnections()
    {
        if(!super.StartAwaitingConnections())
            return false;

        _broadcastPacket = _packetFactory.CreatePacketBroadcast(_connHelper.GetMyLocalAddress());
        return true;
    }

    @Override
    protected boolean ProcessStateWaitingForConnection()
    {
        if(!super.ProcessStateWaitingForConnection())
            return false;

// In specified frequency, send broadcast packet over broadcast endpoint.

        long delta = Calendar.getInstance().getTimeInMillis() - _timerBroadcast;
        if(delta >= ENDPOINT_BROADCAST_SEND_PERIOD_MILLIS)
        {
            _timerBroadcast = Calendar.getInstance().getTimeInMillis();

            _endpointBroadcast.SendDataBroadcast(_broadcastPacket);
        }


        // Check if any connection happen in the server endpoint.

        _endpoint.Update(); // This blocks for ENDPOINT_SERVER_TIMEOUT_MILLIS

        if(_endpoint.IsConnected())
        {
            _timerBroadcast = 0;
            GoToState(State.Connected);
        }

        return true;
    }

    @Override
    protected boolean ProcessStateConnected()
    {
        if(!super.ProcessStateConnected())
            return false;

        // Monitor endpoint connection.

        _endpoint.Update();

        if(!_endpoint.IsConnected())
        {
            Log.e(App.TAG, "[ServerNetworkSystem] Lost connection with Client.");
            if(!StartAwaitingConnections())
            {
                ProcessConnectionLost();
            }
        }


        // Check for incoming packet data, and add them to received queue.

        ByteBuffer receivedBuffer = _endpoint.GetData();
        if(receivedBuffer != null)
        {
            _packetQueue.Enqueue(receivedBuffer);
        }

        return true;
    }

    @Override
    protected void ProcessConnectionLost()
    {
        super.ProcessConnectionLost();

        if(_endpoint != null)
            _endpoint.Close();

        _endpoint = null;
    }

    @Override
    protected void CreateEndpoints()
    {
        _endpointBroadcast = new EndpointDatagram();
        _endpointBroadcast.Init(_connHelper, EndpointDatagram.Mode.Broadcast, ConnectivityHelper.BROADCAST_PORT);

        _endpoint = new EndpointServer();
        _endpoint.Init(_connHelper.GetMyLocalAddress(), ENDPOINT_SERVER_TIMEOUT_MILLIS);
    }

    @Override
    protected void DestroyEndpoints()
    {
        if(_endpointBroadcast != null)
            _endpointBroadcast.Close();
        if(_endpoint != null)
            _endpoint.Close();

        _endpointBroadcast = null;
        _endpoint = null;
    }


    protected static final int ENDPOINT_SERVER_TIMEOUT_MILLIS = 1000;
    protected static final int ENDPOINT_BROADCAST_SEND_PERIOD_MILLIS = 1000;

    protected EndpointServer _endpoint = null;

    protected ByteBuffer _broadcastPacket = null;
    protected long _timerBroadcast = 0;
}