using System;
 
using System.Collections;
 
using System.Runtime.InteropServices;
 
using System.Diagnostics;
 
using UnityEngine;
 
public class WindowMod: MonoBehaviour
 
{
    static public WindowMod window = null;
    void Awake()
    {
        if (window!=null)
        {
            UnityEngine.Debug.LogError("WindowMod duplicated!!!");
            return;
        }
        window = this;
    }
 
[DllImport("user32.dll")]
static extern IntPtr GetForegroundWindow (); 

	[DllImport("user32.dll", CharSet = CharSet.Auto)]
	public static extern bool SetWindowPos(IntPtr hWnd, IntPtr hPos,int x, int y, int cx, int cy, uint nflags);
	
	[DllImport("User32.dll", EntryPoint = "FindWindow")]   
	private static extern IntPtr FindWindow(string lpClassName,string lpWindowName);   
	
	[DllImport("User32.dll", EntryPoint = "SetWindowLong")]
    private static extern int SetWindowLong(IntPtr hWnd,int nIndex, int dwNewLong);
	
    [DllImport("User32.dll", EntryPoint = "GetWindowLong")]
    private static extern int GetWindowLong(IntPtr hWnd, int dwNewLong);
	
	[DllImport("User32.dll", EntryPoint = "MoveWindow")]
    private static extern bool MoveWindow(IntPtr hWnd, int x,int y, int width,int height,bool repaint);
	
	[DllImport("user32.dll", EntryPoint="ShowWindow", CharSet=CharSet.Auto)]
	public  static extern int  ShowWindow(IntPtr hwnd,int  nCmdShow);
	
	[DllImport("user32.dll", EntryPoint="SendMessage", CharSet=CharSet.Auto)]
	public  static extern int  SendMessage(IntPtr hwnd,int msg,IntPtr wP,IntPtr IP);
	
	[DllImport("user32.dll", EntryPoint="SetParent", CharSet=CharSet.Auto)]
	public  static extern IntPtr SetParent(IntPtr hChild,IntPtr hParent);
	
	[DllImport("user32.dll", EntryPoint="GetParent", CharSet=CharSet.Auto)]
	public  static extern IntPtr GetParent(IntPtr hChild);
	
	[DllImport("User32.dll", EntryPoint = "GetSystemMetrics")]
    public static extern IntPtr GetSystemMetrics(int nIndex);
	
	public enum appStyle {
		FullScreen = 0,
		WindowedFullScreen = 1,
		Windowed = 2,
		WindowedWithoutBorder = 3,
	}
	public appStyle AppWindowStyle = appStyle.WindowedFullScreen;
	
	public enum zDepth {
		Normal = 0,
		Top = 1,
		TopMost = 2,
	}
	public zDepth ScreenDepth = zDepth.Normal;
	
	
	public int windowLeft = 10;
	public int windowTop = 10;
	
	public int windowWidth = 800;
	public int windowHeight = 600;
	
	
	const uint SWP_SHOWWINDOW = 0x0040;
	const int GWL_STYLE = -16;
	const int WS_BORDER = 1; 
	private Rect screenPosition;
	private const int GWL_EXSTYLE = (-20);
	private const int WS_CAPTION = 0xC00000;
	private const int WS_POPUP = 0x800000; 
	IntPtr HWND_TOP = new IntPtr(0);
	IntPtr HWND_TOPMOST = new IntPtr(-1);
	IntPtr HWND_NORMAL = new IntPtr(-2);
	       
	private const int SM_CXSCREEN = 0x00000000;
    private const int SM_CYSCREEN = 0x00000001;
	
