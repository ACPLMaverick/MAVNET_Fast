package com.example.maverick.mavremote.NetworkCommon;

import android.util.Log;

import com.example.maverick.mavremote.App;
import com.example.maverick.mavremote.EventQueue;
import com.example.maverick.mavremote.System;

import java.nio.ByteBuffer;

public abstract class NetworkSystem extends System
{
    public enum State
    {
        NotConnectedIdle,
        WaitingForConnection,
        Connected
    }

    public boolean StartAwaitingConnections()
    {
        if(!_connHelper.IsConnectedToLocalNetwork())
            return false;

        GoToState(State.WaitingForConnection);
        return true;
    }

    public State GetState()
    {
        return _state;
    }

    public EventQueue<ByteBuffer> GetPacketsQueue()
    {
        return _packetQueue;
    }

    @Override
    protected void Start()
    {
        _connHelper = new ConnectivityHelper();
        _connHelper.Init();

        _packetFactory = new DataPacketFactory();

        _state = State.NotConnectedIdle;

        _packetQueue = new EventQueue<>();
        _packetQueue.Init();
    }

    @Override
    protected void Finish()
    {
        _connHelper.Cleanup();
        GoToState(State.NotConnectedIdle);
    }

    @Override
    protected void MainLoop()
    {
        _connHelper.Update(); // Will automatically update only in specified frequency.

        switch(_state)
        {
            default:
            case NotConnectedIdle:
                ProcessStateNotConnectedIdle();
                break;
            case WaitingForConnection:
                ProcessStateWaitingForConnection();
                break;
            case Connected:
                ProcessStateConnected();
                break;
        }
    }

    protected boolean ProcessStateNotConnectedIdle()
    {
        // If no endpoints are created check for connection and create'em.

        if(_endpointBroadcast == null)
        {
            if(_connHelper.IsConnectedToLocalNetwork())
            {
                CreateEndpoints();
                StartAwaitingConnections();
            }
        }

        return true;
    }

    protected boolean ProcessStateWaitingForConnection()
    {
        if(!_connHelper.IsConnectedToLocalNetwork())
        {
            ProcessConnectionLost();
            return false;
        }

        return true;
    }

    protected boolean ProcessStateConnected()
    {
        if(!_connHelper.IsConnectedToLocalNetwork())
        {
            ProcessConnectionLost();
            return false;
        }

        return true;
    }

    protected void GoToState(State state)
    {
        Log.e(App.TAG, "[" + this.getClass().getSimpleName() + "] Goes to State: " + state.toString());
        _state = state;
    }

    protected void ProcessConnectionLost()
    {
        Log.e(App.TAG, "[" + this.getClass().getSimpleName() + "] Network connection lost.");

        DestroyEndpoints();

        GoToState(State.NotConnectedIdle);
    }

    protected abstract void CreateEndpoints();
    protected abstract void DestroyEndpoints();

    protected ConnectivityHelper _connHelper = null;
    protected DataPacketFactory _packetFactory = null;
    protected EndpointDatagram _endpointBroadcast = null;

    protected EventQueue<ByteBuffer> _packetQueue = null;

    protected State _state;
}
