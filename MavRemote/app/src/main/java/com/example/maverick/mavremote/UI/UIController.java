package com.example.maverick.mavremote.UI;

import android.text.Editable;
import android.util.Log;
import android.widget.EditText;

import com.example.maverick.mavremote.App;
import com.example.maverick.mavremote.Utility;

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
        if(_etLog != null)
        {
            _manager.PerformAction(new Runnable()
            {
                @Override
                public void run()
                {
                    Editable et = _etLog.getText();
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

    protected UIManager _manager;

    protected EditText _etLog;
}
