package com.example.maverick.mavremote.UI;

import android.support.constraint.ConstraintLayout;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.TextView;

import com.example.maverick.mavremote.Client.AppClient;
import com.example.maverick.mavremote.Utility;

import java.util.HashMap;
import java.util.Map;

public class Menu
{
    public Menu() { }

    public void Init(UIManager.MenuType menuType)
    {
        _myType = menuType;

        // TODO: Need base class for App, sadly.
        _root = AppClient.GetInstance().GetActivity().findViewById(UIManager.MenuTypeRootResources[menuType.ordinal()]);
        Utility.Assert(_root != null);

        // TODO: Extract views.

        _bInitialized = true;
    }

    public UIManager.MenuType GetMyType()
    {
        Utility.Assert(_bInitialized);
        return _myType;
    }

    public void Enable()
    {
        if(_bEnabled)
            return;

        _root.setVisibility(View.VISIBLE);
    }

    public void Disable()
    {
        if(!_bEnabled)
            return;

        _root.setVisibility(View.INVISIBLE);
    }


    protected Map<Integer, Button> _buttons = new HashMap<>();
    protected Map<Integer, TextView> _textViews = new HashMap<>();
    protected Map<Integer, EditText> _editTextViews = new HashMap<>();
    protected Map<Integer, ImageView> _imageViews = new HashMap<>();

    protected ConstraintLayout _root = null;

    protected UIManager.MenuType _myType;
    protected boolean _bEnabled = true;
    protected boolean _bInitialized = false;
}
