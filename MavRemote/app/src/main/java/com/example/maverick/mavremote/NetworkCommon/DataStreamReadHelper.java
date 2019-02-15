package com.example.maverick.mavremote.NetworkCommon;

import com.example.maverick.mavremote.App;
import com.example.maverick.mavremote.EventQueue;
import com.example.maverick.mavremote.Utility;

import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.util.Arrays;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;

public class DataStreamReadHelper
{
	public DataStreamReadHelper()
	{
		_syncedQueue.Init();
	}

	public void Init(final InputStream inputStream)
	{
		if (_inputStream != null)
		{
			Stop();
		}

		_inputStream = inputStream;
		_threadRunFlag.set(true);

		Utility.StartThread(new Runnable()
		{
			@Override
			public void run()
			{
				PollingThread();
			}
		}, "DataStreamReadHelperThread");
	}

	public void Stop()
	{
		_threadRunFlag.set(false);
		_inputStream = null;
		_syncedQueue.Clear();
	}


	public ByteBuffer Read()
	{
		if(_syncedQueue.IsEmpty())
			return null;

		return _syncedQueue.Dequeue();
	}

	public int GetFailuresInRow()
	{
		return _readFailuresInRow.get();
	}


	private void PollingThread()
	{
		while(_threadRunFlag.get())
		{
			try
			{
				int readBytes = 0;
				int currentReadBytes = readBytes;

				do
				{
					if(_tempBuf.length - readBytes <= 0)
					{
						EnlargeTempBuf();
					}

					currentReadBytes = _inputStream.read(_tempBuf, readBytes, _tempBuf.length - readBytes);
					if(currentReadBytes < 0)
					{
						_readFailuresInRow.addAndGet(1);
						currentReadBytes = 0;
					}
					else
					{
						_readFailuresInRow.set(0);
					}
					readBytes += currentReadBytes;

//					if(_inputStream.available() <= 0)
					{
						break;
					}

				}while (currentReadBytes > 0);

				if(readBytes > 0)
				{
					byte[] readBuffer = Arrays.copyOf(_tempBuf, readBytes);

					_syncedQueue.Enqueue(ByteBuffer.wrap(readBuffer));
				}
			}
			catch(Exception e)
			{
				App.LogLine("Error reading from InputStream: "	+ e.getMessage());
			}

			Utility.SleepThread(POLLING_THREAD_SLEEP_MS);
		}
	}

	private void EnlargeTempBuf()
	{
		_tempBuf = Arrays.copyOf(_tempBuf, _tempBuf.length * TEMP_BUFFER_RESIZE_MUL + TEMP_BUFFER_RESIZE_ADD);
	}


	private InputStream _inputStream = null;
	private EventQueue<ByteBuffer> _syncedQueue = new EventQueue<>();
	private AtomicBoolean _threadRunFlag = new AtomicBoolean(true);
	private AtomicInteger _readFailuresInRow = new AtomicInteger(0);
	private static final int POLLING_THREAD_SLEEP_MS = 1;

	private static final int TEMP_BUFFER_INITIAL_SIZE = 2048;
	private static final int TEMP_BUFFER_RESIZE_ADD = 0;
	private static final int TEMP_BUFFER_RESIZE_MUL = 2;
	private byte[] _tempBuf = new byte[TEMP_BUFFER_INITIAL_SIZE];
}
