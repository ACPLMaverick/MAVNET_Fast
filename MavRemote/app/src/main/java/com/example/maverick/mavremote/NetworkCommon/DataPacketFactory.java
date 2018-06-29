package com.example.maverick.mavremote.NetworkCommon;

import android.provider.ContactsContract;
import android.util.Log;

import com.example.maverick.mavremote.Actions.ActionEvent;
import com.example.maverick.mavremote.App;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.ObjectOutput;
import java.io.ObjectOutputStream;
import java.io.Serializable;
import java.net.SocketAddress;
import java.nio.ByteBuffer;


class DataPacketHeader implements Serializable
{
    public DataPacketHeader(int packetType)
    {
        AppId = DataPacketFactory.AppId;
        PacketType = packetType;
    }


    public final int AppId;
    public final int PacketType;
}

public class DataPacketFactory
{
    public enum PacketType
    {
        Unknown,
        Broadcast,
        ActionEvent
    }

    public DataPacketFactory()
    {
    }

    public ByteBuffer CreatePacket(byte[] bytes, PacketType type)
    {
        DataPacketHeader header = new DataPacketHeader(type.ordinal());
        return CreatePacketInternal(header, bytes);
    }

    public ByteBuffer CreatePacket(Byte[] bytes, PacketType type)
    {
        DataPacketHeader header = new DataPacketHeader(type.ordinal());
        return CreatePacketInternal(header, bytes);
    }

    public ByteBuffer CreatePacket(ActionEvent actionEvent)
    {
        DataPacketHeader header = new DataPacketHeader(PacketType.ActionEvent.ordinal());
        return CreatePacketInternal(header, actionEvent);
    }

    public ByteBuffer CreatePacketBroadcast(SocketAddress address)
    {
        DataPacketHeader header = new DataPacketHeader(PacketType.Broadcast.ordinal());
        return CreatePacketInternal(header, address);
    }


    private ByteBuffer CreatePacketInternal(DataPacketHeader header, Serializable serializable)
    {
        ByteArrayOutputStream bos = new ByteArrayOutputStream();
        ObjectOutput out = null;
        byte[] finalBytes;
        try
        {
            out = new ObjectOutputStream(bos);
            out.writeObject(header);
            out.writeObject(serializable);
            out.flush();
            finalBytes = bos.toByteArray();
        }
        catch(IOException e)
        {
            Log.e(App.TAG, "Error creating packet!\n" + e.getMessage());
            return null;
        }

        return ByteBuffer.wrap(finalBytes);
    }


    public static final int AppId = (App.TAG).hashCode();
}
