package com.example.maverick.mavremote.NetworkCommon;

import android.util.Log;

import com.example.maverick.mavremote.Actions.ActionEvent;
import com.example.maverick.mavremote.App;
import com.example.maverick.mavremote.EventQueue;
import com.example.maverick.mavremote.System;

import java.net.SocketAddress;
import java.nio.ByteBuffer;

public abstract class NetworkSystem extends System
{
    public enum State
    {
        Invalid,
        NotConnectedIdle,
        WaitingForConnection,
        Connected
    }

    public boolean StartAwaitingConnections()
    {
        if(!_connHelper.IsConnectedToLocalNetwork())
            return false;

        CreateEndpoints();
        GoToState(State.WaitingForConnection);
        return true;
    }

    public void Disconnect()
    {
        _bNeedDisconnect = true;
    }


    public State GetState()
    {
        return _state;
    }

    public abstract SocketAddress GetConnectedAddress();

    public SocketAddress GetMyAddress()
    {
        if(_connHelper.IsConnectedToLocalNetwork())
        {
            return _connHelper.GetConnectionLocalAddress();
        }
        else
        {
            return null;
        }
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

        _actionEventQueue = new EventQueue<>();
        _actionEventQueue.Init();
    }

    @Override
    protected void Finish()
    {
        _connHelper.Cleanup();
        ProcessConnectionLost();
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

        if(_bNeedDisconnect)
        {
            ProcessDisconnect();
            _bNeedDisconnect = false;
        }
    }

    protected boolean ProcessStateNotConnectedIdle()
    {
        // If no endpoints are created check for connection and create'em.

        /*
        if(_endpointBroadcast == null)
        {
            if(_connHelper.IsConnectedToLocalNetwork())
            {

            }
        }
        */

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
        if(!_connHelper.IsConnectedToLocalNetwork() || !IsConnectedToRemoteHost())
        {
            ProcessConnectionLost();
            return false;
        }

        return true;
    }

    protected abstract boolean IsConnectedToRemoteHost();

    protected boolean ProcessDisconnect()
    {
        DestroyEndpoints();
        GoToState(State.NotConnectedIdle);
        return true;
    }

    protected void GoToState(State state)
    {
        App.LogLine("[" + this.getClass().getSimpleName() + "] Goes to State: " + state.toString());
        CleanupAfterState(_state);
        _state = state;
    }

    protected void CleanupAfterState(State stateCurrent)
    {
        if(stateCurrent == State.Connected)
        {
            _connHelper.Cleanup();
        }
    }

    protected void ProcessConnectionLost()
    {
        App.LogLine("[" + this.getClass().getSimpleName() + "] Network connection lost.");

        DestroyEndpoints();

        GoToState(State.NotConnectedIdle);
    }

    protected abstract void CreateEndpoints();
    protected abstract void DestroyEndpoints();

    protected ConnectivityHelper _connHelper = null;
    protected DataPacketFactory _packetFactory = null;
    protected EndpointDatagram _endpointBroadcast = null;

    protected EventQueue<ByteBuffer> _packetQueue = null;
    protected EventQueue<ActionEvent> _actionEventQueue = null;

    protected State _state = State.NotConnectedIdle;

    protected boolean _bNeedDisconnect = false;
}
