package com.example.maverick.mavremote.Client;


import com.example.maverick.mavremote.Actions.ActionEvent;
import com.example.maverick.mavremote.App;
import com.example.maverick.mavremote.EventQueue;
import com.example.maverick.mavremote.NetworkCommon.ConnectivityHelper;
import com.example.maverick.mavremote.NetworkCommon.DataPacketFactory;
import com.example.maverick.mavremote.NetworkCommon.DataPacketRetriever;
import com.example.maverick.mavremote.NetworkCommon.Endpoint;
import com.example.maverick.mavremote.NetworkCommon.EndpointDatagram;
import com.example.maverick.mavremote.NetworkCommon.NetworkSystem;

import java.net.SocketAddress;
import java.nio.ByteBuffer;

public class ClientNetworkSystem extends NetworkSystem
{
    public void PushActionEvent(ActionEvent event)
    {
        _actionEventQueue.Enqueue(event);
    }

    @Override
    public SocketAddress GetConnectedAddress()
    {
        if(_endpoint != null)
        {
            return _endpoint.GetAddress();
        }
        else
        {
            return null;
        }
    }

    @Override
    protected boolean ProcessStateWaitingForConnection()
    {
        if(!super.ProcessStateWaitingForConnection())
            return false;

        // Receive all messages on broadcast, decode them and check for OUR broadcast packet.
        // If this is the one, get IP address from it and create endpoint.

        // This will block. For now let's keep it that way.
        ByteBuffer buffer = _endpointBroadcast.GetDataBroadcast();
        if(buffer != null)
        {
            _packetFactory.DecodePacket(buffer, _tmpRetriever);
            if(_tmpRetriever.IsValid())
            {
                if(_tmpRetriever.ThisType == DataPacketFactory.PacketType.Broadcast)
                {
                    SocketAddress address = (SocketAddress)_tmpRetriever.ObjectRef;
                    if(CreateConnectionEndpoint(address) && IsConnectedToRemoteHost())
                    {
                        GoToState(State.Connected);
                    }
                }
            }
        }

        return true;
    }

    @Override
    protected boolean ProcessStateConnected()
    {
        if(!super.ProcessStateConnected())
            return false;


        while(!_actionEventQueue.IsEmpty())
        {
            ActionEvent ev = _actionEventQueue.Dequeue();
            ByteBuffer packet = _packetFactory.CreatePacket(ev);
            _endpoint.SendData(packet);
            _lastPacketSentTime = App.GetCurrentTimeMs();
        }

        ProcessPingServer();

        return true;
    }

    protected void ProcessPingServer()
    {
        final long currTime = App.GetCurrentTimeMs();
        if(currTime - _lastPacketSentTime <= PING_INTERVAL_MS)
        {
            return;
        }

        ByteBuffer packet = _packetFactory.CreatePacketPing();
        _endpoint.SendData(packet);
        _lastPacketSentTime = currTime;
    }

    @Override
    protected boolean IsConnectedToRemoteHost()
    {
        return _endpoint != null && _endpoint.IsConnected();
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
        _endpointBroadcast.Init(_connHelper, EndpointDatagram.Mode.Common, ConnectivityHelper.BROADCAST_PORT);

        // Defer creating endpoint until we actually know the address.
    }

    protected boolean CreateConnectionEndpoint(SocketAddress address)
    {
        _endpoint = new Endpoint();
        return _endpoint.Init(address, ENDPOINT_TIMEOUT_MILLIS);
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


    protected static final long PING_INTERVAL_MS = 1000;
    protected static final int ENDPOINT_TIMEOUT_MILLIS = 1000;

    protected Endpoint _endpoint = null;
    protected DataPacketRetriever _tmpRetriever = new DataPacketRetriever();
    protected long _lastPacketSentTime = 0;
}
