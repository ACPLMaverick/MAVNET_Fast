package com.example.maverick.mavremote.NetworkCommon;

import java.io.Serializable;

public class DataPacketHeader implements Serializable
{
    public DataPacketHeader(DataPacketFactory.PacketType packetType)
    {
        AppId = DataPacketFactory.AppId;
        PacketType = packetType.ordinal();
    }

    public DataPacketHeader(int packetType)
    {
        AppId = DataPacketFactory.AppId;
        PacketType = packetType;
    }


    public final int AppId;
    public final int PacketType;
}

