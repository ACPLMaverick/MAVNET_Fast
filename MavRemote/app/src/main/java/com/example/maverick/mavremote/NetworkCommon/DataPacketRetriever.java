package com.example.maverick.mavremote.NetworkCommon;

import com.example.maverick.mavremote.Utility;

public class DataPacketRetriever<T>
{
    public DataPacketRetriever()
    {
        ThisType = DataPacketFactory.PacketType.Unknown;
        ObjectRef = null;
    }

    public DataPacketRetriever(DataPacketFactory.PacketType pType, T objRef)
    {
        Utility.Assert(objRef != null);
        ThisType = pType;
        ObjectRef = objRef;
    }

    public boolean IsValid() { return ObjectRef != null; }


    public final DataPacketFactory.PacketType ThisType;
    public final T ObjectRef;
}
