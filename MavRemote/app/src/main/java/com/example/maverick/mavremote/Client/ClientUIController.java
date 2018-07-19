package com.example.maverick.mavremote.Client;

import android.graphics.Color;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.example.maverick.mavremote.App;
import com.example.maverick.mavremote.NetworkCommon.NetworkSystem;
import com.example.maverick.mavremote.R;
import com.example.maverick.mavremote.UI.UIController;
import com.example.maverick.mavremote.UI.UIManager;
import com.example.maverick.mavremote.Utility;

import java.net.InetSocketAddress;
import java.net.SocketAddress;

public class ClientUIController extends UIController
{
    public ClientUIController()
    {
        super();
    }

    public void NetworkUpdateConnectionAddress(final SocketAddress addr)
    {
        final String addrString = ((InetSocketAddress)addr).getAddress().toString();

        if(addrString != _networkAddressDisplayed)
        {
            _manager.PerformAction(new Runnable()
            {
                @Override
                public void run()
                {
                    _tvAddress.setText(addrString);
                }
            });

            _networkAddressDisplayed = addrString;
        }
    }

    public void NetworkUpdateConnectionStatus(final NetworkSystem.State state)
    {
        if(_networkStateDisplayed != state)
        {
            _manager.PerformAction(new Runnable()
            {
                @Override
                public void run()
                {
                    _tvConnectionStatus.setText(STATE_NAMES[state.ordinal()]);
                    _tvConnectionStatus.setTextColor(STATE_COLORS[state.ordinal()]);
                }
            });

            _networkStateDisplayed = state;
        }
    }

    public void RegisterOnConnectionClick(final Runnable runnable)
    {
        if(_btnConnect.hasOnClickListeners())
        {
            Log.w(App.TAG, "[ClientUIController] Connection button already has some handler assigned!");
        }

        _btnConnect.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View view)
            {
                runnable.run();
            }
        });
    }

    public void SetConnectionButtonAvailability(final boolean available)
    {
        _manager.PerformAction(new Runnable()
        {
            @Override
            public void run()
            {
                _btnConnect.setEnabled(available);
            }
        });
    }

    @Override
    protected void AcquireUIElements()
    {
        // ClientNetwork
        _tvAddress = _manager.GetMenu(UIManager.MenuType.ClientNetwork)
                .GetTextViews().get(R.id.tvConnectionAddress);
        _tvConnectionStatus = _manager.GetMenu(UIManager.MenuType.ClientNetwork)
                .GetTextViews().get(R.id.tvStatus);
        _btnConnect = _manager.GetMenu(UIManager.MenuType.ClientNetwork)
                .GetButtons().get(R.id.btnConnect);

        Utility.Assert(_tvAddress != null);
        Utility.Assert(_tvConnectionStatus != null);
        Utility.Assert(_btnConnect != null);
    }

    @Override
    protected void AcquireLogElement()
    {
        _etLog = _manager.GetMenu(UIManager.MenuType.ClientNetwork)
                .GetEditTextViews().get(R.id.etLog);
        Utility.Assert(_etLog != null);
    }

    ///////// ClientNetwork
    private static final String[] STATE_NAMES =
            {
                    "Invalid",
                    "Not connected",
                    "Connecting...",
                    "Connected"
            };
    private static final int[] STATE_COLORS =
            {
                    Color.BLACK,
                    Color.RED,
                    Color.YELLOW,
                    Color.GREEN
            };

    private TextView _tvAddress = null;
    private TextView _tvConnectionStatus = null;
    private Button _btnConnect = null;

    private NetworkSystem.State _networkStateDisplayed = NetworkSystem.State.Invalid;
    private String _networkAddressDisplayed = "0.0.0.0";

    ///////// ClientRemote
}
