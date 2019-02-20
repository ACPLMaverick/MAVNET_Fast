package com.example.maverick.mavremote.Server;

import com.example.maverick.mavremote.Actions.ActionEvent;
import com.example.maverick.mavremote.App;
import com.example.maverick.mavremote.NetworkCommon.ConnectivityHelper;
import com.example.maverick.mavremote.NetworkCommon.DataPacketRetriever;
import com.example.maverick.mavremote.NetworkCommon.EndpointDatagram;
import com.example.maverick.mavremote.NetworkCommon.EndpointServer;
import com.example.maverick.mavremote.NetworkCommon.NetworkSystem;
import com.example.maverick.mavremote.Utility;

import java.net.SocketAddress;
import java.nio.ByteBuffer;

public class ServerNetworkSystem extends NetworkSystem
{
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

    public ActionEvent PopActionEvent()
    {
        if(_actionEventQueue.IsEmpty())
        {
            return null;
        }
        else
        {
            return _actionEventQueue.Dequeue();
        }
    }

    public boolean HasActionEvents()
    {
        return !_actionEventQueue.IsEmpty();
    }

    public PacketCounter GetPacketCounterReceived()
    {
        return _packetCounter;
    }

    public PacketCounter GetPacketCounterBroadcasted()
    {
        return _packetCounterBroadcast;
    }

    @Override
    protected void Start()
    {
        super.Start();

        _broadcastPacket = _packetFactory.CreatePacketBroadcast(_connHelper.GetConnectionLocalAddress());
    }

    @Override
    protected boolean ProcessStateWaitingForConnection()
    {
        if(!super.ProcessStateWaitingForConnection())
            return false;

// In specified frequency, send broadcast packet over broadcast endpoint.

        long delta = App.GetCurrentTimeMs() - _timerBroadcast;
        if(delta >= ENDPOINT_BROADCAST_SEND_PERIOD_MILLIS)
        {
            _timerBroadcast = App.GetCurrentTimeMs();

            // Do that only if the screen is on.
            if(Utility.IsScreenOn())
            {
                _endpointBroadcast.SendDataBroadcast(_broadcastPacket);
                _packetCounterBroadcast.IncPacketNumCorrect();
                // App.LogLine("Sending broadcast packet " + _packetCounterBroadcast.GetPacketNumCorrect());
            }
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

        _endpoint.Update();

        // Check for incoming packet data, and process them.

        ByteBuffer receivedBuffer = _endpoint.GetData();
        if(receivedBuffer != null)
        {
            _packetFactory.DecodePacket(receivedBuffer, _tmpRetriever);
            if(_tmpRetriever.IsValid())
            {
                _packetCounter.IncPacketNumCorrect();
                ProcessRetrievedObject(_tmpRetriever);
            }
            else
            {
                _packetCounter.IncPacketNumWrong();
                App.LogLine("Server decoded an invalid packet!");
            }
        }

        return true;
    }

    @SuppressWarnings("unchecked")
    protected void ProcessRetrievedObject(DataPacketRetriever retriever)
    {
        switch (retriever.ThisType)
        {
            case ActionEvent:
            {
                ActionEvent actionEvent = (ActionEvent)retriever.ObjectRef;
                _actionEventQueue.Enqueue(actionEvent);
            }
                break;
            case Unknown:
            case Broadcast:
            default:
                break;
        }
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

        _packetCounter.Reset();

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
        _endpoint.Init(_connHelper.GetConnectionLocalAddress(), ENDPOINT_SERVER_TIMEOUT_MILLIS);
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

    protected PacketCounter _packetCounter = new PacketCounter();
    protected PacketCounter _packetCounterBroadcast = new PacketCounter();

    protected EndpointServer _endpoint = null;

    protected ByteBuffer _broadcastPacket = null;
    private DataPacketRetriever _tmpRetriever = new DataPacketRetriever();
    protected long _timerBroadcast = 0;
}
