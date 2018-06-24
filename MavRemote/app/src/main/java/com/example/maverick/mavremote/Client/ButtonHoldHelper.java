package com.example.maverick.mavremote.Client;

import android.util.Log;
import android.widget.Button;

import com.example.maverick.mavremote.App;
import com.example.maverick.mavremote.Utility;

import java.util.Calendar;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.locks.ReentrantLock;

class HoldElement
{
    public HoldElement(Button btn, int eventPeriod, Runnable event)
    {
        Btn = btn;
        EventPeriod = eventPeriod;
        Timer = eventPeriod;    // So the event fires on the nearest update.
        Event = event;
    }

    public final Button Btn;
    public final Runnable Event;
    public final int EventPeriod;
    public int Timer;
}

public class ButtonHoldHelper
{
    public ButtonHoldHelper()
    {
    }

    public void Init()
    {
        _buttons = new HashMap<>();
        _lock = new ReentrantLock();
        InitDelta();
    }

    public void Update()
    {
        UpdateDelta();
        final int delta = GetDelta();

        _lock.lock();
        for(HoldElement element : _buttons.values())
        {
            element.Timer += delta;

            if(element.Timer >= element.EventPeriod)
            {
                element.Event.run();
                element.Timer = 0;
            }
        }
        _lock.unlock();
    }

    public void AddHoldButton(Button btn, int eventPeriodMillis, Runnable event)
    {
        if(_buttons.containsKey(btn))
        {
            Log.e(App.TAG, "[ButtonHoldHelper] Adding duplicate button! Aborting.");
            return;
        }

        _lock.lock();
        _buttons.put(btn, new HoldElement(btn, eventPeriodMillis, event));
        _lock.unlock();
    }

    public void RemoveHoldButton(Button btn)
    {
        if(!_buttons.containsKey(btn))
        {
            Log.e(App.TAG, "[ButtonHoldHelper] Removing a button that does not exist!. Aborting.");
            return;
        }

        _lock.lock();
        _buttons.remove(btn);
        _lock.unlock();
    }


    protected void InitDelta()
    {
        final long temp = _currentTime;
        _currentTime = Calendar.getInstance().getTimeInMillis();
        _prevTime = temp;
    }

    protected void UpdateDelta()
    {
        _prevTime = _currentTime;
        _currentTime = Calendar.getInstance().getTimeInMillis();
    }

    protected int GetDelta()
    {
        return (int)(_currentTime - _prevTime);
    }


    protected Map<Button, HoldElement> _buttons = null;
    protected ReentrantLock _lock = null;

    protected long _currentTime = 0;
    protected long _prevTime = 0;
}
