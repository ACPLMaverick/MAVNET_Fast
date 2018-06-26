package com.example.maverick.mavremote.NetworkCommon;

import android.util.Log;

import com.example.maverick.mavremote.App;

import java.io.IOException;
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
            Log.e(App.TAG, "Error creating socket with given address: "
                    + address.toString() + "\n" + e.getMessage());
            _sock = null;
            return false;
        }

        return _sock.isBound() && _sock.isConnected();
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
            Log.e(App.TAG, "Error closing socket with address: "
                    + _sock.getLocalAddress().toString() + " and port: "
                    + Integer.toString(_sock.getPort()) + "\n"
                    + e.getMessage());

            return false;
        }

        return true;
    }

    public boolean SendData(ByteBuffer data)
    {
        if(_sock == null)
            return false;

        try
        {
            int bytesWritten = _sock.getChannel().write(data);
            if(bytesWritten == data.array().length)
                return true;
            else
                return false;
        }
        catch(IOException e)
        {
            Log.e(App.TAG, "Error on Socket.SendData with address: "
                    + _sock.getLocalAddress().toString() + " and port: "
                    + Integer.toString(_sock.getPort()) + "\n"
                    + e.getMessage());
            return false;
        }
    }

    public Byte[] GetData()
    {
        if(_sock == null)
            return null;

        try
        {
            // First test if there is anything to read
            int testRead = _sock.getChannel().read(_testBuffer);
            if(testRead <= 0)
                return null;

            // Read all data
            ArrayList<Byte> allBytes = new ArrayList<>();
            allBytes.add(_testBuffer.array()[0]);
            int bytesRead = 0;
            do
            {
                bytesRead = _sock.getChannel().read(_readBuffer);
                for(int i = 0; i < bytesRead; ++i)
                {
                    allBytes.add(_readBuffer.array()[i]);
                }
            }while(bytesRead > 0);

            return (Byte[])allBytes.toArray();
        }
        catch(IOException e)
        {
            Log.e(App.TAG, "Error on Socket.GetData with address: "
                    + _sock.getLocalAddress().toString() + " and port: "
                    + Integer.toString(_sock.getPort()) + "\n"
                    + e.getMessage());
            return null;
        }
    }

    public ByteBuffer GetDataBB()
    {
        Byte[] bytes = GetData();
        if(bytes == null || bytes.length == 0)
            return null;

        byte[] bytesFundamental = new byte[bytes.length];
        for(int i = 0; i < bytes.length; ++i)
            bytesFundamental[i] = bytes[i];

        return ByteBuffer.wrap(bytesFundamental);
    }


    protected static final int READ_BUFFER_SIZE = 1024;

    protected Socket _sock = null;
    protected ByteBuffer _readBuffer = ByteBuffer.allocate(READ_BUFFER_SIZE);
    protected ByteBuffer _testBuffer = ByteBuffer.allocate(1);
}
