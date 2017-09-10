#pragma once

/**
习题包最大路径长度
*/
#define EXAM_MAX_PATH		1024

/**
学生ID最大长度
*/
#define STUDENT_ID_MAX_LENGTH    65

/**
pptshell UI组件ID定义
*/
#define ICR_UICOMPONET_NAVIGATOR		1                 //PPTShell导航栏UI组件
#define ICR_UICOMPONET_TOOLBAR          2               //PPTShell工具栏UI组件

/**
互动课堂按钮ID定义
*/
#define ICR_BUTTON_ID_ROSTER            1             //花名册按钮ID
#define ICR_BUTTON_ID_PUSHSCREEN        2             //推屏按钮ID
#define ICR_BUTTON_ID_EXAM              3             //随堂练习按钮ID
#define ICR_BUTTON_ID_SUMMARY           4             // 课堂总结按钮ID
#define ICR_BUTTON_ID_HOMEWORK          5             //布置作业按钮ID
#define ICR_BUTTON_ID_INTERACTIVE_TOOL  6             //互动工具按钮ID
#define ICR_BUTTON_ID_VOTE              7            //抢答按钮ID
#define ICR_BUTTON_ID_BAIKE             8           //百科按钮ID

/**
按钮状态
*/
#define ICR_UI_STATE_VISIBLE            1                   //显示
#define ICR_UI_STATE_INVISIBLE          2                   //不显示
#define ICR_UI_STATE_ENABLE             3                   //可用
#define ICR_UI_STATE_DISABLE            4                   //禁用

/**
推屏质量
*/
#define ICR_SCREEN_QUALITY_LOW          1         //低，屏幕压缩1倍
#define ICR_SCREEN_QUALITY_HIGH          2        //高，屏幕未压缩

/**
@name IcrRelease 当Icr需要释放pptshell分配的内存时调用
@param pAddress 内存地址
*/
typedef void(__stdcall *LPFN_IcrReleaseCallbackPtr)(void* pAddress);

/**
@name IcrGetScreen 需要讲课屏幕界面时调用，保证在UI线程中调用
@param nQuality 屏幕质量ICR_SCREEN_QUALITY_*
@param pData 屏幕数据，jpg格式，不再使用时调用IcrRelease释放内存
@param nSize 数据大小
@return 成功或失败
*/
typedef bool(__stdcall *LPFN_IcrGetScreenCallbackPtr)(int nQuality, unsigned char** pData, int& nSize);

/**
@name IcrSendExamEvent 发送随堂练习答题事件，保证在UI线程中调用
@param szEventName 事件名称，\0结尾
@param szEventData 事件数据，\0结尾
@return 成功或失败
*/
typedef bool(__stdcall *LPFN_IcrSendExamEventCallbackPtr)(const char* szEventName, const char* szEventData);

/**
@name IcrGetTopLevelWindowHandle Icr需要显示窗口时可能会调用去获取pptshell顶层窗口句柄作为父窗口句柄
@return pptshell顶层窗口句柄
*/
typedef HWND(__stdcall *LPFN_IcrGetTopLevelWindowHandleCallbackPtr)();

/**
@name IcrMessageBox Icr需要显示MessageBox时调用，功能同windows MessageBox
@param szText 消息内容
@param szCaption 标题
@param uType MB_OK 或 MB_OKCANCEL 或 MB_YESNO
@return IDOK 或 IDCANCEL 或 IDYES 或 IDNO
*/
typedef int(__stdcall *LPFN_IcrMessageBoxCallbackPtr)(const char* szText, const char* szCaption, unsigned int uType);

/**
@name IcrShowToast Icr需要显示toast时调用，toast是顶层居中显示消息，然后自动消息，异步实现不可阻塞调用线程
@param szText 消息内容，实现必须拷贝一份消息内容
@param nDuration 显示时长，单位毫秒
*/
typedef void(__stdcall *LPFN_IcrShowToastCallbackPtr)(const char* szText, int nDuration);

