package com.example.maverick.mavremote.Server.Instrumentation;

class Vec2
{
	public Vec2()
	{
		X = 0.0f;
		Y = 0.0f;
	}

	public Vec2(float x, float y)
	{
		X = x;
		Y = y;
	}

	public Vec2(final Vec2 copy)
	{
		X = copy.X;
		Y = copy.Y;
	}

	public float GetX() { return X; }
	public float GetY() { return Y; }

	public void Add(final Vec2 v)
	{
		X += v.X;
		Y += v.Y;
	}

	public float GetLength()
	{
		return (float)Math.sqrt((double)(X * X + Y * Y));
	}

	public void Normalize()
	{
		final float len = GetLength();
		if(len > MIN_LENGTH)
		{
			X /= len;
			Y /= len;
		}
	}

	public Vec2 GetDirRounded()
	{
		Vec2 norm = new Vec2(this);
		norm.Normalize();

		// Angle between this vec and X axis. Should be 0 for (1, 0).
		float angle = (float)Math.atan2(norm.Y, norm.X);

		/*
		\ B /
		 \ /
		C   A
		 / \
	    / D \

	    A -> (1, 0) 	[-0.25*PI, 0.25*PI)
	    B -> (0, 1) 	[0.25*PI, 0.75*PI)
	    C -> (-1, 0) 	[0.75*PI, PI] && [-PI, -0.75*PI)
	    D -> (0, -1) 	[-0.75*PI, -0.25*PI)
		 */

		if(angle >= -PI_4 && angle < PI_4)	// A
		{
			norm.X = 1.0f;
			norm.Y = 0.0f;
		}
		else if(angle >= PI_4 && angle < PI3_4)	//B
		{
			norm.X = 0.0f;
			norm.Y = 1.0f;
		}
		else if(angle >= -PI3_4 && angle < -PI_4) // D
		{
			norm.X = 0.0f;
			norm.Y = -1.0f;
		}
		else // C
		{
			norm.X = -1.0f;
			norm.Y = 0.0f;
		}

		return norm;
	}


	private static final float MIN_LENGTH = 0.00001f;
	private static final float PI = (float)Math.PI;
	private static final float PI_4 = PI * 0.25f;
	private static final float PI3_4 = PI * 0.75f;

	private float X;
	private float Y;
}
