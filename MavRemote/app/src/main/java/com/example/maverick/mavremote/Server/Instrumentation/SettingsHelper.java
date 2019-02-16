package com.example.maverick.mavremote.Server.Instrumentation;

import android.content.ContentResolver;
import android.provider.Settings;

import com.example.maverick.mavremote.App;

public class SettingsHelper
{
	public SettingsHelper()
	{

	}

	public void Initialize()
	{
		try
		{
			_contentResolver = App.GetInstance().GetActivity().getContentResolver();
		}
		catch (NullPointerException e)
		{
			App.LogLine("Failed to obtain Content Resolver: " + e.getMessage());
		}
	}

	public void Cleanup()
	{
		_contentResolver = null;
	}

	public void ToggleStatistics()
	{
		if(_contentResolver == null)
			return;

		// This is disabled as it requires app to be a system app.

		/*
		final String setting = Settings.Global.SHOW_PROCESSES;
		boolean state = GlobalGetBoolean(setting);
		GlobalSetBoolean(setting, !state);
		*/
	}


	private int GlobalGetInt(final String setting)
	{
		int settingValue = 0;
		try
		{
			settingValue = Settings.Global.getInt(_contentResolver, setting);
		}
		catch (Settings.SettingNotFoundException e)
		{
			App.LogLine("Failed to retrieve setting " + setting + ", :" + e.getMessage());
		}
		return settingValue;
	}

	private void GlobalSetInt(final String setting, final int value)
	{
		Settings.Global.putInt(_contentResolver, setting, value);
	}

	private boolean GlobalGetBoolean(final String setting)
	{
		return GlobalGetInt(setting) > 0;
	}

	private void GlobalSetBoolean(final String setting, final boolean value)
	{
		GlobalSetInt(setting, value ? 1 : 0);
	}

	private ContentResolver _contentResolver = null;
}
