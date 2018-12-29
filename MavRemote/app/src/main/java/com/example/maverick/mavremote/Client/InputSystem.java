package com.example.maverick.mavremote.Client;

import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;

import com.example.maverick.mavremote.Actions.ActionEvent;
import com.example.maverick.mavremote.Actions.Movement;
import com.example.maverick.mavremote.App;
import com.example.maverick.mavremote.EventQueue;
import com.example.maverick.mavremote.R;
import com.example.maverick.mavremote.System;
import com.example.maverick.mavremote.UI.Menu;
import com.example.maverick.mavremote.UI.UIManager;

import java.util.HashMap;

import static com.example.maverick.mavremote.Client.AppClient.GetInstance;

public class InputSystem extends System
{
    public static final int[] KEYS_BLACKLISTED_ON_DEVICE =
    {
        KeyEvent.KEYCODE_VOLUME_UP,
        KeyEvent.KEYCODE_VOLUME_DOWN,
        KeyEvent.KEYCODE_BACK
    };

    // Can return null if queue empty.
    public ActionEvent PopEvent()
    {
        if(_eventQueue == null || _eventQueue.IsEmpty())
        {
            return null;
        }
        else
        {
            return _eventQueue.Dequeue();
        }
    }

    @Override
    protected void Start()
    {
        _buttonHoldHelper = new ButtonHoldHelper();
        _buttonHoldHelper.Init();

        _longClickHelper = new HashMap<>();

        _touchAreaHelper = new TouchAreaHelper();
        _scrollAreaHelper = new TouchAreaHelper();

        _eventQueue = new EventQueue<>();
        _eventQueue.Init();

        _oskHelper = new OSKHelper();
        _oskHelper.Init();
        _oskHelper.RegisterOnHideAction(new Runnable()
        {
            @Override
            public void run()
            {
                OnOSKHide();
            }
        });

        Button btn;

        // Register onClick with hold methods for remote buttons.
        btn = GetMenu().GetButtons().get(R.id.btnDpadUp);
        AssignHoldEventToButton(btn, KeyEvent.KEYCODE_DPAD_UP);

        btn = GetMenu().GetButtons().get(R.id.btnDpadRight);
        AssignHoldEventToButton(btn, KeyEvent.KEYCODE_DPAD_RIGHT);

        btn = GetMenu().GetButtons().get(R.id.btnDpadDown);
        AssignHoldEventToButton(btn, KeyEvent.KEYCODE_DPAD_DOWN);

        btn = GetMenu().GetButtons().get(R.id.btnDpadLeft);
        AssignHoldEventToButton(btn, KeyEvent.KEYCODE_DPAD_LEFT);

        // Dpad center gets simple click
        btn = GetMenu().GetButtons().get(R.id.btnDpadCenter);
        AssignClickEventToButton(btn, KeyEvent.KEYCODE_ENTER);

        // Other click buttons.

        btn = GetMenu().GetButtons().get(R.id.btnBack);
        AssignClickEventToButton(btn, KeyEvent.KEYCODE_BACK);

        btn = GetMenu().GetButtons().get(R.id.btnHome);
        AssignClickEventToButton(btn, KeyEvent.KEYCODE_HOME);

        btn = GetMenu().GetButtons().get(R.id.btnApps);
        // Task manager needs to be handled with a special key code.
        AssignClickEventToButton(btn, ActionEvent.GetIntFromSpecialKeyEvent(ActionEvent.SpecialKeyEvent.TaskManager));

        btn = GetMenu().GetButtons().get(R.id.btnMediaPause);
        AssignClickEventToButton(btn, KeyEvent.KEYCODE_MEDIA_PAUSE);

        btn = GetMenu().GetButtons().get(R.id.btnMediaPlay);
        AssignClickEventToButton(btn, KeyEvent.KEYCODE_MEDIA_PLAY);

        btn = GetMenu().GetButtons().get(R.id.btnMediaForward);
        AssignHoldAndClickEventsToButton(btn, KeyEvent.KEYCODE_MEDIA_FAST_FORWARD,
                KeyEvent.KEYCODE_MEDIA_PLAY, KeyEvent.KEYCODE_MEDIA_NEXT);

        btn = GetMenu().GetButtons().get(R.id.btnMediaBackwards);
        AssignHoldAndClickEventsToButton(btn, KeyEvent.KEYCODE_MEDIA_REWIND,
                KeyEvent.KEYCODE_MEDIA_PLAY, KeyEvent.KEYCODE_MEDIA_PREVIOUS);

        btn = GetMenu().GetButtons().get(R.id.btnMute);
        AssignClickEventToButton(btn, KeyEvent.KEYCODE_MUTE);

        btn = GetMenu().GetButtons().get(R.id.btnScreen);
        AssignClickEventToButton(btn, KeyEvent.KEYCODE_SYSRQ);

        btn = GetMenu().GetButtons().get(R.id.btnRec);
        AssignClickEventToButton(btn, KeyEvent.KEYCODE_MEDIA_RECORD);

        btn = GetMenu().GetButtons().get(R.id.btnBackspace);
        AssignClickEventToButton(btn, KeyEvent.KEYCODE_DEL, new Runnable()
        {
            @Override
            public void run()
            {
                _oskHelper.OnBackspaceBtnClicked();
            }
        });
        // TODO: Maybe implement some general-purpose recording?
        // TODO: Button state changes in case of MUTE and RECORD.

        btn = GetMenu().GetButtons().get(R.id.btnOsk);
        btn.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View view)
            {
                OSKAppear();
            }
        });

        // Ideas for other buttons:


        // Set up touch and scroll/swipe field areas and register for touch events.
        SetupTouchArea();
        SetupScrollArea();


        _bIsRunning = true;
    }

    @Override
    protected void Finish()
    {

    }

    @Override
    protected void MainLoop()
    {
        // Poll volume regulation from ClientActivity.
        if(GetInstance().HasActivity())
        {
            if (!GetInstance().GetActivityTyped().GetSystemKeyEvents().IsEmpty())
            {
                KeyEvent ke = GetInstance().GetActivityTyped().GetSystemKeyEvents().Dequeue();

                if (
                        ke.getKeyCode() == KeyEvent.KEYCODE_VOLUME_UP
                                || ke.getKeyCode() == KeyEvent.KEYCODE_VOLUME_DOWN
                        )
                {
                    PassKeyboardEvent(ke.getKeyCode());
                }
                else if(ke.getKeyCode() == KeyEvent.KEYCODE_BACK)
                {
                    // Do not care about OSK because it consumes input.
                    GetInstance().OnBackButtonPressed();
                }
            }
        }

        // Update Hold Helper.
        _buttonHoldHelper.Update();
        _touchAreaHelper.Update();
        _scrollAreaHelper.Update();

        _oskHelper.Update();
    }

    private void AssignClickEventToButton(final Button button, final int kbEvent)
    {
        AssignClickEventToButton(button, kbEvent, null);
    }

    private void AssignClickEventToButton(final Button button, final int kbEvent, final Runnable runnable)
    {
        App.GetInstance().GetUIManager().PerformAction(new Runnable()
        {
            @Override
            public void run()
            {
                button.setOnClickListener(new View.OnClickListener()
                {
                    @Override
                    public void onClick(View view)
                    {
                        PassKeyboardEvent(kbEvent);
                        if(runnable != null)
                            runnable.run();
                    }
                });
            }
        });
    }

    private void AssignHoldEventToButton(final Button button, final int kbEvent)
    {
        App.GetInstance().GetUIManager().PerformAction(new Runnable()
        {
            @Override
            public void run()
            {
                button.setOnTouchListener(new View.OnTouchListener()
                {
                    @Override
                    public boolean onTouch(View view, MotionEvent motionEvent)
                    {
                        switch (motionEvent.getAction())
                        {
                            case MotionEvent.ACTION_DOWN:
                            {
                                _buttonHoldHelper.AddHoldButton(
                                        (Button) view,
                                        BUTTON_HOLD_PERIOD_MILLIS,
                                        new Runnable()
                                        {
                                            @Override
                                            public void run()
                                            {
                                                PassKeyboardEvent(kbEvent);
                                            }
                                        });
                            }
                            break;
                            case MotionEvent.ACTION_UP:
                            {
                                _buttonHoldHelper.RemoveHoldButton((Button) view);
                            }
                            break;
                            default:
                            break;
                        }
                        return false;
                    }
                });
            }
        });
    }

    private void AssignHoldAndClickEventsToButton(final Button button, final int kbEventHoldIn,
                                                  final int kbEventHoldOut, final int kbEventClick)
    {
        App.GetInstance().GetUIManager().PerformAction(new Runnable()
        {
            @Override
            public void run()
            {
                button.setOnClickListener(new View.OnClickListener()
                {
                    @Override
                    public void onClick(View view)
                    {
                        PassKeyboardEvent(kbEventClick);
                    }
                });

                button.setOnLongClickListener(new View.OnLongClickListener()
                {
                    @Override
                    public boolean onLongClick(View view)
                    {
                        _longClickHelper.put((Button)view, (Button)view);
                        PassKeyboardEvent(kbEventHoldIn);
                        return true;
                    }
                });

                button.setOnTouchListener(new View.OnTouchListener()
                {
                    @Override
                    public boolean onTouch(View view, MotionEvent motionEvent)
                    {
                        switch (motionEvent.getAction())
                        {
                            case MotionEvent.ACTION_UP:
                            {
                                final Button btn = (Button)view;
                                if(_longClickHelper.containsKey(btn))
                                {
                                    _longClickHelper.remove(btn);
                                    PassKeyboardEvent(kbEventHoldOut);
                                }
                            }
                            break;
                            default:
                                break;
                        }
                        return false;
                    }
                });
            }
        });
    }

    private void SetupTouchArea()
    {
        _touchAreaHelper.Init(GetMenu().GetImageViews().get(R.id.ivTouchArea), TOUCH_SCALE);
        _touchAreaHelper.AssignEventHandlerToMovement(new Runnable()
        {
            @Override
            public void run()
            {
                PassMovement
                (
                        _touchAreaHelper.GetLatestMovementDeltaX(),
                        _touchAreaHelper.GetLatestMovementDeltaY()
                );
            }
        });
        _touchAreaHelper.AssignEventHandlerToButtonAction(new Runnable()
        {
            @Override
            public void run()
            {
                PassMouseClick(_touchAreaHelper.GetLatestClickType());
            }
        });
    }

    private void SetupScrollArea()
    {
        _scrollAreaHelper.Init(GetMenu().GetImageViews().get(R.id.ivScrollArea), SCROLL_SCALE);
        _scrollAreaHelper.AssignEventHandlerToMovement(new Runnable()
        {
            @Override
            public void run()
            {
                PassScroll(-_scrollAreaHelper.GetLatestMovementDeltaY());
            }
        });
    }

    private Menu GetMenu()
    {
        return App.GetInstance().GetUIManager().GetMenu(UIManager.MenuType.ClientRemote);
    }

    private void OSKAppear()
    {
        final Button btn = GetMenu().GetButtons().get(R.id.btnOsk);
        if(btn.isEnabled())
        {
            App.GetInstance().GetUIManager().PerformAction(new Runnable()
            {
                @Override
                public void run()
                {
                    btn.setEnabled(false);
                }
            });
        }

        _oskHelper.Appear();
    }

    private void OnOSKHide()
    {
        final Button btn = GetMenu().GetButtons().get(R.id.btnOsk);
        if(!btn.isEnabled())
        {
            App.GetInstance().GetUIManager().PerformAction(new Runnable()
            {
                @Override
                public void run()
                {
                    btn.setEnabled(true);
                }
            });
        }

        OSKState state = _oskHelper.GetCurrentState();
        if(state.IsValid())
        {
            PassOSK(state.GetText());
        }
    }

    private void PassKeyboardEvent(int kbEvent)
    {
        App.LogLine("[InputSystem] Pressing key: " + KeyEvent.keyCodeToString(kbEvent));

        ActionEvent ev = new ActionEvent(kbEvent);

        _eventQueue.Enqueue(ev);
    }

    private void PassMouseClick(ActionEvent.MouseClickTypes mouseClick)
    {
        App.LogLine("[InputSystem] Mouse button: " + mouseClick.toString());

        ActionEvent ev = new ActionEvent(mouseClick);

        _eventQueue.Enqueue(ev);
    }

    private void PassMovement(int dx, int dy)
    {
        App.LogLine("[InputSystem] Movement: " + Integer.toString(dx) + ", "  + Integer.toString(dy));

        ActionEvent ev = new ActionEvent(new Movement(dx, dy));

        _eventQueue.Enqueue(ev);
    }

    private void PassScroll(int dy)
    {
        App.LogLine("[InputSystem] Scroll: " + Integer.toString(dy));

        ActionEvent ev = new ActionEvent(new Movement(dy));

        _eventQueue.Enqueue(ev);
    }

    private void PassOSK(final String oskText)
    {
        App.LogLine("Sending OSK event with text: " + oskText);

        ActionEvent ev = new ActionEvent(oskText);

        _eventQueue.Enqueue(ev);
    }


    private static final int BUTTON_HOLD_PERIOD_MILLIS = 750;
    private static final float TOUCH_SCALE = 1.3f;
    private static final float SCROLL_SCALE = 0.1f;

    private ButtonHoldHelper _buttonHoldHelper = null;
    private TouchAreaHelper _touchAreaHelper = null;
    private TouchAreaHelper _scrollAreaHelper = null;
    private HashMap<Button, Button> _longClickHelper = null;
    private OSKHelper _oskHelper = null;

    private EventQueue<ActionEvent> _eventQueue = null;
}
