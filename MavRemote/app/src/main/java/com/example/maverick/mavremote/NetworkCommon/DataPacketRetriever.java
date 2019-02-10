package com.example.maverick.mavremote.NetworkCommon;

import com.example.maverick.mavremote.Utility;

public class DataPacketRetriever<T>
{
    public DataPacketRetriever()
    {
        ThisType = DataPacketFactory.PacketType.Unknown;
        ObjectRef = null;
    }

    public boolean IsValid() { return ObjectRef != null; }


    public DataPacketFactory.PacketType ThisType;
    public T ObjectRef;
}
