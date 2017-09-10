/*****************************************************************************
	
	winhlpr.h - helper header file to make it compatible with visual studio
	
	-Issam Ali <http://issamsoft.com>

*****************************************************************************/


#ifndef _WINHLPR_
#define _WINHLPR_


#ifdef _MS_VISUAL_STUDIO

#pragma once
#define false   0
#define true    1
#define bool int

#  ifdef MODULE_API_EXPORTS
#    define MODULE_API __declspec(dllexport)
#  else
#    define MODULE_API __declspec(dllimport)
#  endif
#else
#  define MODULE_API
#endif /* _MS_VISUAL_STUDIO */

#endif