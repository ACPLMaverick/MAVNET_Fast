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
        final boolean bIsCurrentNull = _currentReceivedDisplayed == null;
        final boolean bPerformUpdate =  bIsCurrentNull || !_currentReceivedDisplayed.IsTheSame(pck);

        if(bIsCurrentNull)
        {
            _currentReceivedDisplayed = new PacketCounter();
        }

        if(bPerformUpdate)
        {
            _currentReceivedDisplayed.Copy(pck);
            _manager.PerformAction(new Runnable()
            {
                @Override
                public void run()
                {
                    _tvReceived.setText(_currentReceivedDisplayed.toString());
                }
            });
        }
    }

    public void UpdateBroadcasted(final int broadcastedNum)
    {
        if(broadcastedNum != _currentBroadcastedNum)
        {
            _currentBroadcastedNum = broadcastedNum;
            _manager.PerformAction(new Runnable()
            {
                @Override
                public void run()
                {
                    _tvBroadcasted.setText(String.valueOf(_currentBroadcastedNum));
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
        _tvBroadcasted = _manager.GetMenu(UIManager.MenuType.ServerNetwork)
                .GetTextViews().get(R.id.tvSrvBroadcasted);
    }

    @Override
    protected void AcquireLogElement()
    {
        _etLog = _manager.GetMenu(UIManager.MenuType.ServerNetwork)
                .GetEditTextViews().get(R.id.etSrvLog);
    }


    private TextView _tvStatus = null;
    private TextView _tvMyAddress = null;
    private TextView _tvClientAddress = null;
    private TextView _tvReceived = null;
    private TextView _tvBroadcasted = null;

    private NetworkSystem.State _currentStateDisplayed = NetworkSystem.State.Invalid;
    private SocketAddress _currentMyAddressDisplayed = null;
    private SocketAddress _currentClientAddressDisplayed = null;
    private PacketCounter _currentReceivedDisplayed = null;
    private int _currentBroadcastedNum = 0;

    private Runnable _startStopRunnable = null;

    private boolean _bStartStopButtonState = true;  // true is START false is STOP
}
