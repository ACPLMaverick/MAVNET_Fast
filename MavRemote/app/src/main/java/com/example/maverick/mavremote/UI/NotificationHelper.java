package com.example.maverick.mavremote.UI;

import android.app.AlertDialog;
import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Build;
import android.os.Handler;
import android.support.v4.app.NotificationCompat;
import android.support.v4.app.NotificationManagerCompat;
import android.support.v7.app.AppCompatActivity;

import com.example.maverick.mavremote.App;
import com.example.maverick.mavremote.ClientActivity;
import com.example.maverick.mavremote.R;
import com.example.maverick.mavremote.Server.AppServer;
import com.example.maverick.mavremote.ServerActivity;

import java.util.concurrent.locks.ReentrantLock;

public class NotificationHelper
{
    public enum MessageState
    {
        None,
        Waiting,
        Positive,
        Negative
    }


    public NotificationHelper()
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

    public boolean DisplayNotificationText(final String notificationText)
    {
        if(_notificationBuilder == null)
        {
            // Create new notification.

            CreateNotificationChannel();

            Intent intent = new Intent(App.GetInstance().GetContext(),
                    App.GetInstance() instanceof AppServer ? ServerActivity.class : ClientActivity.class);
            intent.setFlags(Intent.FLAG_ACTIVITY_SINGLE_TOP | Intent.FLAG_ACTIVITY_CLEAR_TOP);
            PendingIntent pendingIntent =
                    PendingIntent.getActivity(App.GetInstance().GetContext(), 0, intent, 0);

            _notificationBuilder
                    = new NotificationCompat.Builder(App.GetInstance().GetContext(), NOTIFICATION_CHANNEL_ID)
                    .setSmallIcon(R.drawable.ic_launcher_foreground)
                    .setContentTitle(TITLE)
                    .setContentText(notificationText)
                    .setPriority(NotificationCompat.PRIORITY_HIGH)
                    .setContentIntent(pendingIntent)
                    .setAutoCancel(false);
        }
        else
        {
            // Update existing notification.
            _notificationBuilder.setContentText(notificationText);
        }

        NotificationManagerCompat notificationManager = NotificationManagerCompat.from
        (
                App.GetInstance().GetContext()
        );
        notificationManager.notify(NOTIFICATION_ID, _notificationBuilder.build());

        return true;
    }

    public boolean DisplayMessageOneResponse(final String messageText)
    {
        if(_currentMessageState.ordinal() > MessageState.None.ordinal())
        {
            App.LogLine("[NotificationHelper] Trying to create message when one is in progress!");
            return false;
        }

        if(!App.GetInstance().CanUseUI())
        {
            App.LogLine("[NotificationHelper] Trying to create message without valid Activity!");
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

                alert.setTitle(TITLE);
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
            App.LogLine("[NotificationHelper] Trying to create message when one is in progress!");
            return false;
        }

        if(!App.GetInstance().CanUseUI())
        {
            App.LogLine("[NotificationHelper] Trying to create message without valid Activity!");
            return false;
        }

        _lock.lock();
        _handler.post(new Runnable()
        {
            @Override
            public void run()
            {
                Context ctx = App.GetInstance().GetActivity();
                AlertDialog.Builder builder = new AlertDialog.Builder(ctx);
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

                alert.setTitle(TITLE);

                alert.show();

                _currentMessage = alert;
            }
        });

        _currentMessageState = MessageState.Waiting;
        _lock.unlock();

        return true;
    }

    public void Cleanup()
    {
        NotificationManagerCompat notificationManager = NotificationManagerCompat.from
                (
                        App.GetInstance().GetContext()
                );
        notificationManager.cancel(NOTIFICATION_ID);
    }

    private void CreateNotificationChannel()
    {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O)
        {
            CharSequence name = "MavRemote_ChannelName";
            String description = "MavRemote_ChannelDescription";
            int importance = NotificationManager.IMPORTANCE_HIGH;
            NotificationChannel channel = new NotificationChannel(NOTIFICATION_CHANNEL_ID, name, importance);
            channel.setDescription(description);
            // Register the channel with the system; you can't change the importance
            // or other notification behaviors after this
            NotificationManager notificationManager =
                    App.GetInstance().GetContext().getSystemService(NotificationManager.class);
            notificationManager.createNotificationChannel(channel);
        }

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


    private static final String TITLE = "MavRemote";
    private static final String NOTIFICATION_CHANNEL_ID = TITLE + "_ChannelID";
    private static final int NOTIFICATION_ID = 0xDEADBEEF;

    private Handler _handler;
    private ReentrantLock _lock;

//    private HashMap<AlertDialog, MessageState> _currentDialogs;
    private AlertDialog _currentMessage = null;
    private MessageState _currentMessageState = MessageState.None;

    private NotificationCompat.Builder _notificationBuilder = null;
}
