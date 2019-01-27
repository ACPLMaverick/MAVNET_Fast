package com.example.maverick.mavremote.Server.Instrumentation;

import com.example.maverick.mavremote.Actions.Movement;
import com.example.maverick.mavremote.App;
import com.example.maverick.mavremote.EventQueue;
import com.example.maverick.mavremote.Utility;

import org.jetbrains.annotations.NotNull;

import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;

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
		_pendingMovements = new EventQueue<>();
		_pendingMovements.Init();

		// ++test
//		Movement testMovement = new Movement(284, 109);
//		SplitMovements(testMovement, _tmpMovements);
//		_tmpMovements.clear();
		// --test

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

	public void AddMouseMovement(final Movement movement)
	{
		if(_pendingMovements.GetSize() < MAX_PENDING)
		{
			_pendingMovements.Enqueue(movement);
		}
		else
		{
			App.LogLine("Exceeding max pending mouse movements! This shouldn't be happening.");
		}
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
		// Process only one pending movement per tick.
		if(!_pendingMovements.IsEmpty())
		{
			SplitMovements(_pendingMovements.Dequeue(), _tmpMovements);
			for(Movement movement : _tmpMovements)
			{
				_eventCoder.MouseMoveEventToCodes(movement, _currentDeviceEvents);
			}
			_tmpMovements.clear();
		}

		if(!_currentDeviceEvents.isEmpty())
		{
			_sendEventWrapper.SendInputEvent(SendEventWrapper.DeviceType.Mouse, _currentDeviceEvents.pop());
		}

		Utility.SleepThread(TICK_WAIT_MS);
	}

	private void Cleanup()
	{
		_eventCoder = null;
		_sendEventWrapper = null;
	}

	private void SplitMovements(final Movement inMovement, List<Movement> outMovements)
	{
		final int lengthSum = Math.abs(inMovement.GetX()) + Math.abs(inMovement.GetY());
		final int mainDiv = SPLIT_THRESHOLD;

		if(lengthSum <= 2 * mainDiv)
		{
			outMovements.add(inMovement);
			return;
		}

		final int DIM_NUM = 2;
		final boolean xBiggerThanY = Math.abs(inMovement.GetX()) > Math.abs(inMovement.GetY());
		int dims[] =
				{
						xBiggerThanY ? inMovement.GetX() : inMovement.GetY(),
						xBiggerThanY ? inMovement.GetY() : inMovement.GetX()
				};

		int signs[] = new int[DIM_NUM];
		int partsNum;
		int smallerValue;
		int remainders[] = new int[DIM_NUM];
		for(int i = 0; i < 2; ++i)
		{
			signs[i] = GetSign(dims[i]);
		}

		// Bigger goes the classic way.
		partsNum = Math.abs(dims[0]) / mainDiv;
		remainders[0] = (Math.abs(dims[0]) % mainDiv) * signs[0];

		// Adjust smaller values so partsNum could be the same or similar.
		// Don't have to do zero checks here I think.
		smallerValue = Math.abs(dims[1]) / partsNum;
		remainders[1] = (Math.abs(dims[1]) % partsNum) * signs[1];

		// First dim is smaller, so iterate over first partsNum and add smaller sometimes.
		int dbgSumX = 0;
		int dbgSumY = 0;

		for(int i = 0; i < partsNum; ++i)
		{
			final int bigger = mainDiv * signs[0];
			int smaller = smallerValue * signs[1];
			// Include remainder in processed shorter parts.
			if(remainders[1] > 0)
			{
				final int distributionFactor = 1;
				smaller += distributionFactor;
				remainders[1] -= distributionFactor;
			}

			Movement newMovement = xBiggerThanY ? new Movement(bigger, smaller) : new Movement(smaller, bigger);
			outMovements.add(newMovement);

			dbgSumX += newMovement.GetX();
			dbgSumY += newMovement.GetY();
		}

		// Add remainders
		if(remainders[0] != 0 || remainders[1] != 0)
		{
			Movement newMovement = xBiggerThanY ? new Movement(remainders[0], remainders[1]) : new Movement(remainders[1], remainders[0]);
			outMovements.add(newMovement);

			dbgSumX += newMovement.GetX();
			dbgSumY += newMovement.GetY();
		}

		Utility.Assert(dbgSumX == inMovement.GetX() && dbgSumY == inMovement.GetY());
	}

	private int GetSign(int val)
	{
		if(val == 0)
		{
			return 1;
		}
		else
		{
			return val / Math.abs(val);
		}
	}


	private static final int TICK_WAIT_MS = 1;
	private static final int MAX_PENDING = 255;	// This shouldn't be ever exceeded.
	private static final int SPLIT_THRESHOLD = 4;

	private Thread _thread = null;
	private SendEventWrapper _sendEventWrapper = null;
	private EventCoder _eventCoder = null;

	private EventQueue<Movement> _pendingMovements;
	private LinkedList<InputDeviceEvent> _currentDeviceEvents = new LinkedList<>();
	private ArrayList<Movement> _tmpMovements = new ArrayList<>();

	private boolean _bIsRunning = false;
}
