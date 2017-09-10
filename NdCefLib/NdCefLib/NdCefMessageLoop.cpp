#include "stdafx.h"
#include "NdCefMessageLoop.h"

#include "include/base/cef_bind.h"
#include "include/cef_app.h"
#include "include/wrapper/cef_closure_task.h"

namespace NdCef {

	CNdCefMessageLoop* g_main_message_loop = NULL;

	const wchar_t kWndClass[] = L"NdCef_MessageWindow";
	const wchar_t kTaskMessageName[] = L"NdCef_CustomTask";

	CNdCefMessageLoop::CNdCefMessageLoop()
		: thread_id_(base::PlatformThread::CurrentId()),
		task_message_id_(RegisterWindowMessage(kTaskMessageName)),
		dialog_hwnd_(NULL),
		message_hwnd_(NULL) {
			CNdDebug::printfStr("CNdCefMessageLoop");
			g_main_message_loop = this;
	}

	CNdCefMessageLoop::~CNdCefMessageLoop() {
		CNdDebug::printfStr("CNdCefMessageLoop::~CNdCefMessageLoop()");
		DCHECK(RunsTasksOnCurrentThread());
		DCHECK(!message_hwnd_);
	    //DCHECK(queued_tasks_.empty());
		g_main_message_loop = NULL;
	}

	CNdCefMessageLoop* CNdCefMessageLoop::Get() {
		DCHECK(g_main_message_loop);
		return g_main_message_loop;
	}

	void CNdCefMessageLoop::PostClosure(const base::Closure& closure) {
		PostTask(CefCreateClosureTask(closure));
	}
	void SetUserDataPtr(HWND hWnd, void* ptr) {
		SetLastError(ERROR_SUCCESS);
		LONG_PTR result = ::SetWindowLongPtr(
			hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(ptr));
		CHECK(result != 0 || GetLastError() == ERROR_SUCCESS);
	}

	int CNdCefMessageLoop::Run(HINSTANCE hInstance) {
		DCHECK(RunsTasksOnCurrentThread());

		{
			base::AutoLock lock_scope(lock_);

			message_hwnd_ = CreateMessageWindow(hInstance);
			CHECK(message_hwnd_);

			SetUserDataPtr(message_hwnd_, this);

			while (!queued_tasks_.empty()) {
				PostTaskInternal(queued_tasks_.front());
				queued_tasks_.pop();
			}
		}

		MSG msg;

		while (GetMessage(&msg, NULL, 0, 0)) {
			if (dialog_hwnd_ && IsDialogMessage(dialog_hwnd_, &msg))
				continue;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		{
			base::AutoLock lock_scope(lock_);

			DestroyWindow(message_hwnd_);
			message_hwnd_ = NULL;
		}

		return static_cast<int>(msg.wParam);
	}

	void CNdCefMessageLoop::Quit() {
		PostClosure(base::Bind(::PostQuitMessage, 0));
	}

	void CNdCefMessageLoop::PostTask(CefRefPtr<CefTask> task) {
		base::AutoLock lock_scope(lock_);
		PostTaskInternal(task);
	}

	bool CNdCefMessageLoop::RunsTasksOnCurrentThread() const {
		return (thread_id_ == base::PlatformThread::CurrentId());
	}

	void CNdCefMessageLoop::SetCurrentModelessDialog(
		HWND hWndDialog) {
			DCHECK(RunsTasksOnCurrentThread());

#ifndef NDEBUG
			if (hWndDialog) {
				DCHECK(!dialog_hwnd_);
			}
#endif
			dialog_hwnd_ = hWndDialog;
	}

	HWND CNdCefMessageLoop::CreateMessageWindow(HINSTANCE hInstance) {
		WNDCLASSEX wc = {0};
		wc.cbSize = sizeof(wc);
		wc.lpfnWndProc = MessageWndProc;
		wc.hInstance = hInstance;
		wc.lpszClassName = kWndClass;
		RegisterClassEx(&wc);

		return CreateWindow(kWndClass, 0, 0, 0, 0, 0, 0, HWND_MESSAGE, 0, hInstance,
			0);
	}

	template <typename T>	T GetUserDataPtr(HWND hWnd) {
		return reinterpret_cast<T>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	}

	LRESULT CALLBACK CNdCefMessageLoop::MessageWndProc(
		HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
			CNdCefMessageLoop* self =
				GetUserDataPtr<CNdCefMessageLoop*>(hWnd);

			if (self && message == self->task_message_id_) {
				CefTask* task = reinterpret_cast<CefTask*>(wParam);
				task->Execute();
				task->Release();
			} else {
				switch (message) 
				{
				case WM_DESTROY:
					SetUserDataPtr(hWnd, NULL);
					break;
				}
			}

			return DefWindowProc(hWnd, message, wParam, lParam);
	}

	void CNdCefMessageLoop::PostTaskInternal(
		CefRefPtr<CefTask> task) {
			lock_.AssertAcquired();

			if (!message_hwnd_) {
				queued_tasks_.push(task);
				return;
			}

			task->AddRef();
			CNdDebug::printfStr("CNdCefMessageLoop::PostTaskInternal");
			PostMessage(message_hwnd_, task_message_id_,
				reinterpret_cast<WPARAM>(task.get()), 0);
	}

} 
