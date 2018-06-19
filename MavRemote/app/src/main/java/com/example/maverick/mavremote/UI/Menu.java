package com.example.maverick.mavremote.UI;

import android.support.constraint.ConstraintLayout;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.TextView;

import com.example.maverick.mavremote.App;
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

        _root = App.GetInstance().GetActivity().findViewById(UIManager.MenuTypeRootResources[menuType.ordinal()]);
        Utility.Assert(_root != null);

        // Extract views.
        ExtractChildrenFromViewGroup(_root, 0);

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

    public Map<Integer, Button> GetButtons()
    {
        return _buttons;
    }

    public Map<Integer, TextView> GetTextViews()
    {
        return _textViews;
    }

    public Map<Integer, EditText> GetEditTextViews()
    {
        return _editTextViews;
    }

    public Map<Integer, ImageView> GetImageViews()
    {
        return _imageViews;
    }

    public Map<Integer, View> GetOtherViews()
    {
        return _otherViews;
    }

    public ConstraintLayout GetRoot()
    {
        return _root;
    }

    public boolean IsEnabled()
    {
        return _bEnabled;
    }


    protected void ExtractChildrenFromViewGroup(ViewGroup viewGroup, int recLevel)
    {
        if(recLevel == EXTRACT_CHILDREN_MAX_RECURSION + 1)
        {
            Log.w(App.TAG, "Reached maximum recursion level in ExtractChildrenFromViewGroup.");
            return;
        }

        for(int i = 0; i < viewGroup.getChildCount(); ++i)
        {
            View child = viewGroup.getChildAt(i);

            if(child instanceof ViewGroup)
            {
                ExtractChildrenFromViewGroup((ViewGroup)child, recLevel + 1);
            }

            // TODO: Make SupportedTypes enum and auto-generate HashMap for each type (in an array).
            if(child instanceof Button)
            {
                _buttons.put(child.getId(), (Button)child);
            }
            else if(child instanceof TextView)
            {
                _textViews.put(child.getId(), (TextView) child);
            }
            else if(child instanceof EditText)
            {
                _editTextViews.put(child.getId(), (EditText) child);
            }
            else if(child instanceof ImageView)
            {
                _imageViews.put(child.getId(), (ImageView)child);
            }
            else
            {
                _otherViews.put(child.getId(), child);
            }
        }
    }


    protected static final int EXTRACT_CHILDREN_MAX_RECURSION = 10;


    protected Map<Integer, Button> _buttons = new HashMap<>();
    protected Map<Integer, TextView> _textViews = new HashMap<>();
    protected Map<Integer, EditText> _editTextViews = new HashMap<>();
    protected Map<Integer, ImageView> _imageViews = new HashMap<>();
    protected Map<Integer, View> _otherViews = new HashMap<>();

    protected ConstraintLayout _root = null;

    protected UIManager.MenuType _myType;
    protected boolean _bEnabled = true;
    protected boolean _bInitialized = false;
}
