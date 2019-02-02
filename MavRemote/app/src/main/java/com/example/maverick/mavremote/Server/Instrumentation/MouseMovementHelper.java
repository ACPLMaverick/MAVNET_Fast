package com.example.maverick.mavremote.Server.Instrumentation;

import com.example.maverick.mavremote.Actions.Movement;
import com.example.maverick.mavremote.App;
import com.example.maverick.mavremote.EventQueue;
import com.example.maverick.mavremote.Utility;

import org.jetbrains.annotations.NotNull;

import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;
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

		public int GetLengthInt()
		{
			return Math.abs(GetX()) + Math.abs(GetY());
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

	/*
	//		V1
	public void AddMouseMovement(final Movement movement)
	{
		if(_pendingDeviceEvents.size() > MAX_PENDING)	// Don't sync it.
		{
			App.LogLine("Exceeding max pending mouse movements! This shouldn't be happening.");
			return;
		}

		SplitMovements(movement, _tmpMovements);
		_lock.lock();
		for(Movement splitMovement : _tmpMovements)
		{
			_eventCoder.MouseMoveEventToCodes(splitMovement, _pendingDeviceEvents);
		}
		_lock.unlock();
		_tmpMovements.clear();
	}
	*/

	public void AddMouseMovement(final Movement movement)
	{
		if(_pendingDeviceEvents.size() > MAX_PENDING)	// Don't sync it.
		{
			App.LogLine("Exceeding max pending mouse movements! This shouldn't be happening.");
			return;
		}

		_lock.lock();

		_currentMovement.Add(movement);

		// First, clear everything and recompute from beginning.
		_tmpMovements.clear();
		_pendingDeviceEvents.clear();
		// Add first sync just in case.
		_eventCoder.MakeSync(_pendingDeviceEvents);

		Vec2 totalMovement = new Vec2(_currentMovement.GetX(), _currentMovement.GetY());
		MovementEp totalMovementInt = new MovementEp(_currentMovement);
		final float totalLen = totalMovement.GetLength();
		final float splitThres = 1.0f;
		if(totalLen <= splitThres)
		{
			_eventCoder.MouseMoveEventToCodes(_currentMovement, _pendingDeviceEvents);
			_currentTickWaitUs = DEFAULT_TICK_WAIT_US;
		}
		else
		{
			Vec2 baseDir = new Vec2(totalMovement);
			baseDir.Normalize();
			Vec2 accum = new Vec2();

			// Create new integer movements and subtract them from totalMovement until zero.
			while(totalMovementInt.IsMovement())
			{
				Vec2 thisMovement = new Vec2(baseDir);
				if(Math.abs(accum.GetX()) >= 1.0f)
				{
					thisMovement.SetX(thisMovement.GetX() + GetSign(accum.GetX()));
					accum.SetX(accum.GetX() - GetSign(accum.GetX()));
				}
				if(Math.abs(accum.GetY()) >= 1.0f)
				{
					thisMovement.SetY(thisMovement.GetY() + GetSign(accum.GetY()));
					accum.SetY(accum.GetY() - GetSign(accum.GetY()));
				}

				// Truncate floating point values.
				MovementEp intMovement = new MovementEp((int)thisMovement.GetX(), (int)thisMovement.GetY());
				Vec2 leftovers = new Vec2(thisMovement.GetX() - (float)intMovement.GetX(), thisMovement.GetY() - (float)intMovement.GetY());
				accum.Add(leftovers);

				if(intMovement.IsMovement())
				{
					_tmpMovements.add(intMovement);
					totalMovementInt.Sub(intMovement);
				}

				// Correct edge cases
				if(totalMovementInt.IsMovement() && totalMovementInt.GetLengthInt() <= 2)
				{
					_tmpMovements.add(totalMovementInt);
					break;	// Finished.
				}
			}

			// Make new device events.
			for(Movement intMovement : _tmpMovements)
			{
				_eventCoder.MouseMoveEventToCodes(intMovement, _pendingDeviceEvents);
			}

			// Compute new tick wait time in relation to the device event num: t = s / v
			if(_pendingDeviceEvents.isEmpty())
			{
				_currentTickWaitUs = DEFAULT_TICK_WAIT_US;
			}
			else
			{
				_currentTickWaitUs = (int)((float)INPUT_EVENT_NUM_PER_MS / (float)_pendingDeviceEvents.size() * 1000.0f);
			}
		}

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

	/*
	//		V1
	private void Tick()
	{
		// Process only one pending device event per tick.

		InputDeviceEvent event = null;
		_lock.lock();
		if(!_pendingDeviceEvents.isEmpty())
		{
			event = _pendingDeviceEvents.poll();
		}
		_lock.unlock();

		if(event != null)
		{
			_sendEventWrapper.SendInputEvent(SendEventWrapper.DeviceType.Mouse, event);
		}

		Utility.SleepThread(TICK_WAIT_MS);
	}
	*/

	// V2
	private void Tick()
	{
		InputDeviceEvent inputDeviceEvent = null;
		int currentTickWaitUs = DEFAULT_TICK_WAIT_US;
		_lock.lock();
		if(!_pendingDeviceEvents.isEmpty())
		{
			inputDeviceEvent = _pendingDeviceEvents.poll();
			_currentMovement.Sub(_eventCoder.GetMovementFromInputEventCode(inputDeviceEvent));
			currentTickWaitUs = _currentTickWaitUs;
		}
		_lock.unlock();

		if(inputDeviceEvent != null)
		{
			_sendEventWrapper.SendInputEvent(SendEventWrapper.DeviceType.Mouse, inputDeviceEvent);
		}

		if(_executedEventCounter >= INPUT_BUFFERING_WAIT_PER_INPUT_EVENT_NUM)
		{
			_executedEventCounter = 0;
			Utility.SleepThread(INPUT_BUFFERING_WAIT_TIME_MS);
		}
		else
		{
			++_executedEventCounter;
			Utility.SleepThreadUs(currentTickWaitUs);
		}
	}

	private void Cleanup()
	{
		_eventCoder = null;
		_sendEventWrapper = null;
	}

	private void SplitMovements(final Movement inMovement, List<Movement> outMovements)
	{
		//SplitMovementsNoSplit(inMovement, outMovements);
		SplitMovementsApproach1(inMovement, outMovements);
	}

	private void SplitMovementsNoSplit(final Movement inMovement, List<Movement> outMovements)
	{
		outMovements.add(inMovement);
	}

	private void SplitMovementsApproach1(final Movement inMovement, List<Movement> outMovements)
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
		if(val >= 0)
		{
			return 1;
		}
		else
		{
			return -1;
		}
	}

	private float GetSign(float val)
	{
		final float THRES = 0.001f;
		if(val < -THRES)
		{
			return -1.0f;
		}
		else
		{
			return 1.0f;
		}
	}


	private static final int INPUT_EVENT_NUM_PER_MS = 32;
	private static final int DEFAULT_TICK_WAIT_US = 1000;
	private static final int INPUT_BUFFERING_WAIT_PER_INPUT_EVENT_NUM = 128; // TODO: Think of a better way to do this.
	private static final int INPUT_BUFFERING_WAIT_TIME_MS = 1;
	private static final int MAX_PENDING = 255;	// This shouldn't be ever exceeded.
	private static final int SPLIT_THRESHOLD = 4;

	private Thread _thread = null;
	private SendEventWrapper _sendEventWrapper = null;
	private EventCoder _eventCoder = null;

	private LinkedList<InputDeviceEvent> _pendingDeviceEvents = new LinkedList<>();
	private ReentrantLock _lock = new ReentrantLock();
	private ArrayList<Movement> _tmpMovements = new ArrayList<>();	// Code running from MMH thread shouldn't touch this.

	private MovementEp _currentMovement = new MovementEp(0, 0);
	private int _currentTickWaitUs = DEFAULT_TICK_WAIT_US;
	private int _executedEventCounter = 0;

	private boolean _bIsRunning = false;
}