/**
@name IcrEnable Icr检测到有学生端连接认为是互动课堂场景时调用告知pptshell启动互动课堂模式
*/
typedef void(__stdcall *LPFN_IcrEnableCallbackPtr)();

/**
@name IcrGetExamType 点击随堂练习后，判断当前播放的习题类型是否支持时调用
@return 习题类型，不再使用时调IcrRelease回调释放，NULL表示不是习题
*/
typedef char*(__stdcall *LPFN_IcrGetExamTypeCallbackPtr)();

/**
@name IcrSetButtonState 设置PPTShell按钮状态，课堂活动过程中控制按钮的显示隐藏禁用启用状态
@param nUIComponentId   UI组件ID ICR_UICOMPONET_*
@param nButtonId    按钮ID，ICR_BUTTON_ID_*，0表示对整个组件UI设置状态
@param nState 状态ICR_UI_STATE_*
*/
typedef void(__stdcall *LPFN_IcrSetButtonStateCallbackPtr)(int nUIComponentId, int nButtonId, int nState);

/**
@name IcrInit 初始化，程序启动时调用，必须在UI线程中调用
@return 成功或失败
*/
typedef bool(__stdcall *LPFN_IcrInit)();

/**
@name LPFN_IcrUnInit 程序退出时调用（一般在主窗口关闭时调用）
*/
typedef void(__stdcall *LPFN_IcrUnInit)();

/**
@name IcrSetCallback 设置回调，调用IcrOnStart之前必需设置
*/
typedef void(__stdcall *LPFN_IcrSetCallback)(LPFN_IcrReleaseCallbackPtr releasePtr,
    LPFN_IcrGetScreenCallbackPtr getScreenPtr,
    LPFN_IcrSendExamEventCallbackPtr sendExamEventPtr,
    LPFN_IcrGetTopLevelWindowHandleCallbackPtr getTopLevelWindowHandlePtr,
    LPFN_IcrMessageBoxCallbackPtr messageBoxPtr,
    LPFN_IcrShowToastCallbackPtr showToastPtr,
    LPFN_IcrEnableCallbackPtr enableIcrPtr,
    LPFN_IcrGetExamTypeCallbackPtr getExamTypePtr,
    LPFN_IcrSetButtonStateCallbackPtr setButtonStatePtr);

/**
@name IcrOnStart 全屏播放时调用
@param szParamJson start所需参数JSON串格式：
    exam: 所有习题完整路径，习题包main.xml所在的文件夹
    is_recovery: 标识是否恢复，true或false，当程序异常退出后重新启动，教师选择恢复到之前状态时，设置为true
    slide_pos: 幻灯片位置，花名册显示时需要
    {
	"token":{"access_token": "登录时候返回的token", "mac_key":"登录时返回key"},
    "teacher":{"id":"teacher-id", "name" : "nick-name"},
    "class":{"id":"class-id"},
    "chapter":{"subject":"科目ID", "chapter":"章节ID", "teaching_material":"教材ID"},
    "students":"json内容参考 http://wiki.sdp.nd/index.php?title=Admin-API%E6%8E%A5%E5%8F%A3-%E8%8E%B7%E5%8F%96%E7%8F%AD%E7%BA%A7%E5%AD%A6%E7%94%9F%E5%88%97%E8%A1%A8",
    "groups":"json内容参考 http://wiki.sdp.nd/index.php?title=Admin-API%E6%8E%A5%E5%8F%A3-%E8%8E%B7%E5%8F%96%E7%8F%AD%E7%BA%A7%E5%AD%A6%E7%94%9F%E5%88%86%E7%BB%84%E5%88%97%E8%A1%A8",
    "devices":"json内容参考 http://wiki.sdp.nd/index.php?title=Admin-API%E6%8E%A5%E5%8F%A3-%E8%8E%B7%E5%8F%96%E7%8F%AD%E7%BA%A7%E5%8F%8A%E5%AD%A6%E7%94%9F%E8%AE%BE%E5%A4%87",
    "exam" : [{"path" : "exam-path-full-path1"}],
    "is_recovery" : true,
    "slide_pos" : {"left":0, "top":0, "width":100, "height":100}
	"vr": {"enable: true|false"}
    }
@return 成功或失败
*/
typedef bool(__stdcall *LPFN_IcrOnStart)(const char* szParamJson);

