#pragma once
class  Interpolator
{
public:
	typedef enum{
		TWEEN_Linear = 0,
		TWEEN_Quadratic,
		TWEEN_Cubic,
		TWEEN_Quartic,
		TWEEN_Quintic,
		TWEEN_Sinusoidal,
		TWEEN_Exponential,
		TWEEN_Circular,
		TWEEN_Back,
		TWEEN_Bounce,
		TWEEN_Elastic,
	}TWEENTYPE;

	typedef enum{ 
		EASEIN = 0, 
		EASEOUT, 
		EASEINOUT 
	} EaseType;

public:
	Interpolator(void);
	~Interpolator(void);

	//////////////////////////////////////////////////////////////////////////
	// TweenFunc
	// @twType			缓动类型
	// @t						当前帧
	// @b					初始位移
	// @c					移动的总距离
	// @d					总帧数
	// @iEaseType		动画类型( easeIn=0,easeOut=1,easeInOut=2)
	// @s					iEaseType = TWEEN_Back时的参数，默认值为1.70158
	// @a					iEaseType = TWEEN_Elastic时的参数，默认值等于c
	// @p					iEaseType = TWEEN_Elastic时的参数，默认值等于0.3*d
	// return					当前位移
	//////////////////////////////////////////////////////////////////////////
	static float getInterpolator(int twType, float t, float b, float c, float d, int iEaseType = 0, float s = 1.70158);
};