	int Xscreen;
    public int ScreenWidth
    {
        get { return Xscreen; }
    }
    int Yscreen;
    public int ScreenHeight
    {
        get { return Yscreen; }
    }

void Start ()
{	 
		
		Xscreen = (int)GetSystemMetrics(SM_CXSCREEN);
        Yscreen = (int)GetSystemMetrics(SM_CYSCREEN);
		
		if((int)AppWindowStyle == 0)
		{
			Screen.SetResolution(Xscreen,Yscreen,true);
		}
		if((int)AppWindowStyle == 1)
		{
			Screen.SetResolution(Xscreen-1,Yscreen-1,false);
			screenPosition = new Rect(0,0,Xscreen-1,Yscreen-1);
		}
		if((int)AppWindowStyle == 2)
		{
			Screen.SetResolution(windowWidth,windowWidth,false);
		}
		if((int)AppWindowStyle == 3)
		{
			Screen.SetResolution(windowWidth,windowWidth,false);
			screenPosition = new Rect(windowLeft,windowTop,windowWidth,windowWidth);
		}
		
}
	
	
	int i = 0;
	void Update () {

        if (UnityEngine.Debug.isDebugBuild)
        {
            return;
        }
		
		if(i == 5)
		{
			if((int)AppWindowStyle == 1)
			{
				SetWindowLong(GetForegroundWindow (), -16, 369164288);
				if((int)ScreenDepth == 0)
					SetWindowPos (GetForegroundWindow (), HWND_NORMAL,(int)screenPosition.x,(int)screenPosition.y, (int)screenPosition.width,(int) screenPosition.height, SWP_SHOWWINDOW);
				if((int)ScreenDepth == 1)
					SetWindowPos (GetForegroundWindow (), HWND_TOP,(int)screenPosition.x,(int)screenPosition.y, (int)screenPosition.width,(int) screenPosition.height, SWP_SHOWWINDOW);
				if((int)ScreenDepth == 2)
					SetWindowPos (GetForegroundWindow (), HWND_TOPMOST,(int)screenPosition.x,(int)screenPosition.y, (int)screenPosition.width,(int) screenPosition.height, SWP_SHOWWINDOW);
				ShowWindow(GetForegroundWindow (),3);
			}
			
			if((int)AppWindowStyle == 2)
			{	
			if((int)ScreenDepth == 0)	
				{
			SetWindowPos(GetForegroundWindow (), HWND_NORMAL, 0, 0, 0, 0, 0x0001 | 0x0002);
			SetWindowPos(GetForegroundWindow (), HWND_NORMAL, 0, 0, 0, 0,0x0001| 0x0002  | 0x0020);
				}
				if((int)ScreenDepth == 1)	
				{
			SetWindowPos(GetForegroundWindow (), HWND_TOP, 0, 0, 0, 0, 0x0001 | 0x0002);
			SetWindowPos(GetForegroundWindow (), HWND_TOP, 0, 0, 0, 0,0x0001| 0x0002  | 0x0020);
				}
				if((int)ScreenDepth == 2)	
				{
			SetWindowPos(GetForegroundWindow (), HWND_TOPMOST, 0, 0, 0, 0, 0x0001 | 0x0002);
			SetWindowPos(GetForegroundWindow (), HWND_TOPMOST, 0, 0, 0, 0,0x0001| 0x0002  | 0x0020);
				}
				
			}
			
			if((int)AppWindowStyle == 3)
			{
				SetWindowLong(GetForegroundWindow (), -16, 369164288);
				if((int)ScreenDepth == 0)
					SetWindowPos (GetForegroundWindow (), HWND_NORMAL,(int)screenPosition.x,(int)screenPosition.y, (int)screenPosition.width,(int) screenPosition.height, SWP_SHOWWINDOW);
				if((int)ScreenDepth == 1)
					SetWindowPos (GetForegroundWindow (), HWND_TOP,(int)screenPosition.x,(int)screenPosition.y, (int)screenPosition.width,(int) screenPosition.height, SWP_SHOWWINDOW);
				if((int)ScreenDepth == 2)
					SetWindowPos (GetForegroundWindow (), HWND_TOPMOST,(int)screenPosition.x,(int)screenPosition.y, (int)screenPosition.width,(int) screenPosition.height, SWP_SHOWWINDOW);
			}
		}
		i++;
		
	}
 
}
 
