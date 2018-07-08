package com.example.maverick.mavremote.NetworkCommon;

import android.util.Log;

import com.example.maverick.mavremote.App;

import java.io.IOException;
import java.io.InputStream;
import java.net.InetSocketAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketAddress;
import java.nio.ByteBuffer;
import java.util.ArrayList;

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
            Log.e(App.TAG, "Error creating socket with given address: "
                    + address.toString() + "\n" + e.getMessage());
            _sock = null;
            return false;
        }

        return _sock.isBound() && _sock.isBound();
    }

    public boolean Close()
    {
        if(_sock == null)
            return false;

        try
        {
            _sock.close();
        }
        catch(IOException e)
        {
            Log.e(App.TAG, "Error closing server socket with address: "
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
        if(!IsConnected())
            ListenForConnections();

        CheckConnectionState();
    }

    // Returns null if there is no data.
    public ByteBuffer GetData()
    {
        try
        {
            InputStream stream = _connectionSock.getInputStream();
            if(stream.available() <= 0)
            {
                return null;
            }

            boolean bAvailable = true;
            ArrayList<Byte> allBytes = new ArrayList<>();
            do
            {
                int bytesRead = stream.read(_connectionReadBuffer, 0, CONNECTION_BUFFER_SIZE);
                for(int i = 0; i < bytesRead; ++i)
                    allBytes.add(_connectionReadBuffer[i]);

                bAvailable = stream.available() > 0;
            }while(bAvailable);

            byte[] fundamentalBytes = new byte[allBytes.size()];
            for(int i = 0; i < allBytes.size(); ++i)
                fundamentalBytes[i] = allBytes.get(i);

            return ByteBuffer.wrap(fundamentalBytes);
        }
        catch(IOException e)
        {
            Log.e(App.TAG, "Error reading from Server Connection Socket with address: "
                    + _connectionSock.getLocalSocketAddress().toString() + " and port: "
                    + Integer.toString(_connectionSock.getLocalPort()) + "\n"
                    + e.getMessage());

            return null;
        }
    }

    public int GetTimeout()
    {
        try
        {
            return _sock.getSoTimeout();
        }
        catch(IOException e)
        {
            Log.e(App.TAG, "Error on socket GetTimeout: " + e.getMessage());

            return 0;
        }
    }

    public void SetTimeout(int timeout)
    {
        try
        {
            _sock.setSoTimeout(timeout);
        }
        catch(IOException e)
        {
            Log.e(App.TAG, "Error on socket SetTimeout: " + e.getMessage());

            return;
        }
    }

    public boolean IsConnected() { return _connectionSock != null; }


    private void ListenForConnections()
    {
        try
        {
            _sock.accept();
        }
        catch(IOException e)
        {
            Log.e(App.TAG, "Error on socket accept: " + e.getMessage());

            return;
        }
    }

    private void CheckConnectionState()
    {
        if(_connectionSock != null
                && !(_connectionSock.isBound() || _connectionSock.isConnected()))
        {
            if(!_connectionSock.isClosed())
            {
                try
                {
                    _connectionSock.close();
                }
                catch(IOException e)
                {
                    Log.e(App.TAG, "Error on connection socket close: " + e.getMessage());
                }
            }

            _connectionSock = null;
        }
    }


    private static final int CONNECTION_BUFFER_SIZE = 1024;


    private ServerSocket _sock = null;
    private Socket _connectionSock = null;

    private byte[] _connectionReadBuffer = new byte[CONNECTION_BUFFER_SIZE];
}