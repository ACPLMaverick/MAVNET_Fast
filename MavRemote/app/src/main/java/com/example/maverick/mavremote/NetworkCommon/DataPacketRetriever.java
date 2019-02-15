package com.example.maverick.mavremote.NetworkCommon;


public class DataPacketRetriever
{
    public DataPacketRetriever()
    {
        ThisType = DataPacketFactory.PacketType.Unknown;
        ObjectRef = null;
    }

    public boolean IsValid() { return ObjectRef != null; }


    public DataPacketFactory.PacketType ThisType;
    public Object ObjectRef;
}
