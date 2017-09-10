//===========================================================================
// FileName:					Packet.h
//	
// Desc:					Packet defination
//===========================================================================
#ifndef _PACKET_H_
#define _PACKET_H_

typedef struct PACKET_HEAD
{
	DWORD		dwSize;
	DWORD		dwMajorType;
	DWORD		dwMinorType;

} PACKET_HEAD, *PPACKET_HEAD;


//------------------------------------------------------
// Major packet type
//
enum PACKET_TYPE
{
	ACTION_NONE,
	ACTION_USER,
	ACTION_CONTROL,
	ACTION_PUSH_DATA,
	ACTION_PULL_DATA,
	ACTION_SYNC_DATA,
	ACTION_TRANSFER_FILE,

	MOBILE_HEARTBEAT = 2048,		//手机发来的心跳包
};


//------------------------------------------------------
// Minor packet type
// 

//
// User relative operation
//
enum
{
	USER_LOGON						= 1,
	USER_LOGOFF,
	USER_GUEST_LOGON,
	USER_GUEST_LOGOFF,
	USER_MODIFY_NAME,
	USER_GUEST_MODIFY_NAME,
	USER_MODIFY_PERMISSION,
	USER_GUEST_MODIFY_PERMISSION,
	USER_MODIFY_LEADER,
	NDUSER_REQUEST_LOGON_INFO,			//P2M		向手机请求登录信息
	NDUSER_LOGON_INFO,								//M2P		手机向PC同步共享平台用户登录信息
};


//
// Control PPT/3D/Image/WhiteBoard
//
enum 
{
	CONTROL_M2P_PPT					= 1,
	CONTROL_M2P_LASER,			// 激光笔
	CONTROL_M2P_BRUSH,
	CONTROL_M2P_3D,
	CONTROL_M2P_IMAGE,			// 图片快传
	CONTROL_M2P_WHITE_BOARD,	// 白板/黑板
	CONTROL_P2M_MOBILE,
	CONTROL_P2M_RESPONSE,		// PC端给移动端的应答
	CONTROL_M2P_MAGNIFIER,		// 放大镜
	CONTROL_M2P_SPOTLIGHT,		// 聚光灯
};

//
// Transfer file
//
enum 
{
	TRANSFER_FILE_START				= 1,	// 开始传输文件
	TRANSFER_FILE_END,				// 结束传输文件
	TRANSFER_FILE_DATA,				// 传输的文件数据
	TRANSFER_FILE_CANCEL,			// 取消传输文件

	TRANSFER_FILE_CHECK_EXISTS,		// 接收文件CRC检查
	TRANSFER_FILE_CHECK_RESULT,		// 文件CRC检查结果
};

//
// Push informations 
//
enum 
{
	PUSH_P2M_PPTINFO				= 1,
	PUSH_P2M_OUTLINE,
	PUSH_P2M_PCINFO,
	PUSH_P2M_PLAY_BEGIN,
	PUSH_P2M_PLAY_SLIDE,
	PUSH_P2M_PLAY_END,
	PUSH_P2M_USER_STATUS,
	PUSH_P2M_VIDEO_LIST,
	PUSH_P2M_UPDATE_PPTIMAGE,			//增量ppt缩略图更新
	PUSH_P2M_PREVIEW_INDEX,				//预览状态下当前页
	PUSH_P2M_PPTCTL,					// PC端PPT状态变化，通知手机端
	PUSH_P2M_MAGNIFIER,					// 放大镜，通知手机端
	PUSH_P2M_SPOTLIGHT,					// 聚光灯，通知手机端
};

//增量ppt缩略图更新
enum EPPTImageUpdate
{	
	UPDATE_START						= 1,			 //开始增量增加
	UPDATE_PAGECNT,		 //增量更新，总页数
	UPDATE_NEW,				//缩略图更新，消息体：（IDNEW：4个字节新缩略图id）（IDOLD:4个字节旧缩略图id）
	UPDATE_END,				//结束增量更新
};

//
// Pull informations 
//
enum
{
	PULL_M2P_APKFILE				= 1,
	PULL_M2P_PPTFILE,
};

