#include "StdAfx.h"
#include "Tween.h"


float Linear(float t, float b, float c, float d, int iEaseType /*= 0*/, float s /*= 1.70158*/ )
{
	  return c * t / d + b;
}

float Quadratic(float t, float b, float c, float d, int iEaseType /*= 0*/, float s /*= 1.70158*/ )
{
	if (iEaseType == Interpolator::EASEIN)
	{
		return c*(t/=d)*t + b;
	}

	else if(iEaseType == Interpolator::EASEOUT)
	{
		return -c *(t/=d)*(t-2) + b;
	}

	if ((t/=d/2) < 1) return c/2*t*t + b;
	return -c/2 * ((--t)*(t-2) - 1) + b;
}

float BounceEaseOut(float t, float b, float c, float d, float s /*= 1.70158*/ )
{
	if ((t/=d) < (1/2.75)) {
		return c*(7.5625f*t*t) + b;
	} else if (t < (2/2.75)) {
		return c*(7.5625f*(t-=(1.5f/2.75f))*t + .75f) + b;
	} else if (t < (2.5/2.75)) {
		return c*(7.5625f*(t-=(2.25f/2.75f))*t + .9375f) + b;
	} else {
		return c*(7.5625f*(t-=(2.625f/2.75f))*t + .984375f) + b;
	}
}

float BounceEaseIn(float t, float b, float c, float d, float s /*= 1.70158*/ )
{
	return c - BounceEaseOut(t, b, c, d, s) + b;
}


float Bounce(float t, float b, float c, float d, int iEaseType /*= 0*/, float s /*= 1.70158*/ )
{
	if (iEaseType == Interpolator::EASEIN)
	{
		return BounceEaseIn(t, b, c, d, s);
	}

	else if(iEaseType == Interpolator::EASEOUT)
	{
		return BounceEaseOut(t, b, c, d, s);
	}

	if (t < d/2)
		return BounceEaseIn(t, b, c, d, s) * .5f + b;
	else 
		return BounceEaseOut(t*2-d, 0, c, d, s) * .5f + c*.5f + b;
}



float Interpolator::getInterpolator( int twType, float t, float b, float c, float d, int iEaseType /*= 0*/, float s /*= 1.70158*/ )
{
	switch(twType)
	{
	case TWEEN_Linear:
		{
			return Linear(t, b, c, d, iEaseType, s );
		}
		break;
	case TWEEN_Quadratic:
		{
			return Quadratic(t, b, c, d, iEaseType, s );
		}
		break;
	case TWEEN_Cubic:
		{
		}
		break;
	case TWEEN_Quartic:
		{
		}
		break;
	case TWEEN_Quintic:
		{
		}
		break;
	case TWEEN_Sinusoidal:
		{
		}
		break;
	case TWEEN_Exponential:
		{
		}
		break;
	case TWEEN_Circular:
		{
		}
		break;
	case TWEEN_Back:
		{
		}
		break;
	case TWEEN_Bounce:
		{
			return Bounce(t, b, c, d, iEaseType, s );
		}
		break;
	case TWEEN_Elastic:
		{
		}
		break;
	}

	return 0.0f;
}
