package com.example.maverick.mavremote.Server.Instrumentation;

import android.view.KeyEvent;

import com.example.maverick.mavremote.Actions.ActionEvent;
import com.example.maverick.mavremote.Actions.Movement;
import com.example.maverick.mavremote.Utility;

import java.util.ArrayList;
import java.util.List;

class EventCoder
{
    enum MouseEvent
    {
        MovementX,
        MovementY,
		Scroll,
		BtnLeft,
		BtnRight
    }

    enum MouseEventType // Must be consistent with Native enum!
	{
		Key,
		Rel
	}

	private class MouseEventConversion
	{
		public MouseEvent Ev;
		public int Value;

		public MouseEventConversion()
		{
			Ev = null;
			Value = 0;
		}

		public MouseEventConversion(MouseEvent ev, int value)
		{
			Ev = ev;
			Value = value;
		}

		@Override
		public String toString()
		{
			return "MouseEventConversion{" +
					"Ev=" + Ev.toString() +
					", Value=" + String.valueOf(Value) +
					'}';
		}
	}

    /*
    Screen coordinates for Android:

    (0, 0)                  (MaxX, 0)



    (0, MaxY)               (MaxX, MaxY)
     */

    public EventCoder()
	{
		BuildKeycodeArray();
		BuildMouseCodeArray();
	}

	public int GetBadCode()
	{
		return BAD_CODE;
	}

	public int[][] GetKeycodes()
	{
		return _keycodes;
	}

	public int[] GetMouseCodes()
	{
		return _mouseCodes;
	}

	public int[] GetMouseCodeTypes()
	{
		return _mouseCodeTypes;
	}

	public boolean ActionEventToCodes(final ActionEvent ev, final ActionEvent.Type evType,
									  List<InputDeviceEvent> outCodes)
	{
		if(ActionEvent.IsKeyboardEvent(evType))
		{
			return TryKeyEventToCodes(ev.GetKeyboardEv(), evType, outCodes);
		}
		else if(evType == ActionEvent.Type.MouseClicks)
		{
			MouseClickEventToCodes(ev.GetMouseEv(), outCodes);
			return true;
		}
		else if(evType == ActionEvent.Type.Movement)
		{
			MouseMoveEventToCodes(ev.GetMovementEv(), outCodes);
			return true;
		}
		else
		{
			return false;
		}
	}

	public void MouseMoveEventToCodes(final Movement movement, List<InputDeviceEvent> outCodes)
	{
		ConvertMouseMovementEvent(movement, _tmpConversions);

		for(MouseEventConversion conversion : _tmpConversions)
		{
			MakeInputDeviceEventsForMouseEvent(conversion, outCodes, 0.0f);
		}

		MakeSync(outCodes);
		_tmpConversions.clear();
	}

	public Movement GetMovementFromInputEventCode(final InputDeviceEvent event)
	{
		if(event.GetEvCode() == _mouseCodes[MouseEvent.MovementX.ordinal()])
		{
			return new Movement(event.GetEvValue(), 0);
		}
		else if(event.GetEvCode() == _mouseCodes[MouseEvent.MovementY.ordinal()])
		{
			return new Movement(0, event.GetEvValue());
		}
		else
		{
			return new Movement(0, 0);
		}
	}

	public void MakeSync(List<InputDeviceEvent> outCodes)
	{
		outCodes.add(new InputDeviceEvent(0, 0, 0));
	}

	public void MakeSync(List<InputDeviceEvent> outCodes, float delay)
	{
		outCodes.add(new InputDeviceEvent(0, 0, 0, delay));
	}

	private void MouseClickEventToCodes(final ActionEvent.MouseClickTypes clickType, List<InputDeviceEvent> outCodes)
	{
		ConvertMouseClickEvent(clickType, _tmpConversions);
		for(MouseEventConversion conversion : _tmpConversions)
		{
			MakeInputDeviceEventsForMouseEvent(conversion, outCodes,0.0f);
			MakeSync(outCodes);
		}
		_tmpConversions.clear();
	}

    private boolean TryKeyEventToCodes(int keyEvent, ActionEvent.Type keyEventType, List<InputDeviceEvent> outCodes)
    {
    	if(keyEvent >= KEYCODES_NUM)
		{
			return false;
		}

    	final int[] keyCodes = _keycodes[keyEvent];
    	final float delay = _keycodeDelays[keyEvent];
    	if(keyCodes == null || keyCodes.length < 1 || keyCodes[0] == BAD_CODE)
		{
			return false;
		}
		else
		{
			boolean bSuccess = true;
			if(keyCodes.length == 1)
			{
				switch (keyEventType)
				{
					case KeyClick:
						MakeInputDeviceEventsForKeycode(keyCodes[0], delay, outCodes);
						break;
					case KeyDown:
						MakeInputDeviceEventsForKeycodeDown(keyCodes[0], outCodes);
						break;
					case KeyUp:
						MakeInputDeviceEventsForKeycodeUp(keyCodes[0], outCodes);
						break;
				}
			}
			else
			{
				Utility.Assert(keyEventType == ActionEvent.Type.KeyClick); // Other types not supported in this case.
				bSuccess = TryMakeInputDeviceEventsForMultipleKeycodes(keyEvent, delay, keyCodes, outCodes);
			}
			return bSuccess;
		}
    }

