#pragma once

#include <string>


namespace NdCef {

	class NdCefIRegisterJs{
		public:
			virtual void CefEvent(LPCTSTR,LPCTSTR) = 0;
	};
}