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

	public int[] GetKeycodes()
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
		if(evType == ActionEvent.Type.Keyboard)
		{
			return TryKeyEventToCodes(ev.GetKeyboardEv(), outCodes);
		}
		else if(evType == ActionEvent.Type.MouseClicks)
		{
			MakeInputDeviceEventsForMouseEvent(ConvertMouseClickEvent(ev), outCodes, 0.0f);
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
		ArrayList<MouseEventConversion> conversions = new ArrayList<>();
		ConvertMouseMovementEvent(movement, conversions);

		final float distance = (float)Math.sqrt(
				movement.GetX() * movement.GetX() +
						movement.GetY() * movement.GetY());

		final float delay = distance == 0.0f ? 0.0f : distance / MOVEMENT_SPEED;

		for(MouseEventConversion conversion : conversions)
		{
			MakeInputDeviceEventsForMouseEvent(conversion, outCodes, delay);
		}
	}

    private boolean TryKeyEventToCodes(int keyEvent, List<InputDeviceEvent> outCodes)
    {
    	Utility.Assert(keyEvent < KEYCODES_NUM);
    	final int keyCode = _keycodes[keyEvent];
    	if(keyCode == BAD_CODE)
		{
			return false;
		}
		else
		{
			MakeInputDeviceEventsForKeycode(keyCode, outCodes);
			return true;
		}
    }

	private void MakeInputDeviceEventsForKeycode(int keyCode, List<InputDeviceEvent> outCodes)
	{
		outCodes.add(new InputDeviceEvent(EV_KEY, keyCode, EV_KEY_VALUE_DOWN));
		outCodes.add(new InputDeviceEvent(EV_KEY, keyCode, EV_KEY_VALUE_UP));
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

	private MouseEventConversion ConvertMouseClickEvent(final ActionEvent ev)
	{
		ActionEvent.MouseClickTypes clickTypes = ev.GetMouseEv();
		MouseEventConversion conv = new MouseEventConversion();

		switch (clickTypes)
		{
			case LMBDown:
				conv.Ev = MouseEvent.BtnLeft;
				conv.Value = 1;
				break;
			case LMBUp:
				conv.Ev = MouseEvent.BtnLeft;
				conv.Value = 0;
				break;
			case RMBDown:
				conv.Ev = MouseEvent.BtnRight;
				conv.Value = 1;
				break;
			case RMBUp:
				conv.Ev = MouseEvent.BtnRight;
				conv.Value = 0;
				break;
			default:
				Utility.Assert(false, "Not supported enum value.");
				break;

		}

		return conv;
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
			if(USE_PER_PIXEL_MOUSE_CODING)
			{
				final int x = movement.GetX();
				final int y = movement.GetY();

				if (x == 0 && y == 0)
				{
					return;
				}

				if (y == 0)
				{
					final int numX = Math.abs(x);
					final int dirX = x / numX;

					for (int i = 0; i < numX; ++i)
					{
						MouseEventConversion conv = new MouseEventConversion();
						conv.Ev = MouseEvent.MovementX;
						conv.Value = dirX;
						outConversions.add(conv);
					}

					return;
				}
				else if (x == 0)
				{
					final int numY = Math.abs(y);
					final int dirY = y / numY;

					for (int i = 0; i < numY; ++i)
					{
						MouseEventConversion conv = new MouseEventConversion();
						conv.Ev = MouseEvent.MovementY;
						conv.Value = dirY;
						outConversions.add(conv);
					}

					return;
				}

				final int numX = Math.abs(x);
				final int numY = Math.abs(y);

				final int dirX = x / numX;
				final int dirY = y / numY;

				final int totalMovement = numX + numY;

				final boolean bSmallerXDim = numX < numY;
				final int smallerDim = bSmallerXDim ? x : y;
				final int largerDim = bSmallerXDim ? y : x;
				final int smallerDimDir = bSmallerXDim ? dirX : dirY;
				final int largerDimDir = bSmallerXDim ? dirY : dirX;
				final int smallerEveryNLargers = (int) Math.round((double) Math.abs(largerDim) / (double) Math.abs(smallerDim)) + 1;

				for (int i = 0; i < totalMovement; ++i)
				{
					MouseEventConversion conv = new MouseEventConversion();

					final boolean bIsSmallerThisStep = i % smallerEveryNLargers == 0;
					if (bIsSmallerThisStep)
					{
						conv.Ev = bSmallerXDim ? MouseEvent.MovementX : MouseEvent.MovementY;
						conv.Value = smallerDimDir;
					}
					else
					{
						conv.Ev = bSmallerXDim ? MouseEvent.MovementY : MouseEvent.MovementX;
						conv.Value = largerDimDir;
					}

					outConversions.add(conv);
				}
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
	}

    private void BuildKeycodeArray()
	{
		for(int i = 0; i < KEYCODES_NUM; ++i)
		{
			_keycodes[i] = BAD_CODE;
		}

		// According to input-event-codes.h from NDK.

		_keycodes[KeyEvent.KEYCODE_ESCAPE] = 1;
		_keycodes[KeyEvent.KEYCODE_DPAD_UP] = 103;	// DPad keys are not working, must use arrows.
		_keycodes[KeyEvent.KEYCODE_DPAD_RIGHT] = 106;
		_keycodes[KeyEvent.KEYCODE_DPAD_DOWN] = 108;
		_keycodes[KeyEvent.KEYCODE_DPAD_LEFT] = 105;
		_keycodes[KeyEvent.KEYCODE_ENTER] = 28;
		_keycodes[KeyEvent.KEYCODE_BACK] = 1;	// 158 is BACK but it is not working.
		_keycodes[KeyEvent.KEYCODE_HOME] = 172;	// Might be 102.
		_keycodes[ActionEvent.GetIntFromSpecialKeyEvent(ActionEvent.SpecialKeyEvent.TaskManager)] = -1; // TODO
		_keycodes[KeyEvent.KEYCODE_MEDIA_PAUSE] = 119;	// 164, 201
		_keycodes[KeyEvent.KEYCODE_MEDIA_PLAY] = 207;	// 200, 0x183
		_keycodes[KeyEvent.KEYCODE_MEDIA_FAST_FORWARD] = 208;
		_keycodes[KeyEvent.KEYCODE_MEDIA_NEXT] = 0x197;
		_keycodes[KeyEvent.KEYCODE_MEDIA_REWIND] = 168; // 0x275
		_keycodes[KeyEvent.KEYCODE_MEDIA_PREVIOUS] = 0x19C;
		_keycodes[KeyEvent.KEYCODE_MUTE] = 113;	// unmute: 0x274
		_keycodes[KeyEvent.KEYCODE_SYSRQ] = 99;
		_keycodes[KeyEvent.KEYCODE_MEDIA_RECORD] = 167; // stoprecord, pauserecord: 0x271, 0x272
		_keycodes[KeyEvent.KEYCODE_DEL] = 14;
		_keycodes[KeyEvent.KEYCODE_VOLUME_UP] = 115;
		_keycodes[KeyEvent.KEYCODE_VOLUME_DOWN] = 114;
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


	private final boolean USE_PER_PIXEL_MOUSE_CODING = true;

    private final float MOVEMENT_SPEED = 1.0f;

	private final int BAD_CODE = -1;


	// EV_SYN is performed automatically by the backend.
    private final int EV_KEY = 1;
	private final int EV_KEY_VALUE_DOWN = 1;
	private final int EV_KEY_VALUE_UP = 0;
    private final int EV_REL = 2;

	private final int KEYCODES_NUM = KeyEvent.getMaxKeyCode() + 2;
	private final int[] _keycodes = new int[KEYCODES_NUM];

	private final int MOUSE_CODES_NUM = MouseEvent.values().length;
	private final int[] _mouseCodes = new int[MOUSE_CODES_NUM];

	private final int[] _mouseCodeTypes = new int[MOUSE_CODES_NUM];
}
