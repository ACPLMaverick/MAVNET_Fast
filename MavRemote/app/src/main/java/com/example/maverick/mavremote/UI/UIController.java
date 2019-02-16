package com.example.maverick.mavremote.UI;

import android.graphics.Color;
import android.text.Editable;
import android.util.Log;
import android.widget.EditText;
import android.widget.TextView;

import com.example.maverick.mavremote.App;
import com.example.maverick.mavremote.NetworkCommon.NetworkSystem;
import com.example.maverick.mavremote.Utility;

import java.net.InetSocketAddress;
import java.net.SocketAddress;

public abstract class UIController
{
    public UIController()
    {
    }

    public void Init(UIManager manager)
    {
        Utility.Assert(manager != null);
        _manager = manager;
        AcquireUIElements();
        AcquireLogElement();

        if(_etLog != null)
        {
            _manager.PerformAction(new Runnable()
            {
                @Override
                public void run()
                {
                    _etLog.setText("");
                }
            });
        }
    }

    public void Update()
    {

    }

    public void Cleanup()
    {
        _manager = null;
    }

    public void Log(final String text)
    {
        if(_etLog != null && _manager != null)
        {
            _manager.PerformAction(new Runnable()
            {
                @Override
                public void run()
                {
                    Editable et = _etLog.getText();
                    if(et.length() > 16384)
                    {
                        et.clear();
                    }
                    et.append(text);
                }
            });
        }

        Log.e(App.TAG, "[InternalLog] " + text);
    }

    public void LogLine(final String text)
    {
        Log("\n" + text);
    }

    protected abstract void AcquireUIElements();
    protected void AcquireLogElement()
    {
        _etLog = null;
    }


    protected void UpdateStatusTextView(final TextView statusTextView, final NetworkSystem.State state)
    {
        _manager.PerformAction(new Runnable()
        {
            @Override
            public void run()
            {
                statusTextView.setText(STATE_NAMES[state.ordinal()]);
                statusTextView.setTextColor(STATE_COLORS[state.ordinal()]);
            }
        });
    }

    protected void UpdateAddressTextView(final TextView addressTextView, final SocketAddress address)
    {
        final String addrString = address != null
                ? ((InetSocketAddress)address).getAddress().getCanonicalHostName()
                : "0.0.0.0";

        _manager.PerformAction(new Runnable()
        {
            @Override
            public void run()
            {
                addressTextView.setText(addrString);
            }
        });
    }

    ///////// ClientNetwork
    protected static final String[] STATE_NAMES =
            {
                    "Invalid",
                    "Not connected",
                    "Connecting...",
                    "Connected"
            };
    protected static final int[] STATE_COLORS =
            {
                    Color.BLACK,
                    Color.RED,
                    Color.YELLOW,
                    Color.GREEN
            };

    protected UIManager _manager;

    protected EditText _etLog;
}
