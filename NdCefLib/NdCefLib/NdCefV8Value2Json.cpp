#include "StdAfx.h"
#include "NdCefV8Value2Json.h"

namespace NdCef {
	void CNdCefV8Value2Json::V8Value2ObjJsonItem(Json::Value& root,CefString value,CefRefPtr<CefV8Value> object){
		if (object.get()){
			if (object->IsBool()){
				root[value.ToString()] = object->GetBoolValue();
			}else if (object->IsInt()){
				root[value.ToString()] = object->GetIntValue();
			}else if (object->IsUInt()){
				root[value.ToString()] = object->GetUIntValue();
			}else if (object->IsDouble()){
				root[value.ToString()] = object->GetDoubleValue();
			}else if (object->IsDate()){
				root[value.ToString()] = object->GetDateValue().GetTimeT();
			}else if (object->IsString()){
				root[value.ToString()] = object->GetStringValue().ToString();
			}else if (object->IsObject()){
				root[value.ToString()] = V8Value2ObjJson(object);
			}else if (object->IsArray()){
				root[value.ToString()] = V8Value2ArrayJson(object);
			}
		}
	}

	void CNdCefV8Value2Json::V8Value2ArrayJsonItem(Json::Value& root,CefRefPtr<CefV8Value> object){
		if (object.get()){
			if (object->IsBool()){
				root.append(object->GetBoolValue());
			}else if (object->IsInt()){
				root.append(object->GetIntValue());
			}else if (object->IsUInt()){
				root.append(object->GetUIntValue());
			}else if (object->IsDouble()){
				root.append(object->GetDoubleValue());
			}else if (object->IsDate()){
				root.append(object->GetDateValue().GetTimeT());
			}else if (object->IsString()){
				root.append(object->GetStringValue().ToString());
			}else if (object->IsObject()){
				root.append(V8Value2ObjJson(object));
			}else if (object->IsArray()){
				root.append(V8Value2ArrayJson(object));
			}
		}
	}
	Json::Value CNdCefV8Value2Json::V8Value2ArrayJson(CefRefPtr<CefV8Value> object){
		Json::Value root;
		if (object->IsArray()){
			int nLen = object->GetArrayLength();
			for (int i=0;i<nLen;i++)  
			{  
				CefRefPtr<CefV8Value> nitem = object->GetValue(i);
				V8Value2ArrayJsonItem(root,nitem);
			}  
		}
		return root;
	}
	Json::Value CNdCefV8Value2Json::V8Value2ObjJson(CefRefPtr<CefV8Value> object){
		Json::Value root;
		if (object->IsObject()){
			std::vector<CefString> keys;
			object->GetKeys(keys);
			std::vector<CefString>::iterator iter;  
			for (iter=keys.begin();iter!=keys.end();iter++)  
			{  
				CefString value = (*iter);
				CefRefPtr<CefV8Value> nitem = object->GetValue(value);
				V8Value2ObjJsonItem(root,value,nitem);
			}  
		}
		return root;
	}
	std::string CNdCefV8Value2Json::V8Value2ArrayStr(CefRefPtr<CefV8Value> object){
		Json::Value root = V8Value2ArrayJson(object);
		Json::FastWriter writer;  
		std::string strWrite = writer.write(root);
		return strWrite;
	}
	std::string CNdCefV8Value2Json::V8Value2ObjStr(CefRefPtr<CefV8Value> object){
		Json::Value root = V8Value2ObjJson(object);
		Json::FastWriter writer;  
		std::string strWrite = writer.write(root);
		return strWrite;
	}
	void CNdCefV8Value2Json::ObjStr2V8ValueItem(Json::Value root,CefRefPtr<CefV8Value>& nv8root){
		Json::Value::Members member;
		member = root.getMemberNames();
		for (Json::Value::Members::iterator iter = member.begin(); iter != member.end(); iter++)
		{
			Json::Value item = root[(*iter)];
			if (item.isObject()){
				CefRefPtr<CefV8Value> v8objitem =  CefV8Value::CreateObject(NULL);
				ObjStr2V8ValueItem(item,v8objitem);
				nv8root->SetValue((*iter).c_str(),v8objitem,V8_PROPERTY_ATTRIBUTE_READONLY);
			}else if (item.isArray()){
				CefRefPtr<CefV8Value> v8arrayitem =  CefV8Value::CreateArray(item.size());
				ObjStr2V8ValueItem((*iter),v8arrayitem);
				nv8root->SetValue((*iter).c_str(),v8arrayitem,V8_PROPERTY_ATTRIBUTE_READONLY);
			}else {
				nv8root->SetValue((*iter).c_str(),CefV8Value::CreateString(root[(*iter)].asString()),V8_PROPERTY_ATTRIBUTE_READONLY);
			}
		}
	
	}
	void CNdCefV8Value2Json::ObjStr2V8Value(std::string objstr,CefRefPtr<CefV8Value>& nv8root){
		Json::Value root;
		Json::Reader reader;  
		reader.parse(objstr,root);
		ObjStr2V8ValueItem(root,nv8root);
	}
}