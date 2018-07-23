package com.example.maverick.mavremote.UI;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.os.Handler;
import android.util.Log;

import com.example.maverick.mavremote.App;
import com.example.maverick.mavremote.Utility;

import java.util.concurrent.locks.ReentrantLock;

public class NotificationManager
{
    public enum MessageState
    {
        None,
        Waiting,
        Positive,
        Negative
    }


    public NotificationManager()
    {
        _handler = new Handler();
        _lock = new ReentrantLock();
    }

    /*
    public MessageState CheckMessageState(AlertDialog dialog)
    {
        MessageState state = _currentDialogs.get(dialog);
        if(state == null)
        {
            return MessageState.None;
        }
        else if(state.ordinal() > MessageState.Waiting.ordinal())
        {
            _currentDialogs.remove(dialog);
        }

        return state;
    }
    */
    public MessageState CheckMessageStateAndCleanup()
    {
        _lock.lock();
        MessageState state = _currentMessageState;
        _lock.unlock();

        if(state.ordinal() > MessageState.Waiting.ordinal())
        {
            // Cleanup on message end.
            _lock.lock();
            _currentMessageState = MessageState.None;
            _lock.unlock();
        }

        return state;
    }

    public boolean DisplayNotificationText(String notificationText)
    {
        return false;
    }

    public boolean DisplayMessageOneResponse(final String messageText)
    {
        if(_currentMessageState.ordinal() > MessageState.None.ordinal())
        {
            App.LogLine("[NotificationManager] Trying to create message when one is in progress!");
            return false;
        }

        _lock.lock();
        _handler.post(new Runnable()
        {
            @Override
            public void run()
            {
                AlertDialog.Builder builder = new AlertDialog.Builder(App.GetInstance().GetActivity());
                builder.setMessage(messageText)
                        .setCancelable(false)
                        .setNeutralButton("OK", new DialogInterface.OnClickListener()
                        {
                            @Override
                            public void onClick(DialogInterface dialogInterface, int i)
                            {
                                dialogInterface.cancel();
                                OnClickChangeMessageState(MessageState.Positive);
                            }
                        });

                final AlertDialog alert = builder.create();

                _currentMessage = alert;

                alert.setTitle("MavRemote");
                alert.show();
            }
        });

        _currentMessageState = MessageState.Waiting;
        _lock.unlock();

        return true;
    }

    public boolean DisplayMessageTwoResponses(final String messageText)
    {
        if(_currentMessageState.ordinal() > MessageState.None.ordinal())
        {
            App.LogLine("[NotificationManager] Trying to create message when one is in progress!");
            return false;
        }

        _lock.lock();
        _handler.post(new Runnable()
        {
            @Override
            public void run()
            {
                AlertDialog.Builder builder = new AlertDialog.Builder(App.GetInstance().GetActivity());
                builder.setMessage(messageText)
                        .setCancelable(false)
                        .setPositiveButton("Yes", new DialogInterface.OnClickListener()
                        {
                            @Override
                            public void onClick(DialogInterface dialogInterface, int i)
                            {
                                dialogInterface.cancel();
                                OnClickChangeMessageState(MessageState.Positive);
                            }
                        })
                        .setNegativeButton("No", new DialogInterface.OnClickListener()
                        {
                            @Override
                            public void onClick(DialogInterface dialogInterface, int i)
                            {
                                dialogInterface.cancel();
                                OnClickChangeMessageState(MessageState.Negative);
                            }
                        });

                final AlertDialog alert = builder.create();

                alert.setTitle("MavRemote");

                alert.show();

                _currentMessage = alert;
            }
        });

        _currentMessageState = MessageState.Waiting;
        _lock.unlock();

        return true;
    }


    private void OnClickChangeMessageState(MessageState newState)
    {
        _lock.lock();
        if(_currentMessage != null)
        {
            _currentMessageState = newState;
            _currentMessage = null;
        }
        _lock.unlock();
    }


    private Handler _handler;
    private ReentrantLock _lock;

//    private HashMap<AlertDialog, MessageState> _currentDialogs;
    private AlertDialog _currentMessage = null;
    private MessageState _currentMessageState = MessageState.None;
}
