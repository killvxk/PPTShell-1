#pragma once

#include "include/base/cef_scoped_ptr.h"
#include "include/cef_command_line.h"
#include "include/base/cef_ref_counted.h"
#include "include/internal/cef_types_wrappers.h"

namespace NdCef {

	class CNdCefContext {
	public:
		CNdCefContext(int argc,
			const char* const* argv);

		~CNdCefContext();

		static CNdCefContext* Get();

		static std::string GetConsoleLogPath() ;
		static std::string GetDownloadPath(const std::string& file_name);
		static std::string GetAppWorkingDirectory() ;
	    std::string GetMainURL();
		void PopulateSettings(CefSettings* settings);
		void PopulateBrowserSettings(CefBrowserSettings* settings);

	private:
		CefRefPtr<CefCommandLine> command_line_;
	    std::string main_url_;


	};

}  


