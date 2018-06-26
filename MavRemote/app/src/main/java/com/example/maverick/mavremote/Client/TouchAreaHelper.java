package com.example.maverick.mavremote.Client;

import android.util.Log;
import android.view.MotionEvent;
import android.view.View;

import com.example.maverick.mavremote.Actions.ActionEvent;
import com.example.maverick.mavremote.App;
import com.example.maverick.mavremote.Utility;

import java.util.ArrayList;
import java.util.Calendar;
import java.util.concurrent.locks.ReentrantLock;

class TouchTimer
{
    TouchTimer()
    {

    }

    void Start()
    {
        _bIsRunning = true;
        _timer = Calendar.getInstance().getTimeInMillis();
    }

    void Stop()
    {
        _bIsRunning = false;
        _timer = 0;
    }

    long GetTimeElapsedMillis()
    {
        if(_bIsRunning)
        {
            return Calendar.getInstance().getTimeInMillis() - _timer;
        }
        else
        {
            return 0;
        }
    }

    boolean IsRunning()
    {
        return _bIsRunning;
    }

    private long _timer = 0;
    private boolean _bIsRunning = false;
}

public class TouchAreaHelper
{
    public TouchAreaHelper() { }

    public void Init(View touchArea, float movementScale)
    {
        Utility.Assert(touchArea != null);
        _touchArea = touchArea;
        _movementScale = movementScale;

        _touchArea.post(new Runnable()
        {
            @Override
            public void run()
            {
                SetupExtents();
                SetupEvents();

                _bIsReady = true;
            }
        });
    }

    public void Update()
    {
        if(_timerFirstDown.IsRunning())
        {
            _lockProtector.lock();

            if(IsMove())
            {
                _timerFirstDown.Stop();
                _latestClickType = null;
            }
            else if(_timerFirstDown.GetTimeElapsedMillis() > MAX_TIME_FOR_LMB_MILLIS)
            {
                _timerFirstDown.Stop();
                _latestClickType = ActionEvent.MouseClickTypes.LMBDown;
                CallEventHandlersButtonAction();
            }

            _lockProtector.unlock();
        }
    }

    public void AssignEventHandlerToButtonAction(Runnable runnable)
    {
        _eventHandlersButtonAction.add(runnable);
    }

    public void AssignEventHandlerToMovement(Runnable runnable)
    {
        _eventHandlersMovement.add(runnable);
    }

    public ActionEvent.MouseClickTypes GetLatestClickType()
    {
        return _latestClickType;
    }

    public int GetLatestMovementDeltaX()
    {
        return (int)(_latestMovementDeltaX * _movementScale);
    }

    public int GetLatestMovementDeltaY()
    {
        return (int)(_latestMovementDeltaY * _movementScale);
    }

    public boolean IsReady()
    {
        return _bIsReady;
    }


    protected void SetupExtents()
    {
        final float width = _touchArea.getWidth();
        final float height = _touchArea.getHeight();

        // Top
        _globalExtents[0] = 0.0f;
        // Right
        _globalExtents[1] = width;
        // Bottom
        _globalExtents[2] = height;
        // Left
        _globalExtents[3] = 0.0f;

        Utility.Assert
        (
        _globalExtents[0] < _globalExtents[2]
                    && _globalExtents[3] < _globalExtents[1]
        );
    }

    protected void SetupEvents()
    {
        // Supported:
        // One finger click - LMB click
        // One finger hold after a tap - LMB hold
        // Two finger click - RMB click
        // Two finger hold after a tap - RMB hold
        // One finger move - move cursor.

        App.GetInstance().GetUIManager().PerformAction(new Runnable()
        {
            @Override
            public void run()
            {
                _touchArea.setOnTouchListener(new View.OnTouchListener()
                {
                    @Override
                    public boolean onTouch(View view, MotionEvent motionEvent)
                    {
                        if(motionEvent.getActionMasked() == MotionEvent.ACTION_MOVE)
                        {
                            if(IsTouchInViewArea(motionEvent))
                            {
                                if(motionEvent.getPointerCount() == 1)
                                {
                                    _lockProtector.lock();

                                    UpdateTouchDeltaMovementCoords(motionEvent);
                                    UpdateDistance(_latestMovementDeltaX, _latestMovementDeltaY);

                                    CallEventHandlersMovement();

                                    UpdateTouchMovementCoords(motionEvent);

                                    _lockProtector.unlock();
                                }
                            }
                        }
                        else if(motionEvent.getActionMasked() == MotionEvent.ACTION_UP)
                        {
                            _lockProtector.lock();

                            if(_timerFirstDown.IsRunning())
                            {
                                _timerFirstDown.Stop();
                                _latestClickType = ActionEvent.MouseClickTypes.LMBDown;
                                CallEventHandlersButtonAction();
                                _latestClickType = ActionEvent.MouseClickTypes.LMBUp;
                                CallEventHandlersButtonAction();
                            }
                            else if(_latestClickType != null)
                            {
                                _latestClickType = FlipMouseClick(_latestClickType);
                                CallEventHandlersButtonAction();
                            }

                            _lockProtector.unlock();

                            ClearOnUp();
                        }
                        else if(motionEvent.getActionMasked() == MotionEvent.ACTION_DOWN)
                        {
                            _lockProtector.lock();

                            Utility.Assert(!_timerFirstDown.IsRunning());
                            _timerFirstDown.Start();

                            _lockProtector.unlock();
                        }
                        else if(motionEvent.getActionMasked() == MotionEvent.ACTION_POINTER_DOWN)
                        {
                            _lockProtector.lock();

                            if(!IsMove())
                            {
                                _timerFirstDown.Stop();
                                _latestClickType = ActionEvent.MouseClickTypes.RMBDown;
                                CallEventHandlersButtonAction();
                            }

                            _lockProtector.unlock();
                        }
                        else
                        {
                            return false;
                        }

                        return true;
                    }
                });
            }
        });
    }

