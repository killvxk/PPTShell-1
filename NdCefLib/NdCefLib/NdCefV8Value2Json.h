#pragma once

#include <vector>

#include "include/cef_v8.h"
#include "include/internal/cef_ptr.h"
#include "json/json.h"

namespace NdCef {
	class CNdCefV8Value2Json
	{
	public:
		static void V8Value2ObjJsonItem(Json::Value& root,CefString value,CefRefPtr<CefV8Value> object);
		static void V8Value2ArrayJsonItem(Json::Value& root,CefRefPtr<CefV8Value> object);
		static Json::Value V8Value2ArrayJson(CefRefPtr<CefV8Value> object);
		static Json::Value V8Value2ObjJson(CefRefPtr<CefV8Value> object);
		static std::string V8Value2ArrayStr(CefRefPtr<CefV8Value> object);
		static std::string V8Value2ObjStr(CefRefPtr<CefV8Value> object);
		static void ObjStr2V8Value(std::string,CefRefPtr<CefV8Value>& nv8root);
		static void ObjStr2V8ValueItem(Json::Value root,CefRefPtr<CefV8Value>& nv8root);
	};
}
