package com.example.maverick.mavremote.Server;

import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.example.maverick.mavremote.App;
import com.example.maverick.mavremote.NetworkCommon.NetworkSystem;
import com.example.maverick.mavremote.R;
import com.example.maverick.mavremote.UI.UIController;
import com.example.maverick.mavremote.UI.UIManager;

import java.net.SocketAddress;

public class ServerUIController extends UIController
{
    public void UpdateCurrentNetworkState(NetworkSystem.State state)
    {
        if(state != _currentStateDisplayed)
        {
            _currentStateDisplayed = state;
            UpdateStatusTextView(_tvStatus, state);
        }
    }

    public void UpdateCurrentMyAddress(final SocketAddress addr)
    {
        if(_currentMyAddressDisplayed != addr)
        {
            _currentMyAddressDisplayed = addr;
            UpdateAddressTextView(_tvMyAddress, addr);
        }
    }

    public void UpdateCurrentClientAddress(final SocketAddress addr)
    {
        if(_currentClientAddressDisplayed != addr)
        {
            _currentClientAddressDisplayed = addr;
            UpdateAddressTextView(_tvClientAddress, addr);
        }
    }

    public void UpdateReceived(final PacketCounter pck)
    {
        if(_currentReceivedDisplayed == null || _currentReceivedDisplayed.IsTheSame(pck))
        {
            _currentReceivedDisplayed = new PacketCounter(pck);
            _manager.PerformAction(new Runnable()
            {
                @Override
                public void run()
                {
                    _tvReceived.setText(pck.toString());
                }
            });
        }
    }

    public void RegisterOnConnectionClick(final Runnable runnable)
    {
        _startStopRunnable = runnable;
    }


    @Override
    protected void AcquireUIElements()
    {
        _tvStatus = _manager.GetMenu(UIManager.MenuType.ServerNetwork)
                .GetTextViews().get(R.id.tvSrvStatus);
        _tvMyAddress = _manager.GetMenu(UIManager.MenuType.ServerNetwork)
                .GetTextViews().get(R.id.tvSrvMyAddress);
        _tvClientAddress = _manager.GetMenu(UIManager.MenuType.ServerNetwork)
                .GetTextViews().get(R.id.tvSrvClientAddress2);
        _tvReceived = _manager.GetMenu(UIManager.MenuType.ServerNetwork)
                .GetTextViews().get(R.id.tvSrvReceived);

        _btnStartStop = _manager.GetMenu(UIManager.MenuType.ServerNetwork)
                .GetButtons().get(R.id.btnSrvStartStop);

        _btnStartStop.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View view)
            {
                OnStartStopClicked();
            }
        });
    }

    @Override
    protected void AcquireLogElement()
    {
        _etLog = _manager.GetMenu(UIManager.MenuType.ServerNetwork)
                .GetEditTextViews().get(R.id.etSrvLog);
    }

    protected void OnStartStopClicked()
    {
        // Process button state change.
        if(_bStartStopButtonState)
        {
            _manager.PerformAction(new Runnable()
            {
                @Override
                public void run()
                {
                    _btnStartStop.setText("STOP");
                }
            });
        }
        else
        {
            _manager.PerformAction(new Runnable()
            {
                @Override
                public void run()
                {
                    _btnStartStop.setText("START");
                }
            });
        }
        _bStartStopButtonState = !_bStartStopButtonState;

        // Fire assigned runnable.
        if(_startStopRunnable != null)
            _startStopRunnable.run();
    }


    private TextView _tvStatus = null;
    private TextView _tvMyAddress = null;
    private TextView _tvClientAddress = null;
    private TextView _tvReceived = null;

    private Button _btnStartStop = null;

    private NetworkSystem.State _currentStateDisplayed = NetworkSystem.State.Invalid;
    private SocketAddress _currentMyAddressDisplayed = null;
    private SocketAddress _currentClientAddressDisplayed = null;
    private PacketCounter _currentReceivedDisplayed = null;

    private Runnable _startStopRunnable = null;

    private boolean _bStartStopButtonState = true;  // true is START false is STOP
}
