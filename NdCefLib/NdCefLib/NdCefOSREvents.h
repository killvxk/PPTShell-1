#pragma  one
#include "include/cef_render_handler.h"
#include "NdCefHandler.h"

namespace NdCef {

class OsrDragEvents {
 public:
  virtual CefBrowserHost::DragOperationsMask OnDragEnter(
      CefRefPtr<CefDragData> drag_data,
      CefMouseEvent ev,
      CefBrowserHost::DragOperationsMask effect) = 0;

  virtual CefBrowserHost::DragOperationsMask OnDragOver(
      CefMouseEvent ev,
      CefBrowserHost::DragOperationsMask effect) = 0;

  virtual void OnDragLeave() = 0;

  virtual CefBrowserHost::DragOperationsMask OnDrop(
      CefMouseEvent ev,
      CefBrowserHost::DragOperationsMask effect) = 0;

 protected:
  virtual ~OsrDragEvents() {}
};

} 

