package com.example.maverick.mavremote.Server;

public class PacketCounter
{
    public PacketCounter()
    {
        Reset();
    }

    public PacketCounter(final PacketCounter copy)
    {
        _packetNumCorrect = copy._packetNumCorrect;
        _packetNumWrong = copy._packetNumWrong;
    }

    public int GetPacketNumCorrect()
    {
        return _packetNumCorrect;
    }

    public int GetPacketNumWrong()
    {
        return _packetNumWrong;
    }

    public int GetPacketNumTotal()
    {
        return _packetNumWrong + _packetNumCorrect;
    }

    public void IncPacketNumCorrect()
    {
        ++_packetNumCorrect;
    }

    public void IncPacketNumWrong()
    {
        ++_packetNumWrong;
    }

    public void Reset()
    {
        _packetNumCorrect = 0;
        _packetNumWrong = 0;
    }

    public boolean IsTheSame(final PacketCounter other)
    {
        return _packetNumCorrect == other._packetNumCorrect && _packetNumWrong == other._packetNumWrong;
    }

    @Override
    public String toString()
    {
        return "OK: " + GetPacketNumCorrect()
                + ", Wrong: " + GetPacketNumWrong()
                + ", Total: " + GetPacketNumTotal();
    }

    private int _packetNumCorrect;
    private int _packetNumWrong;
}