	private void MakeInputDeviceEventsForKeycode(int keyCode, float delay, List<InputDeviceEvent> outCodes)
	{
		outCodes.add(new InputDeviceEvent(EV_KEY, keyCode, EV_KEY_VALUE_DOWN));
		MakeSync(outCodes, delay);
		outCodes.add(new InputDeviceEvent(EV_KEY, keyCode, EV_KEY_VALUE_UP));
		MakeSync(outCodes);
	}

	private void MakeInputDeviceEventsForKeycodeDown(int keyCode, List<InputDeviceEvent> outCodes)
	{
		outCodes.add(new InputDeviceEvent(EV_KEY, keyCode, EV_KEY_VALUE_DOWN));
		MakeSync(outCodes);
	}

	private void MakeInputDeviceEventsForKeycodeUp(int keyCode, List<InputDeviceEvent> outCodes)
	{
		outCodes.add(new InputDeviceEvent(EV_KEY, keyCode, EV_KEY_VALUE_UP));
		MakeSync(outCodes);
	}

	private boolean TryMakeInputDeviceEventsForMultipleKeycodes(int keyEvent, float delay, int[] keyCodes,
																List<InputDeviceEvent> outCodes)
	{
		boolean bAddedAnyCode = false;
		for(int keyCode : keyCodes)
		{
			if(keyCode == BAD_CODE)
				continue;

			bAddedAnyCode = true;
			outCodes.add(new InputDeviceEvent(EV_KEY, keyCode, EV_KEY_VALUE_DOWN));
		}

		if(!bAddedAnyCode)
			return false;

		MakeSync(outCodes, delay);

		for(int keyCode : keyCodes)
		{
			if(keyCode == BAD_CODE)
				continue;

			outCodes.add(new InputDeviceEvent(EV_KEY, keyCode, EV_KEY_VALUE_UP));
		}

		MakeSync(outCodes);

		return true;
	}

    private void MakeInputDeviceEventsForMouseEvent(MouseEventConversion conv, List<InputDeviceEvent> outCodes, final float delay)
	{
		final int evCode = _mouseCodes[conv.Ev.ordinal()];
		final MouseEventType evType =  MouseEventType.values()[_mouseCodeTypes[conv.Ev.ordinal()]];
		int evTypeInt = 0;
		switch (evType)
		{
			case Key:
			default:
				evTypeInt = EV_KEY;
				break;
			case Rel:
				evTypeInt = EV_REL;
				break;
		}

		outCodes.add(new InputDeviceEvent(evTypeInt, evCode, conv.Value, delay));
	}

	private void ConvertMouseClickEvent(final ActionEvent.MouseClickTypes clickType, List<MouseEventConversion> outConversions)
	{
		switch (clickType)
		{
			case LMBClick:
				outConversions.add(new MouseEventConversion(MouseEvent.BtnLeft, 1));
				outConversions.add(new MouseEventConversion(MouseEvent.BtnLeft, 0));
				break;
			case LMBDown:
				outConversions.add(new MouseEventConversion(MouseEvent.BtnLeft, 1));
				break;
			case LMBUp:
				outConversions.add(new MouseEventConversion(MouseEvent.BtnLeft, 0));
				break;
			case RMBDown:
				outConversions.add(new MouseEventConversion(MouseEvent.BtnRight, 1));
				break;
			case RMBUp:
				outConversions.add(new MouseEventConversion(MouseEvent.BtnRight, 0));
				break;
			default:
				Utility.Assert(false, "Not supported enum value.");
				break;

		}
	}

	private void ConvertMouseMovementEvent(final Movement movement, List<MouseEventConversion> outConversions)
	{
		if(movement.IsScroll())
		{
			MouseEventConversion scrollConv = new MouseEventConversion();
			scrollConv.Ev = MouseEvent.Scroll;
			// Limiting scroll here.
			scrollConv.Value = movement.GetY() != 0 ? movement.GetY() / Math.abs(movement.GetY()) : 0;

			outConversions.add(scrollConv);
		}
		else
		{
			if(movement.GetY() != 0)
			{
				MouseEventConversion conv = new MouseEventConversion();
				conv.Ev = MouseEvent.MovementY;
				conv.Value = movement.GetY();

				outConversions.add(conv);
			}
			if(movement.GetX() != 0)
			{
				MouseEventConversion conv = new MouseEventConversion();
				conv.Ev = MouseEvent.MovementX;
				conv.Value = movement.GetX();

				outConversions.add(conv);
			}
		}
	}