/**
@name IcrOnClick 单击互动工具菜单栏上按钮时调用，必须在UI线程中调用
@param nButtonID 按钮ID
*/
typedef void(__stdcall *LPFN_IcrOnClick)(int nButtonID);

/**
@name IcrOnPageIndexChange 翻页事件通知，必须在UI线程中调用
@param nCurrentPageIndex 当前页面索引
@param nNextPageIndex 下一个页面索引
@return true表示允许翻页，false表示不允许翻页
*/
typedef bool(__stdcall *LPFN_IcrOnPageIndexChange)(int nCurrentPageIndex, int nNextPageIndex);

/**
@name IcrOnRecvExamEvent 接收随堂练习答题过程中H5触发的事件，必须在UI线程中调用
@param szEventName 事件名称
@param szEventData 事件数据
*/
typedef void(__stdcall *LPFN_IcrOnRecvExamEvent)(const char* szEventName, const char* szEventData);

/**
@name IcrGetOnlineStudents 获取所有在线学生，随堂练习module需要班级学生的时候调用，必须在UI线程中调用，如果教师登录拥有花名册信息返回花名册，否则返回自组网登录的学生列表
@param ppStudentsJson 学生信息JSON串，不再使用调IcrRelease释放
              JSON格式为：http://wiki.sdp.nd/index.php?title=%E4%BA%92%E5%8A%A8%E8%AF%BE%E5%A0%82/Native%E6%8E%A5%E5%8F%A3#.E8.8E.B7.E5.8F.96.E5.BD.93.E5.89.8D.E7.8F.AD.E7.BA.A7.E7.9A.84.E6.89.80.E6.9C.89.E5.AD.A6.E7.94.9F.E4.BF.A1.E6.81.AF
*/
typedef void(__stdcall *LPFN_IcrGetOnlineStudents)(char** ppStudentsJson);

/**
@name IcrInvokeNativeMethod JS调用native方法，必须在UI线程中调用
@param szMethodJson 方法名称及参数JSON串，utf-8格式，JSON格式如下：
    {
    "method_name":"dohandwrite",
    "param":{"type":"bool", "value":"true|false"},
    "param":{"type":"int", "value":"15"},
    "param":{"type":"double","value":"12.32"}
    "param":{"type":"string", "value":"string-value"}
    }
@return 返回值JSON串，无返回值返回空串，utf-8格式，无需使用调用IcrRelease释放，JSON格式如下：
    {
    "return":{"type":"bool|int|double|string", "value":"true|false|15|12.3|string-value"}
    }
*/
typedef char*(__stdcall *LPFN_IcrInvokeNativeMethod)(const char* szMethodJson);

/**
@name IcrSendCmd 给学生发送命令
@param szCmdJson 命令信息JSON串，utf-8格式，JSON格式如下：
    {
    "category":"命令分类",
    "content": "命令内容JSON串",
    }

    category值为VR时，content JSON串格式如下：
    {    
    "package_name":"com.nd.vr.tianping",
    "activity_name":"TianPingActivity",
    "op":"play|stop"
    }
@return 成功或失败
*/
typedef bool(__stdcall *LPFN_IcrSendCmd)(const char* szCmdJson);

/**
@name IcrRelease 释放内存
*/
typedef void(__stdcall *LPFN_IcrRelease)(void* pAddress);

/**
@name IcrStop 退出全屏播放时调用，必须在UI线程中调用
*/
typedef void(__stdcall *LPFN_IcrStop)();