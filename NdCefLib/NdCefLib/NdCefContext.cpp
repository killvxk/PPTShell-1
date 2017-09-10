#include "stdafx.h"
#include "NdCefContext.h"
#include <direct.h>
#include <shlobj.h>

namespace NdCef {

	const char kMultiThreadedMessageLoop[] = "multi-threaded-message-loop";
	const char kCachePath[] = "cache-path";
	const char kUrl[] = "url";
	const char kOffScreenRenderingEnabled[] = "off-screen-rendering-enabled";
	const char kOffScreenFrameRate[] = "off-screen-frame-rate";
	const char kTransparentPaintingEnabled[] = "transparent-painting-enabled";
	const char kShowUpdateRect[] = "show-update-rect";
	const char kMouseCursorChangeDisabled[] = "mouse-cursor-change-disabled";
	
	const char kDefaultUrl[] = "http://www.nd.com.cn";

	CNdCefContext* g_main_context = NULL;

	CNdCefContext* CNdCefContext::Get() {
		return g_main_context;
	}

	CNdCefContext::CNdCefContext(int argc,
		const char* const* argv) {
			command_line_ = CefCommandLine::CreateCommandLine();

			command_line_->InitFromString(::GetCommandLineW());

			if (command_line_->HasSwitch(kUrl))
				main_url_ = command_line_->GetSwitchValue(kUrl);
			if (main_url_.empty())
				main_url_ = kDefaultUrl;

			g_main_context = this;
	}

	CNdCefContext::~CNdCefContext() {
		g_main_context = NULL;
	}

	std::string CNdCefContext::GetConsoleLogPath() {
		
		return GetAppWorkingDirectory() + "console.log";
	}

	std::string CNdCefContext::GetMainURL() {
		return main_url_;
	}

	void CNdCefContext::PopulateSettings(CefSettings* settings) {

		//settings->multi_threaded_message_loop = true;

		CefString(&settings->cache_path) =
			command_line_->GetSwitchValue(kCachePath);

		if (command_line_->HasSwitch(kOffScreenRenderingEnabled))
			settings->windowless_rendering_enabled = true;
	}

	void CNdCefContext::PopulateBrowserSettings(CefBrowserSettings* settings) {
		if (command_line_->HasSwitch(kOffScreenFrameRate)) {
			settings->windowless_frame_rate = atoi(command_line_->
				GetSwitchValue(kOffScreenFrameRate).ToString().c_str());
		}
	}

	std::string CNdCefContext::GetDownloadPath(const std::string& file_name) {
		TCHAR szFolderPath[MAX_PATH];
		std::string path;
		if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PERSONAL | CSIDL_FLAG_CREATE,
			NULL, 0, szFolderPath))) {
				path = CefString(szFolderPath);
				path += "\\" + file_name;
		}

		return path;
	}

	std::string CNdCefContext::GetAppWorkingDirectory() {
		char szWorkingDir[MAX_PATH + 1];
		if (_getcwd(szWorkingDir, MAX_PATH) == NULL) {
			szWorkingDir[0] = 0;
		} else {
			size_t len = strlen(szWorkingDir);
			szWorkingDir[len] = '\\';
			szWorkingDir[len + 1] = 0;
		}
		return szWorkingDir;
	}

}  