    protected void CallEventHandlersButtonAction()
    {
        for(Runnable runnable : _eventHandlersButtonAction)
        {
            runnable.run();
        }
    }

    protected void CallEventHandlersMovement()
    {
        for(Runnable runnable : _eventHandlersMovement)
        {
            runnable.run();
        }
    }

    protected boolean IsTouchInViewArea(MotionEvent motionEvent)
    {
        float x = motionEvent.getX();
        float y = motionEvent.getY();

        if
        (
            y > _globalExtents[0]
            && y < _globalExtents[2]
            && x > _globalExtents[3]
            && x < _globalExtents[1]
        )
        {
            return true;
        }

        return false;
    }

    protected void UpdateTouchMovementCoords(MotionEvent motionEvent)
    {
        _latestMovementX = motionEvent.getX();
        _latestMovementY = motionEvent.getY();
    }

    protected void UpdateTouchDeltaMovementCoords(MotionEvent motionEvent)
    {
        _latestMovementDeltaX = (motionEvent.getX() - _latestMovementX);
        _latestMovementDeltaY = (motionEvent.getY() - _latestMovementY);
    }

    protected void UpdateDistance(float dx, float dy)
    {
        _distanceThisTouch += Math.sqrt(dx * dx + dy * dy);
    }

    protected void ClearOnUp()
    {
        _latestMovementDeltaX = 0.0f;
        _latestMovementDeltaY = 0.0f;
        _latestMovementX = 0.0f;
        _latestMovementY = 0.0f;
        _distanceThisTouch = 0.0f;
        _latestClickType = null;
        _timerFirstDown.Stop();
    }

    protected boolean IsMove()
    {
        return _distanceThisTouch >= MIN_DISTANCE_TO_DISABLE_ACTION_BUTTONS;
    }

    protected ActionEvent.MouseClickTypes FlipMouseClick(ActionEvent.MouseClickTypes inClick)
    {
        switch (inClick)
        {
            case LMBUp:
                return ActionEvent.MouseClickTypes.LMBDown;
            case RMBUp:
                return ActionEvent.MouseClickTypes.RMBDown;
            case LMBDown:
                return ActionEvent.MouseClickTypes.LMBUp;
            default:
            case RMBDown:
                return ActionEvent.MouseClickTypes.RMBUp;
        }
    }


    protected static final int MIN_DISTANCE_TO_DISABLE_ACTION_BUTTONS = 2;
    protected static final int MAX_TIME_FOR_LMB_MILLIS = 750;


    protected View _touchArea = null;
    protected float _movementScale = 0.0f;
    protected final float[] _globalExtents = new float[4];  // Top, Right, Bottom, Left
    protected final ArrayList<Runnable> _eventHandlersButtonAction = new ArrayList<>();
    protected final ArrayList<Runnable> _eventHandlersMovement = new ArrayList<>();
    protected final ReentrantLock _lockProtector = new ReentrantLock();

    protected float _latestMovementX = 0;
    protected float _latestMovementY = 0;
    protected float _latestMovementDeltaX = 0;
    protected float _latestMovementDeltaY = 0;
    protected ActionEvent.MouseClickTypes _latestClickType = null;

    protected TouchTimer _timerFirstDown = new TouchTimer();
    protected float _distanceThisTouch = 0.0f;
    protected boolean _bIsReady = false;
}
