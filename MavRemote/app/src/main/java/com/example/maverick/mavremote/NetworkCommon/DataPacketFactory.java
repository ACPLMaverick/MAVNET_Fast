package com.example.maverick.mavremote.NetworkCommon;

import android.provider.ContactsContract;
import android.util.Log;

import com.example.maverick.mavremote.Actions.ActionEvent;
import com.example.maverick.mavremote.App;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InvalidObjectException;
import java.io.ObjectInput;
import java.io.ObjectInputStream;
import java.io.ObjectOutput;
import java.io.ObjectOutputStream;
import java.io.Serializable;
import java.net.SocketAddress;
import java.nio.ByteBuffer;


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

    @SuppressWarnings("unchecked")
    public <T extends Serializable> void DecodePacket(ByteBuffer packet, DataPacketRetriever<T> outRetriever)
    {
        ByteArrayInputStream bis = new ByteArrayInputStream(packet.array());
        ObjectInput input = null;
        try
        {
            input = new ObjectInputStream(bis);
            try
            {
                DataPacketHeader header = (DataPacketHeader)input.readObject();
                if(header.AppId != AppId)
                {
                    throw new InvalidObjectException("AppId does not match.");
                }

                T obj = (T)input.readObject();
                input.close();
                outRetriever.ObjectRef = obj;
                outRetriever.ThisType = PacketType.values()[header.PacketType];
            }
            catch(Exception e)  // Simply. Because ClassCastException, NullPointerException and such can occur.
            {
                // Packet not decoded properly - nothing special.
                App.LogLine("Failed to extract data from packet: " + e.getMessage());
                input.close();
                outRetriever.ObjectRef = null;
                outRetriever.ThisType = PacketType.Unknown;
            }
        }
        catch(IOException e)
        {
            App.LogLine("Error decoding packet: " + e.getMessage());
            outRetriever.ObjectRef = null;
            outRetriever.ThisType = PacketType.Unknown;
        }
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
            App.LogLine("Error creating packet!\n" + e.getMessage());
            return null;
        }

        return ByteBuffer.wrap(finalBytes);
    }


    public static final int AppId = (App.TAG).hashCode();
}
