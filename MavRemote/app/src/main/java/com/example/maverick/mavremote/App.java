package com.example.maverick.mavremote;

public final class App
{
    public static App GetInstance()
    {
        if(_instance == null)
            _instance = new App();

        return _instance;
    }

    public void Run()
    {
        Start();
    }

    public InstrumentationSystem GetInstrumentationSystem() { assert(_instr != null); return _instr; }

    public InfraredSystem GetInfraredSystem() { assert(_infr != null); return _infr; }


    private void InternalRun()
    {
        MainLoop();
        Finish();
    }

    private void Start()
    {
        Utility.StartThread(new Runnable()
        {
            @Override
            public void run()
            {
                InternalRun();
            }
        });

        _instr = new InstrumentationSystem();
        Utility.StartThread(new Runnable()
        {
            @Override
            public void run()
            {
                _instr.Run();
            }
        });

        _infr = new InfraredSystem();
        Utility.StartThread(new Runnable()
        {
            @Override
            public void run()
            {
                _infr.Run();
            }
        });

        // Startup instrumentation tests!
        _tester = new TestSystem();
        Utility.StartThread(new Runnable()
        {
            @Override
            public void run()
            {
                _tester.Run();
            }
        });

        _isRunning = true;
    }

    private void Finish()
    {
        _isRunning = false;
    }

    private void MainLoop()
    {
        while(_isRunning)
        {
            //Log.d(App.TAG, "Debug message, hay!");
            Utility.SleepThread(1000);
        }
    }


    public static final String TAG = "MavRemote";

    private static App _instance = null;

    private InstrumentationSystem _instr = null;
    private InfraredSystem _infr = null;
    private TestSystem _tester = null;
    private boolean _isRunning = false;
}
