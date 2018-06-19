package com.example.maverick.mavremote.UI;

import com.example.maverick.mavremote.R;
import com.example.maverick.mavremote.Utility;

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


    public UIManager() { }

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

        for(Menu menu : _menus)
        {
            if(menu != null)
            {
                if(menu.GetMyType() == menuType)
                {
                    _currentMenu = menuType;
                    menu.Enable();
                }
                else
                {
                    menu.Disable();
                }
            }
        }
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

    public void DisplayNotification(String notificationText)
    {

    }

    public void DisplayMessageOneResponse(String messageText)
    {

    }

    public boolean DisplayMessageTwoResponses(String messageText)
    {
        return false;
    }


    private Menu[] _menus = new Menu[MenuType.values().length];
    private MenuType _currentMenu;
    private boolean _bHasAnyMenuInitialized = false;
}
