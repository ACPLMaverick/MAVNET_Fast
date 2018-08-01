package com.example.maverick.mavremote.Client;

import android.content.Context;
import android.graphics.Point;
import android.graphics.Rect;
import android.support.constraint.ConstraintLayout;
import android.text.Editable;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewTreeObserver;
import android.view.inputmethod.InputMethodManager;
import android.widget.EditText;

import com.example.maverick.mavremote.App;
import com.example.maverick.mavremote.EventQueue;
import com.example.maverick.mavremote.R;
import com.example.maverick.mavremote.UI.UIManager;
import com.example.maverick.mavremote.Utility;

import java.util.Calendar;

public class OSKHelper
{
    public void Init()
    {
        Utility.Assert(App.GetInstance().HasActivity());

        _motionEvents = new EventQueue<>();
        _motionEvents.Init();

         _currentState = new OSKState(false, "");

        _et = App.GetInstance().GetUIManager().GetMenu(UIManager.MenuType.ClientRemote)
                .GetEditTextViews().get(R.id.etOSKField);

        _clEt = App.GetInstance().GetUIManager()
                .GetMenu(UIManager.MenuType.ClientRemote).GetLayoutViews().get(R.id.clOSKField);

        App.GetInstance().GetUIManager().PerformAction(new Runnable()
        {
            @Override
            public void run()
            {
                _et.setText("");
                _et.setFocusable(false);
                _et.setFocusableInTouchMode(false);
                _clEt.setVisibility(View.INVISIBLE);
                _clEt.setMaxHeight(0);
            }
        });

        Point screenDimensions = new Point();
        App.GetInstance().GetActivity().getWindowManager().getDefaultDisplay().getSize(screenDimensions);
        _acceptRect = new Rect();
        _acceptRect.bottom = (int)((float)screenDimensions.y * 1.0f);
        _acceptRect.top = (int)((float)screenDimensions.y * 0.9f);
        _acceptRect.right = (int)((float)screenDimensions.x * 1.0f);
        _acceptRect.left = (int)((float)screenDimensions.x * 0.9f);

        App.GetInstance().GetUIManager().GetMenu(UIManager.MenuType.ClientRemote).GetRoot()
                .getViewTreeObserver()
                .addOnGlobalLayoutListener(new ViewTreeObserver.OnGlobalLayoutListener()
        {
            @Override
            public void onGlobalLayout()
            {
                GlobalLayoutHandler();
            }
        });
    }

    public void Update()
    {
        if(_bVisible)
        {
            String str = _et.getText().toString();
            if(str.contains("\n"))
            {
                // Meaning an "enter" was pressed and we need to close.
                _bAccepted = true;
                GetImm().hideSoftInputFromWindow(_et.getWindowToken(), 0);
                OnHide();
            }
        }

        if(_hidingTimer > 0)
        {
            long timeElapsed = Calendar.getInstance().getTimeInMillis() - _hidingTimer;
            if(timeElapsed >= HIDE_ACTION_DELAY_MILLIS)
            {
                OnHide();
            }
        }

        if(!_motionEvents.IsEmpty())
        {
            MotionEvent ev = _motionEvents.Dequeue();
            ProcessMotionEventAcceptPress(ev);
        }
    }

    public void Appear()
    {
        if(_bVisible)
            return;


//        App.GetInstance().GetUIManager().PerformAction(new Runnable()
//        {
//            @Override
//            public void run()
//            {
                _clEt.setVisibility(View.VISIBLE);
                _clEt.setMaxHeight(1920);
                _et.setFocusable(true);
                _et.setFocusableInTouchMode(true);
//            }
//        });

        boolean ret = _et.requestFocus();
        if(ret)
        {
            InputMethodManager imm = GetImm();
            imm.showSoftInput(_et, 0);
            /*
            try
            {
                Window win = App.GetInstance().GetActivity().getWindow();
                win.setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_STATE_ALWAYS_VISIBLE);
            }
            catch(NullPointerException e)
            {
                App.LogLine("[OSK] Bring up OSK failed: " + e.getMessage());
            }
            */
        }
        else
        {
            App.LogLine("[OSK] Set focus failed!");
        }

        _bAccepted = false;
        _keypadHeightPrev = GetLayoutKeypadHeight();

        _bVisible = true;
    }

    public void RegisterOnHideAction(Runnable runnable)
    {
        _hideAction = runnable;
    }

    public boolean IsVisible() { return _bVisible; }

    public OSKState GetCurrentState()
    {
        return _currentState;
    }

    public void PushMotionEvent(MotionEvent ev)
    {
        _motionEvents.Enqueue(ev);
    }

    private InputMethodManager GetImm()
    {
        return (InputMethodManager)
                (App.GetInstance().GetContext().getSystemService(Context.INPUT_METHOD_SERVICE));
    }

    private void OnHide()
    {
        if(!_bVisible)
            return;

        String str = _et.getText().toString();
        if(str.contains("\n"))
        {
            str = str.substring(0, str.length() - 1);
        }

        App.GetInstance().GetUIManager().PerformAction(new Runnable()
        {
            @Override
            public void run()
            {
                _clEt.setVisibility(View.INVISIBLE);
                _clEt.setMaxHeight(0);
                _et.clearFocus();
                _et.setFocusable(false);
                _et.setFocusableInTouchMode(false);
                _et.setText("");
            }
        });

        _currentState = new OSKState(_bAccepted, str);

        _bAccepted = false;
        _bVisible = false;
        _hidingTimer = 0;

        if(_hideAction != null)
        {
            _hideAction.run();
        }
    }

    private void GlobalLayoutHandler()
    {
        if(!_bVisible)
            return;

        if(_hidingTimer > 0)    // Already know we're hiding so don't process further.
            return;

        final int keypadHeight = GetLayoutKeypadHeight();
        final int heightDiff = keypadHeight - _keypadHeightPrev;
        _keypadHeightPrev = keypadHeight;

        if(heightDiff < 0) // meaning kb is getting smaller, i.e. is hiding.
        {
            // This trigger delay for hide action.
            _hidingTimer = Calendar.getInstance().getTimeInMillis();
        }
    }

    private int GetLayoutKeypadHeight()
    {
        View root = App.GetInstance().GetUIManager().GetMenu(UIManager.MenuType.ClientRemote).GetRoot();
        Rect r = new Rect();
        root.getWindowVisibleDisplayFrame(r);
        int screenHeight = root.getRootView().getHeight();
        int keypadHeight = screenHeight - r.bottom;

        return keypadHeight;
    }

    private void ProcessMotionEventAcceptPress(MotionEvent ev)
    {
        if(_bVisible)
        {
            if(!_bAccepted)
            {
                if(ev.getAction() == MotionEvent.ACTION_UP)
                {
                    if(_acceptRect.contains((int)ev.getX(), (int)ev.getY()))
                    {
                        _bAccepted = true;
                    }
                }
            }
        }
    }


    private static final long HIDE_ACTION_DELAY_MILLIS = 1;

    private ConstraintLayout _clEt = null;
    private EditText _et = null;

    private Runnable _hideAction = null;

    private Rect _acceptRect = null;
    private boolean _bVisible = false;
    private boolean _bAccepted = false;
    private int _keypadHeightPrev = 0;
    private long _hidingTimer = 0;

    private OSKState _currentState = null;

    private EventQueue<MotionEvent> _motionEvents = null;
}
