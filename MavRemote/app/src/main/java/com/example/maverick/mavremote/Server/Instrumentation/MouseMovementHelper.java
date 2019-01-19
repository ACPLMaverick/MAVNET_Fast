package com.example.maverick.mavremote.Server.Instrumentation;

import com.example.maverick.mavremote.Actions.Movement;
import com.example.maverick.mavremote.Utility;

import org.jetbrains.annotations.NotNull;

import java.util.LinkedList;
import java.util.concurrent.locks.ReentrantLock;

class MouseMovementHelper
{
	private class MovementEp extends Movement
	{
		public MovementEp(int x, int y)
		{
			super(x, y);
		}

		public MovementEp(final Movement copy)
		{
			super(copy.GetX(), copy.GetY());
		}

		public void Add(int x, int y)
		{
			_x += x;
			_y += y;
		}

		public void Add(final Movement v)
		{
			_x += v.GetX();
			_y += v.GetY();
		}

		public void Sub(int x, int y)
		{
			_x -= x;
			_y -= y;
		}

		public void Sub(final Movement v)
		{
			_x -= v.GetX();
			_y -= v.GetY();
		}

		public boolean IsMovement()
		{
			return _x != 0 || _y != 0;
		}
	}

	public void Start(@NotNull SendEventWrapper sendEventWrapper, @NotNull EventCoder eventCoder)
	{
		_sendEventWrapper = sendEventWrapper;
		_eventCoder = eventCoder;

		/*

		//TEST

		Vec2 vecs[] = new Vec2[6];
		vecs[0] = new Vec2(1.0f, 0.0f);
		vecs[1] = new Vec2(0.0f, 1.0f);
		vecs[2] = new Vec2(-1.0f, 0.0f);
		vecs[3] = new Vec2(0.0f, -1.0f);
		vecs[4] = new Vec2(1.0f, 1.0f);
		vecs[5] = new Vec2(-1.0f, -1.0f);

		for(int i = 0; i < 6; ++i)
		{
			vecs[i] = vecs[i].GetDirRounded();
		}
		*/

		if(_thread != null)
		{
			return;
		}

		_thread = Utility.StartThread(new Runnable()
		{
			@Override
			public void run()
			{
				Start_Internal();
			}
		}, "MouseMovementHelper");
	}

	public void Stop()
	{
		if(_thread != null && _thread.isAlive())
		{
			_bIsRunning = false;
		}
	}

	public void UpdateMouseOffset(final Movement movement)
	{
		_lock.lock();
		_nextMovement = new MovementEp(movement);
		_lock.unlock();
	}

	private void Start_Internal()
	{
		_bIsRunning = true;

		while(_bIsRunning)
		{
			Tick();
		}

		Cleanup();
	}

	private void Tick()
	{
		if(_nextMovement != null)
		{
			_lock.lock();
			_currentMovement.Add(_nextMovement);
			_nextMovement = null;
			_lock.unlock();

//			_currentDeviceEvents.clear();
			_eventCoder.MouseMoveEventToCodes(_currentMovement, _currentDeviceEvents);
		}

		if(!_currentDeviceEvents.isEmpty())
		{
			InputDeviceEvent ev = _currentDeviceEvents.pop();
			_sendEventWrapper.SendInputEvent(SendEventWrapper.DeviceType.Mouse, ev);
			SubFromCurrentMovement(ev);
//			Utility.SleepThreadUs((int)(ev.GetDelay()));
//			Utility.SleepThreadUs(TICK_PROCESS_WAIT_US);
		}
		else
		{
			Utility.SleepThread(TICK_IDLE_WAIT_MS);
		}
	}

	private void Cleanup()
	{
		_eventCoder = null;
		_sendEventWrapper = null;
	}

	private void SubFromCurrentMovement(InputDeviceEvent ev)
	{
		if(ev.GetEvCode() == 0)	// TODO: This is hacky.
		{
			_currentMovement.Sub(ev.GetEvValue(), 0);
		}
		else if(ev.GetEvCode() == 1)
		{
			_currentMovement.Sub(0, ev.GetEvValue());
		}
	}

	private static final int TICK_PROCESS_WAIT_US = 500;
	private static final int TICK_IDLE_WAIT_MS = 1;

	private Thread _thread = null;
	private SendEventWrapper _sendEventWrapper = null;
	private EventCoder _eventCoder = null;

	private MovementEp _currentMovement = new MovementEp(0, 0);
	private MovementEp _nextMovement = null;
	private LinkedList<InputDeviceEvent> _currentDeviceEvents = new LinkedList<>();

	private ReentrantLock _lock = new ReentrantLock();

	private boolean _bIsRunning = false;
}
