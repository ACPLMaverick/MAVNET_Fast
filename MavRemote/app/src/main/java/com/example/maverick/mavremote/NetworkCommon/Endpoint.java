package com.example.maverick.mavremote.NetworkCommon;

import android.util.Log;

import com.example.maverick.mavremote.App;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketAddress;
import java.nio.ByteBuffer;
import java.util.ArrayList;

import javax.net.SocketFactory;

public class Endpoint
{
    public Endpoint() { }

    public boolean Init(SocketAddress address, int timeout)
    {
        try
        {
            _sock = SocketFactory.getDefault().createSocket();
            _sock.connect(address, timeout);
        }
        catch(IOException e)
        {
            App.LogLine("Error creating socket with given address: "
                    + address.toString() + "\n" + e.getMessage());
            _sock = null;
            return false;
        }

        final boolean bSuccess = _sock.isBound() && _sock.isConnected();
        if(bSuccess)
        {
            try
            {
                _sockOutputStream = _sock.getOutputStream();
                _sockInputStream = _sock.getInputStream();
                _inputStreamHelper.Init(_sockInputStream);
            }
            catch(IOException e)
            {
                App.LogLine("Error obtaining streams of socket with given address: "
                        + address.toString() + "\n" + e.getMessage());
                _sock = null;
                return false;
            }
            _sockAddress = new InetSocketAddress(_sock.getInetAddress(), _sock.getPort());
        }
        else
        {
            _sock = null;
        }

        return bSuccess;
    }

    public boolean Close()
    {
        if(_sock == null)
            return false;

        _inputStreamHelper.Stop();

        try
        {
            _sock.close();
        }
        catch(IOException e)
        {
            App.LogLine("Error closing socket with address: "
                    + _sock.getLocalAddress().toString() + " and port: "
                    + Integer.toString(_sock.getPort()) + "\n"
                    + e.getMessage());

            return false;
        }

        return true;
    }

    public boolean SendData(ByteBuffer data)
    {
        if(_sock == null || _sockInputStream == null)
            return false;

        try
        {
            _sockOutputStream.write(data.array());
            _dataWriteFailuresInRow = 0;
            return true;
        }
        catch(IOException e)
        {
            App.LogLine("Error on Socket.SendData with address: "
                    + _sock.getLocalAddress().toString() + " and port: "
                    + Integer.toString(_sock.getPort()) + "\n"
                    + e.getMessage());
            ++_dataWriteFailuresInRow;
            return false;
        }
    }

    public ByteBuffer GetData()
    {
        if(_sock == null || _sockInputStream == null)
            return null;

        return _inputStreamHelper.Read();
    }

    public boolean IsConnected()
    {
        return (_sock != null
                && _sock.isBound()
                && _sock.isConnected()
                && _inputStreamHelper != null
                && _inputStreamHelper.GetFailuresInRow() <= SOCK_MAX_READ_FAILURES_IN_ROW
                && _dataWriteFailuresInRow <= SOCK_MAX_WRITE_FAILURES_IN_ROW);
    }

    public SocketAddress GetAddress()
    {
        if(!IsConnected())
        {
            return BAD_ADDRESS;
        }
        return _sockAddress;
    }


    private static int SOCK_MAX_READ_FAILURES_IN_ROW = 16;
    private static int SOCK_MAX_WRITE_FAILURES_IN_ROW = 3;
    protected static InetSocketAddress BAD_ADDRESS = new InetSocketAddress("0.0.0.0", 0);

    protected Socket _sock = null;
    protected SocketAddress _sockAddress = null;
    protected OutputStream _sockOutputStream = null;
    protected InputStream _sockInputStream = null;
    protected DataStreamReadHelper _inputStreamHelper = new DataStreamReadHelper();
    protected int _dataWriteFailuresInRow = 0;
}
