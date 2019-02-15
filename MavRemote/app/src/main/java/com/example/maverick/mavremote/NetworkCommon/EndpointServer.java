package com.example.maverick.mavremote.NetworkCommon;

import com.example.maverick.mavremote.App;

import java.io.IOException;
import java.io.InputStream;
import java.net.InetSocketAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketAddress;
import java.nio.ByteBuffer;

import javax.net.ServerSocketFactory;

public class EndpointServer
{
    public boolean Init(SocketAddress address, int timeout)
    {
        try
        {
            _sock = ServerSocketFactory.getDefault().
                    createServerSocket(((InetSocketAddress)address).getPort());
            SetTimeout(timeout);
        }
        catch(IOException e)
        {
            App.LogLine("Error creating socket with given address: "
                    + address.toString() + "\n" + e.getMessage());
            _sock = null;
            return false;
        }

        return _sock.isBound() && _sock.isBound();
    }

    public boolean Close()
    {
        if(_connectionSock != null)
        {
            CloseConnectionSock();
        }

        if(_sock == null)
            return false;

        try
        {
            _sock.close();
        }
        catch(IOException e)
        {
            App.LogLine("Error closing server socket with address: "
                    + _sock.getLocalSocketAddress().toString() + " and port: "
                    + Integer.toString(_sock.getLocalPort()) + "\n"
                    + e.getMessage());

            return false;
        }

        return true;
    }

    // This will block for a timeout, because it listens for connections when no connection is made.
    public void Update()
    {
        // TODO: FIX THIS CRASH.

        if(!IsConnected())
            ListenForConnections();

        CheckConnectionState();
    }

    // Returns null if there is no data.
    public ByteBuffer GetData()
    {
        if(_connectionSockInputStream == null)
        {
            return null;
        }

        return _inputReader.Read();
    }

    public int GetTimeout()
    {
        try
        {
            return _sock.getSoTimeout();
        }
        catch(IOException e)
        {
            App.LogLine("Error on socket GetTimeout: " + e.getMessage());

            return 0;
        }
    }

    public boolean IsConnected()
    {
        return _connectionSock != null
            && _connectionSock.isBound()
            && _connectionSock.isConnected()
            && _inputReader != null
            && _inputReader.GetFailuresInRow() <= CONNECTION_SOCK_MAX_FAILURES_IN_ROW;
    }

    public SocketAddress GetAddress()
    {
        if(!IsConnected())
        {
            return Endpoint.BAD_ADDRESS;
        }
        return _connectionSockAddress;
    }



    private void SetTimeout(int timeout)
    {
        try
        {
            _sock.setSoTimeout(timeout);
        }
        catch(IOException e)
        {
            App.LogLine("Error on socket SetTimeout: " + e.getMessage());
        }
    }

    private void ListenForConnections()
    {
        try
        {
            Socket connSock = _sock.accept();

            // If no exception has been thrown, assuming the connection has been made.
            if(connSock != null)
            {
                InitConnectionSock(connSock);
            }
            else
            {
                App.LogLine("ServerSocket accept returned a null socket!");
            }
        }
        catch(IOException e)
        {
            if(e.getMessage() != null)
            {
                App.LogLine("Error on socket accept: " + e.getMessage());
            }
        }
    }

    private void CheckConnectionState()
    {
        if(_connectionSock == null)
        {
            return;
        }

        if(!IsConnected())
        {
            CloseConnectionSock();
        }
    }

    private void InitConnectionSock(Socket newConnectionSock)
    {
        _connectionSock = newConnectionSock;
        try
        {
            _connectionSockInputStream = _connectionSock.getInputStream();
            _connectionSockAddress = new InetSocketAddress(_connectionSock.getInetAddress(), _connectionSock.getPort());
            _inputReader.Init(_connectionSockInputStream);
        }
        catch(IOException e)
        {
            App.LogLine("Error initializing connection socket: " + e.getMessage());
            CloseConnectionSock();
        }
    }

    private void CloseConnectionSock()
    {
        _inputReader.Stop();

        if(!_connectionSock.isClosed())
        {
            try
            {
                _connectionSock.close();
            }
            catch(IOException e)
            {
                App.LogLine("Error on connection socket close: " + e.getMessage());
            }
        }

        _connectionSock = null;
        _connectionSockInputStream = null;
        _connectionSockAddress = null;
    }


    private static int CONNECTION_SOCK_MAX_FAILURES_IN_ROW = 16;

    private ServerSocket _sock = null;
    private Socket _connectionSock = null;
    private InputStream _connectionSockInputStream = null;
    private InetSocketAddress _connectionSockAddress = null;

    private DataStreamReadHelper _inputReader = new DataStreamReadHelper();
}
