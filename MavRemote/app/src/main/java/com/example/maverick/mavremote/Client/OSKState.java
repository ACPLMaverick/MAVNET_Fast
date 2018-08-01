package com.example.maverick.mavremote.Client;

public class OSKState
{
    public OSKState(boolean _bIsAccepted, String _text)
    {
        this._bIsAccepted = _bIsAccepted;
        this._text = _text;
    }

    public boolean IsValid()
    {
        return IsAccepted() && _text != null && !_text.equals("");
    }

    public boolean IsAccepted()
    {
        return _bIsAccepted;
    }

    public String GetText()
    {
        return _text;
    }

    private boolean _bIsAccepted;
    private String _text;
}
