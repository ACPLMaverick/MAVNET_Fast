package com.example.maverick.mavremote.NetworkCommon;

import android.util.Log;

import com.example.maverick.mavremote.App;
import com.example.maverick.mavremote.Utility;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetSocketAddress;
import java.net.SocketAddress;
import java.net.SocketException;
import java.nio.ByteBuffer;

public class EndpointDatagram
{
    public enum Mode
    {
        Common,
        Broadcast
    }

    public void Init(ConnectivityHelper helper, Mode mode, int port)
    {
        _mode = mode;
        _helper = helper;

        try
        {
            _sock = new DatagramSocket(port);

            if(mode == Mode.Broadcast)
            {
                _sock.setBroadcast(true);

                _sock.setSendBufferSize(BROADCAST_BUFFER_SIZE);
                _sock.setReceiveBufferSize(BROADCAST_BUFFER_SIZE);
                _buffer = ByteBuffer.allocate(BROADCAST_BUFFER_SIZE);
            }
            else if(mode == Mode.Common)
            {
                _sock.setSendBufferSize(COMMON_BUFFER_SIZE);
                _sock.setReceiveBufferSize(COMMON_BUFFER_SIZE);
                _buffer = ByteBuffer.allocate(COMMON_BUFFER_SIZE);
            }
            else
            {
                Utility.Assert(false);
            }
        }
        catch(SocketException e)
        {
            App.LogLine("Error on creating EndpointDatagram: " + e.getMessage());
            return;
        }
    }

    public void Close()
    {
        _sock.close();
    }

    public boolean SendData(SocketAddress address, ByteBuffer buffer)
    {
        if(_sock == null)
            return false;

        try
        {
            DatagramPacket packet = new DatagramPacket(buffer.array(), buffer.array().length, address);
            _sock.send(packet);
        }
        catch(IOException e)
        {
            App.LogLine("Error on sending Datagram message: " + e.getMessage());
            return false;
        }

        return true;
    }

    public boolean SendDataBroadcast(ByteBuffer buffer)
    {
        if(_sock == null)
            return false;

        try
        {
            SocketAddress broadcast = _helper.GetBroadcastAddress();
            DatagramPacket packet = new DatagramPacket(buffer.array(), buffer.array().length, broadcast);
            _sock.send(packet);
        }
        catch(IOException e)
        {
            App.LogLine("Error on sending Broadcast message: " + e.getMessage());
            return false;
        }

        return true;
    }

    // Will block until data is available.
    // If the message is longer than the packet's length, the message is truncated.
    public ByteBuffer GetData(SocketAddress address)
    {
        if(_sock == null)
            return null;

        try
        {
            _sock.connect(address);
            DatagramPacket packet = new DatagramPacket(_buffer.array(), _buffer.array().length);
            _sock.receive(packet);

            return ByteBuffer.wrap(packet.getData());
        }
        catch(IOException e)
        {
            App.LogLine("Error on receiving Datagram message: " + e.getMessage());
            return null;
        }
    }

    // Will block until data is available.
    // If the message is longer than the packet's length, the message is truncated.
    public ByteBuffer GetDataBroadcast()
    {
        if(_sock == null)
            return null;

        try
        {
            DatagramPacket packet = new DatagramPacket(_buffer.array(), _buffer.array().length);
            _sock.receive(packet);

            return ByteBuffer.wrap(packet.getData());
        }
        catch(IOException e)
        {
            App.LogLine("Error on receiving Datagram message: " + e.getMessage());
            return null;
        }
    }

    public Mode GetMode()
    {
        return _mode;
    }


    private static final int COMMON_BUFFER_SIZE = 1024;
    private static final int BROADCAST_BUFFER_SIZE = 1024;

    private DatagramSocket _sock;
    private ConnectivityHelper _helper;
    private Mode _mode;

    private ByteBuffer _buffer = null;
}
