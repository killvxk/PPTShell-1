#ifndef _UI_SELECT_STEP_BAR_H_
#define _UI_SELECT_STEP_BAR_H_

namespace DuiLib
{
	class CSelectStepBarUI : public CControlUI
	{
	public:
		CSelectStepBarUI();
		~CSelectStepBarUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void PaintStatusImage(HDC hDC);

		// images
		LPCTSTR GetNodeNotReachedImage();
		void SetNodeNotReachedImageImage(LPCTSTR pStrImage);

		LPCTSTR GetNodeReachedImage();
		void SetNodeReachedImageImage(LPCTSTR pStrImage);

		LPCTSTR GetNodePastedImage();
		void SetNodePastedImageImage(LPCTSTR pStrImage);

		LPCTSTR GetLineReachedImage();
		void SetLineReachedImage(LPCTSTR pStrImage);
		
		LPCTSTR GetLineNotReachedImage();
		void SetLineNotReachedImage(LPCTSTR pStrImage);

		// size
		void SetNodeSize(SIZE size);
		void SetLineSize(SIZE size);

		void PrevStep();
		void NextStep();

	protected:
		int				m_nStepCount;
		int				m_nCurStep;
		int				m_nOrientation;
		SIZE			m_LineSize;
		SIZE			m_NodeSize;

		TDrawInfo		m_diNodeNotReached;
		TDrawInfo		m_diNodeReached;
		TDrawInfo		m_diNodePasted;
		TDrawInfo		m_diLineNotReached;
		TDrawInfo		m_diLineReached;

		DWORD			m_dwNotReachedTextColor;
		DWORD			m_dwReachedTextColor;
		DWORD			m_dwPastedTextColor;
		
	};

} // DuiLib

#endif // 
