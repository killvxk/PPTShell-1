#pragma once

#include <queue>

#include "include/base/cef_lock.h"
#include "include/base/cef_platform_thread.h"
#include "include/base/cef_bind.h"
#include "include/base/cef_scoped_ptr.h"
#include "include/cef_task.h"
#include "NdCefUtilWin.h"

namespace NdCef {

	class CNdCefMessageLoop {
	public:
		CNdCefMessageLoop();
		~CNdCefMessageLoop();

		static CNdCefMessageLoop* Get();

		int Run(HINSTANCE hInstance);
		void Quit();
		void PostTask(CefRefPtr<CefTask> task);
		bool RunsTasksOnCurrentThread() const;
		void SetCurrentModelessDialog(HWND hWndDialog);
		void PostClosure(const base::Closure& closure);
	private:
		static HWND CreateMessageWindow(HINSTANCE hInstance);

		static LRESULT CALLBACK MessageWndProc(HWND hWnd, UINT message, WPARAM wParam,
			LPARAM lParam);

		void PostTaskInternal(CefRefPtr<CefTask> task);

		base::PlatformThreadId thread_id_;
		UINT task_message_id_;

		HWND dialog_hwnd_;

		base::Lock lock_;

		HWND message_hwnd_;
		std::queue<CefRefPtr<CefTask> > queued_tasks_;
	};



	struct DeleteOnMainThread {
		template<typename T>
		static void Destruct(const T* x) {
			if (CURRENTLY_ON_MAIN_THREAD()) {
				delete x;
			} else {
				NdCef::MainMessageLoop::Get()->PostClosure(
					base::Bind(&DeleteOnMainThread::Destruct<T>, x));
			}
		}
	};

}