    private void BuildKeycodeArray()
	{
		for(int i = 0; i < KEYCODES_NUM; ++i)
		{
			_keycodes[i] = null;
			_keycodeDelays[i] = 0.0f;
		}

		// According to input-event-codes.h from NDK.

		_keycodes[KeyEvent.KEYCODE_ESCAPE] = new int[]{ 1 };
		_keycodes[KeyEvent.KEYCODE_DPAD_UP] = new int[]{ 103 };	// DPad keys are not working, must use arrows.
		_keycodes[KeyEvent.KEYCODE_DPAD_RIGHT] = new int[]{ 106 };
		_keycodes[KeyEvent.KEYCODE_DPAD_DOWN] = new int[]{ 108 };
		_keycodes[KeyEvent.KEYCODE_DPAD_LEFT] = new int[]{ 105 };
		_keycodes[KeyEvent.KEYCODE_ENTER] = new int[]{ 28 };
		_keycodes[KeyEvent.KEYCODE_BACK] = new int[]{ 1 };	// 158 is BACK but it is not working.
		_keycodes[KeyEvent.KEYCODE_HOME] = new int[]{ 172 };	// Might be 102.
		_keycodes[KeyEvent.KEYCODE_MEDIA_PLAY] = new int[]{ 0xA4 };	// 200, 0x183
		_keycodes[KeyEvent.KEYCODE_MEDIA_FAST_FORWARD] = new int[]{ 208 };
		_keycodes[KeyEvent.KEYCODE_MEDIA_NEXT] = new int[]{ 0xA3 };
		_keycodes[KeyEvent.KEYCODE_MEDIA_REWIND] = new int[]{ 168 }; // 0x275
		_keycodes[KeyEvent.KEYCODE_MEDIA_PREVIOUS] = new int[]{ 0xA5 };
		_keycodes[KeyEvent.KEYCODE_MUTE] = new int[]{ -1 };	// unmute: 0x274 // Works via inputevent.
		_keycodes[KeyEvent.KEYCODE_SYSRQ] = new int[]{ 0x74, 0x72 }; // Should be combination of POWER + Volume down (0x74 + 0x72)
		_keycodes[KeyEvent.KEYCODE_MEDIA_RECORD] = new int[]{ -1 }; // Forefit this as it is supposedly not supported by android 4.2.2. stoprecord, pauserecord: 0x271, 0x272
		_keycodes[KeyEvent.KEYCODE_DEL] = new int[]{ 14 };
		_keycodes[KeyEvent.KEYCODE_VOLUME_UP] = new int[]{ 115 };
		_keycodes[KeyEvent.KEYCODE_VOLUME_DOWN] = new int[]{ 114 };
		_keycodes[KeyEvent.KEYCODE_POWER] = new int[]{ 0x74 };

		_keycodeDelays[KeyEvent.KEYCODE_SYSRQ] = 1.0f;
	}

	private void BuildMouseCodeArray()
	{
		for(int i = 0; i < MOUSE_CODES_NUM; ++i)
		{
			_mouseCodes[i] = BAD_CODE;
			_mouseCodeTypes[i] = BAD_CODE;
		}

		BuildMouseCode(MouseEvent.MovementX, MouseEventType.Rel, 0x00);
		BuildMouseCode(MouseEvent.MovementY, MouseEventType.Rel, 0x01);
		BuildMouseCode(MouseEvent.Scroll, MouseEventType.Rel, 0x08);
		BuildMouseCode(MouseEvent.BtnLeft, MouseEventType.Key, 0x110);
		BuildMouseCode(MouseEvent.BtnRight, MouseEventType.Key, 0x111);
	}

	private void BuildMouseCode(MouseEvent ev, MouseEventType evType, int value)
	{
		_mouseCodes[ev.ordinal()] = value;
		_mouseCodeTypes[ev.ordinal()] = evType.ordinal();
	}


	private final int BAD_CODE = -1;

	// EV_SYN is performed automatically by the backend.
    private final int EV_KEY = 1;
	private final int EV_KEY_VALUE_DOWN = 1;
	private final int EV_KEY_VALUE_UP = 0;
    private final int EV_REL = 2;

	private final int KEYCODES_NUM = KeyEvent.getMaxKeyCode();
	private final int[][] _keycodes = new int[KEYCODES_NUM][];
	private final float[] _keycodeDelays = new float[KEYCODES_NUM];

	private final int MOUSE_CODES_NUM = MouseEvent.values().length;
	private final int[] _mouseCodes = new int[MOUSE_CODES_NUM];

	private final int[] _mouseCodeTypes = new int[MOUSE_CODES_NUM];

	private ArrayList<MouseEventConversion> _tmpConversions = new ArrayList<>();
}
