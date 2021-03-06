package com.example.maverick.mavremote.Client;

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
import com.example.maverick.mavremote.Utility;

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
        AssignHoldEventToButton(btn, KeyEvent.KEYCODE_DPAD_UP, BUTTON_HOLD_DPAD_PERIOD_MILLIS);

        btn = GetMenu().GetButtons().get(R.id.btnDpadRight);
        AssignHoldEventToButton(btn, KeyEvent.KEYCODE_DPAD_RIGHT, BUTTON_HOLD_DPAD_PERIOD_MILLIS);

        btn = GetMenu().GetButtons().get(R.id.btnDpadDown);
        AssignHoldEventToButton(btn, KeyEvent.KEYCODE_DPAD_DOWN, BUTTON_HOLD_DPAD_PERIOD_MILLIS);

        btn = GetMenu().GetButtons().get(R.id.btnDpadLeft);
        AssignHoldEventToButton(btn, KeyEvent.KEYCODE_DPAD_LEFT, BUTTON_HOLD_DPAD_PERIOD_MILLIS);

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

        btn = GetMenu().GetButtons().get(R.id.btnMediaPlayPause);
        AssignClickEventToButton(btn, KeyEvent.KEYCODE_MEDIA_PLAY);

        btn = GetMenu().GetButtons().get(R.id.btnMediaSubtitles);
        AssignClickEventToButton(btn, ActionEvent.GetIntFromSpecialKeyEvent(ActionEvent.SpecialKeyEvent.Subtitles));

        btn = GetMenu().GetButtons().get(R.id.btnMediaForward);
        AssignHoldAndClickEventsToButton(btn, KeyEvent.KEYCODE_MEDIA_FAST_FORWARD, KeyEvent.KEYCODE_MEDIA_NEXT);

        btn = GetMenu().GetButtons().get(R.id.btnMediaBackwards);
        AssignHoldAndClickEventsToButton(btn, KeyEvent.KEYCODE_MEDIA_REWIND, KeyEvent.KEYCODE_MEDIA_PREVIOUS);

        btn = GetMenu().GetButtons().get(R.id.btnMute);
        AssignClickEventToButton(btn, KeyEvent.KEYCODE_MUTE);

        btn = GetMenu().GetButtons().get(R.id.btnScreen);
        AssignClickEventToButton(btn, KeyEvent.KEYCODE_SYSRQ);

        btn = GetMenu().GetButtons().get(R.id.btnLock);
        AssignClickEventToButton(btn, KeyEvent.KEYCODE_POWER);

        btn = GetMenu().GetButtons().get(R.id.btnBackspace);
        AssignHoldEventToButton(btn, KeyEvent.KEYCODE_DEL, BUTTON_HOLD_DEL_PERIOD_MILLIS);
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
                    PassKeyboardEvent(ke.getKeyCode(), false);
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

    private void AssignHoldEventToButton(final Button button, final int kbEvent, final int holdPeriodMillis)
    {
        AssignHoldEventToButton(button, kbEvent, holdPeriodMillis, new Runnable()
        {
            @Override
            public void run()
            {
                PassKeyboardEvent(kbEvent, !_buttonHoldHelper.GetLastHasVibrated());
            }
        });
    }

    private void AssignHoldEventToButton(final Button button, final int kbEvent,
                                         final int holdPeriodMillis, final Runnable runnable)
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
                                        holdPeriodMillis,
                                        runnable);
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

    private void AssignHoldAndClickEventsToButton(final Button button, final int kbEventHold, final int kbEventClick)
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
                        PassKeyboardHoldEvent(kbEventHold, false);
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
                                    PassKeyboardHoldEvent(kbEventHold, true);
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
                ProcessScroll(-_scrollAreaHelper.GetLatestMovementDeltaYFlt());
            }
        });
        _scrollAreaHelper.AssignEventHandlerToButtonAction(new Runnable()
        {
            @Override
            public void run()
            {
                ResetScroll();
            }
        });
    }

    private void ProcessScroll(float scroll)
    {
        int scrollInt;
        if(Math.abs(scroll) < 1.0f)
        {
            _scrollAccumulator += scroll;
            scrollInt = (int)_scrollAccumulator;
            _scrollAccumulator -= (float)scrollInt;
        }
        else
        {
            ResetScroll();
            scrollInt = (int)scroll;
        }

        if(scrollInt != 0)
        {
            PassScroll(scrollInt);
        }
    }

    private void ResetScroll()
    {
        _scrollAccumulator = 0.0f;
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
        PassKeyboardEvent(kbEvent, true);
    }

    private void PassKeyboardEvent(int kbEvent, boolean bVibrate)
    {
        if(B_ENABLE_LOGGING)
            App.LogLine("[InputSystem] Pressing key: " + KeyEvent.keyCodeToString(kbEvent));

        if(bVibrate)
        {
            Utility.Vibrate(MOUSE_OR_BUTTON_CLICK_VIBRATION_TIME_MS);
        }

        ActionEvent ev = new ActionEvent(kbEvent);
        _eventQueue.Enqueue(ev);
    }

    private void PassKeyboardHoldEvent(int kbEvent, boolean isUp)
    {
        if(B_ENABLE_LOGGING)
            App.LogLine("[InputSystem] " + (isUp ? "Releasing" : "Holding down") + " key: " + KeyEvent.keyCodeToString(kbEvent));

        if(!isUp)
        {
            Utility.Vibrate(BUTTON_HOLD_VIBRATION_TIME_MS);
        }
        else
        {
            Utility.Vibrate(MOUSE_OR_BUTTON_CLICK_VIBRATION_TIME_MS);
        }

        ActionEvent ev = new ActionEvent(isUp ? ActionEvent.Type.KeyUp : ActionEvent.Type.KeyDown, kbEvent);
        _eventQueue.Enqueue(ev);
    }

    private void PassMouseClick(ActionEvent.MouseClickTypes mouseClick)
    {
        if(B_ENABLE_LOGGING)
            App.LogLine("[InputSystem] Mouse button: " + mouseClick.toString());

        if(mouseClick == ActionEvent.MouseClickTypes.LMBDown
            || mouseClick == ActionEvent.MouseClickTypes.RMBDown)   // Vibrate on hold.
        {
            Utility.Vibrate(MOUSE_DOWN_VIBRATION_TIME_MS);
        }
        else if(mouseClick == ActionEvent.MouseClickTypes.LMBClick)
        {
            Utility.Vibrate(MOUSE_OR_BUTTON_CLICK_VIBRATION_TIME_MS);
        }

        ActionEvent ev = new ActionEvent(mouseClick);

        _eventQueue.Enqueue(ev);
    }

    private void PassMovement(int dx, int dy)
    {
        if(B_ENABLE_LOGGING)
            App.LogLine("[InputSystem] Movement: " + Integer.toString(dx) + ", "  + Integer.toString(dy));

        ActionEvent ev = new ActionEvent(new Movement(dx, dy));

        _eventQueue.Enqueue(ev);
    }

    private void PassScroll(int dy)
    {
        if(B_ENABLE_LOGGING)
            App.LogLine("[InputSystem] Scroll: " + Integer.toString(dy));

        ActionEvent ev = new ActionEvent(new Movement(dy));

        _eventQueue.Enqueue(ev);
    }

    private void PassOSK(final String oskText)
    {
        if(B_ENABLE_LOGGING)
            App.LogLine("Sending OSK event with text: " + oskText);

        ActionEvent ev = new ActionEvent(oskText);

        _eventQueue.Enqueue(ev);
    }


    private static final long BUTTON_HOLD_VIBRATION_TIME_MS = 60;
    private static final long MOUSE_OR_BUTTON_CLICK_VIBRATION_TIME_MS = 30;
    private static final long MOUSE_DOWN_VIBRATION_TIME_MS = 80;
    private static final int BUTTON_HOLD_DPAD_PERIOD_MILLIS = 350;
    private static final int BUTTON_HOLD_DEL_PERIOD_MILLIS = 150;
    private static final float TOUCH_SCALE = 1.3f;
    private static final float SCROLL_SCALE = 0.01f;
    private static final boolean B_ENABLE_LOGGING = false;

    private ButtonHoldHelper _buttonHoldHelper = null;
    private TouchAreaHelper _touchAreaHelper = null;
    private TouchAreaHelper _scrollAreaHelper = null;
    private HashMap<Button, Button> _longClickHelper = null;
    private OSKHelper _oskHelper = null;
    private float _scrollAccumulator = 0.0f;

    private EventQueue<ActionEvent> _eventQueue = null;
}