//
// Sync informations		二级消息
//
enum
{
	SYNC_DATA_REQUEST_SLIDE_IMAGE		= 1,			//M2P	手机向PC请求PPT缩略图，消息体：PAGECNT|PAGEIDX……
	SYNC_PPT_HAVE_CHANGE,										//P2M	PPT文档发生变化  
	SYNC_SAVE_CHANGE,												//M2P	保存PPT文档，手机端收到SYNC_PPT_HAVE_CHANGE消息给的应答
	SYNC_NOSAVE_CHANGE,											//M2P 不保存修改，也就是不同步
};


//------------------------------------------------------
// More detail type
//

//
// PPT Control
// 
enum PPTControlType
{
	PPTCTL_NONE = 0,
	PPTCTL_PLAY,
	PPTCTL_STOP,
	PPTCTL_PREV,
	PPTCTL_NEXT,
	PPTCTL_GOTO,
	PPTCTL_CLEAR_INK,
	PPTCTL_BLACK_BACKGROUND,
	PPTCTL_CANCEL_BLACK_BACKGROUND,
	PPTCTL_WHITE_BACKGROUND,
	PPTCTL_CANCEL_WHITE_BACKGROUND,
	PPTCTL_HANDLCLAP,
	PPTCTL_CAIDAI,
	PPTCTL_CONFETTI,
	PPTCTL_CALC,
	PPTCTL_SHOW,
	PPTCTL_LASER_POINTER,
	PPTCTL_PEN_POINTER,
	PPTCTL_NORMAL_POINTER,
	PPTCTL_CANCEL_CALC,
	PPTCTL_PLAY_VIDEO,
	PPTCTL_PAUSE_VIDEO,
	PPTCTL_STOP_VIDEO,
	PPTCTL_FLAME,
	PPTCTL_OPEN,
	PPTCTL_BARRAGE,
	PPTCTL_YAOHAO,	// 摇号
};

//
// Image Control
//
enum ImageControlType
{
	IMGCTL_NONE,
	IMGCTL_OPEN,
	IMGCTL_ZOOMIN,
	IMGCTL_ZOOMOUT,
	IMGCTL_NORMAL,
	IMGCTL_CLOSE,

};

//
// 3D Control
//
enum ThreeDControlType
{
	THREEDCTL_NONE,
	THREEDCTL_ZOOMIN,
	THREEDCTL_ZOOMOUT,
	THREEDCTL_MOVE,
	THREEDCTL_NORMAL,
	THREEDCTL_CLOSE,
};

//BRUSH
enum EBrushControlType
{
	NONE,
	BRUSH_COLOR,
	BRUSH_STYLE,
	BRUSH_WIDTH,
	BRUSH_MOVE,
	BRUSH_UNDO,
	BRUSH_CLEAR,
	BRUSH_BEGIN,
	BRUSH_END,
	BRUSH_SAVE,
	BRUSH_CANCEL_SAVE,
};

//三级命令 放大镜
typedef enum {
	MAGNIFIER_NONE,
	MAGNIFIER_OPEN,			// 打开放大镜
	MAGNIFIER_MOVE,			// 第一个点（－1，－1）最后一个（－2，－2）
	MAGNIFIER_TOGGLE,		// 关闭放大镜
	MAGNIFIER_P2M_RECT,		// PC端给手机端的放大区域，后接左上角、右下角坐标百分比：Left,Top,Right,Bottom
}PPTShellMagnifier;

//三级命令 聚光灯
typedef enum {
	SPOTLIGHT_NONE,
	SPOTLIGHT_OPEN,			//打开聚光灯
	SPOTLIGHT_SWITCH,		//切换聚光灯的形态（1:圆形 2:矩形）
	SPOTLIGHT_CHANGE,		//改变大小 圆传半径（float）,矩形传宽高(float)
	SPOTLIGHT_MOVE,			//聚光灯移动 传中心点
	SPOTLIGHT_OPEN_LIGHT,   //开灯
	SPOTLIGHT_CLOSE_LIGHT,  //关灯
	SPOTLIGHT_TOGGLE		//关闭聚光灯
}PPTShellSpotlight;
//三级命令 黑板
typedef enum{
	BLACKBOARD_OPEN = 1,
	BLACKBOARD_CLOSE,
}BlackBoard;
// 三级命令 PPT状态
enum PPTStateSync
{
	PPTSTATE_P2M_CLOSE = 1,

};

enum ELaserControlType
{
	LASER_CTRL_NONE,
	LASER_MOVE,
	LASER_END,
};

#endif