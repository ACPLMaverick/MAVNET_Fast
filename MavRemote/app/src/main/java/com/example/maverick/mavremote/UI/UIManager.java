package com.example.maverick.mavremote.UI;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.os.Handler;
import android.util.Log;

import com.example.maverick.mavremote.App;
import com.example.maverick.mavremote.Client.AppClient;
import com.example.maverick.mavremote.R;
import com.example.maverick.mavremote.Utility;

import java.util.Calendar;

public class UIManager
{
    public enum MenuType
    {
        ClientNetwork,
        ClientRemote,
        ServerNetwork
    }

    public static final int[] MenuTypeRootResources =
    {
        R.id.incClientNetwork,
        R.id.incClientRemote,
        R.id.clServerNetwork
    };


    public UIManager()
    {
        _handler = new Handler();
    }

    public void InitMenu(MenuType menuType)
    {
        _menus[menuType.ordinal()] = new Menu();
        _menus[menuType.ordinal()].Init(menuType);

        if(!_bHasAnyMenuInitialized)
        {
            SetMenuCurrent(menuType);
            _bHasAnyMenuInitialized = true;
        }
    }

    public boolean IsMenuInitialized(MenuType menuType)
    {
        return _menus[menuType.ordinal()] != null;
    }

    public void SetMenuCurrent(MenuType menuType)
    {
        Utility.Assert(IsMenuInitialized(menuType));

        for(final Menu menu : _menus)
        {
            if(menu != null)
            {
                if(menu.GetMyType() == menuType)
                {
                    _currentMenu = menuType;
                    PerformAction(new Runnable()
                    {
                        @Override
                        public void run()
                        {
                            menu.Enable();
                        }
                    });
                }
                else
                {
                    PerformAction(new Runnable()
                    {
                        @Override
                        public void run()
                        {
                            menu.Disable();
                        }
                    });
                }
            }
        }
    }

    public void PerformAction(Runnable runnable)
    {
        _handler.post(runnable);
    }

    public Menu GetMenu(MenuType menuType)
    {
        return _menus[menuType.ordinal()];
    }

    public Menu GetCurrentMenu()
    {
        Utility.Assert(_bHasAnyMenuInitialized);
        return _menus[_currentMenu.ordinal()];
    }


    private Handler _handler = null;

    private Menu[] _menus = new Menu[MenuType.values().length];
    private MenuType _currentMenu;
    private boolean _bHasAnyMenuInitialized = false;
}
