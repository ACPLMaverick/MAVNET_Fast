package com.example.maverick.mavremote.Client;

import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;

import com.example.maverick.mavremote.Actions.ActionEvent;
import com.example.maverick.mavremote.App;
import com.example.maverick.mavremote.R;
import com.example.maverick.mavremote.System;
import com.example.maverick.mavremote.UI.Menu;
import com.example.maverick.mavremote.UI.UIManager;

import java.util.HashMap;

public class InputSystem extends System
{
    public static final int[] KEYS_BLACKLISTED_ON_DEVICE =
    {
        KeyEvent.KEYCODE_VOLUME_UP,
        KeyEvent.KEYCODE_VOLUME_DOWN,
        KeyEvent.KEYCODE_BACK
    };


    @Override
    protected void Start()
    {
        _buttonHoldHelper = new ButtonHoldHelper();
        _buttonHoldHelper.Init();

        _longClickHelper = new HashMap<>();


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
        // TODO: Maybe implement some general-purpose recording?
        // TODO: Button state changes in case of MUTE and RECORD.

        // Ideas for other buttons:


        // Set up touch and scroll/swipe field areas and register for touch events.


        _bIsRunning = true;
    }

    @Override
    protected void Finish()
    {

    }

    @Override
    protected void MainLoop()
    {
        while(_bIsRunning)
        {
            // Poll volume regulation from ClientActivity.
            if (!AppClient.GetInstance().GetActivityTyped().GetSystemKeyEvents().IsEmpty())
            {
                KeyEvent ke = AppClient.GetInstance().GetActivityTyped().GetSystemKeyEvents().Dequeue();

                if (
                        ke.getKeyCode() == KeyEvent.KEYCODE_VOLUME_UP
                                || ke.getKeyCode() == KeyEvent.KEYCODE_VOLUME_DOWN
                        )
                {
                    PassKeyboardEvent(ke.getKeyCode());
                }
                else if(ke.getKeyCode() == KeyEvent.KEYCODE_BACK)
                {
                    AppClient.GetInstance().OnBackButtonPressed();
                }
            }

            // Update Hold Helper.
            _buttonHoldHelper.Update();
        }
    }

    private void AssignClickEventToButton(final Button button, final int kbEvent)
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
                                view.performClick();
                            }
                            break;
                            default:
                            break;
                        }
                        return true;
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

    private Menu GetMenu()
    {
        return App.GetInstance().GetUIManager().GetMenu(UIManager.MenuType.ClientRemote);
    }

    private void PassKeyboardEvent(int kbEvent)
    {
        Log.e(App.TAG, "[InputSystem] Pressing key: " + KeyEvent.keyCodeToString(kbEvent));
    }

    private void PassMovement(int dx, int dy)
    {
        Log.e(App.TAG, "[InputSystem] Movement: " + Integer.toString(dx) + ", "  + Integer.toString(dy));
    }

    private void PassScroll(int dy)
    {
        Log.e(App.TAG, "[InputSystem] Scroll: " + Integer.toString(dy));
    }


    private static final int BUTTON_HOLD_PERIOD_MILLIS = 750;

    private ButtonHoldHelper _buttonHoldHelper = null;
    private HashMap<Button, Button> _longClickHelper = null;
    private boolean _bIsRunning = false;
}
