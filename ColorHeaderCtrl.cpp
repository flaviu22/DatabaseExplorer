// ColorHeaderCtrl.cpp : implementation file
//

#include "pch.h"
#include "ColorHeaderCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CColorHeaderCtrl

IMPLEMENT_DYNAMIC(CColorHeaderCtrl, CHeaderCtrl)

CColorHeaderCtrl::CColorHeaderCtrl()
	: m_bPrint(FALSE)
	, m_bTheme(FALSE)
	, m_bCustomDraw(FALSE)
	, m_crText(GetSysColor(COLOR_WINDOWTEXT))
	, m_crBackground(GetSysColor(COLOR_WINDOW))
{
	m_crBackgroundHot = m_crBackgroundPressed = m_crBackground;
	m_crBackgroundHotTheme = m_crBackgroundPressedTheme = m_crBackgroundTheme = m_crBackground;
}

BEGIN_MESSAGE_MAP(CColorHeaderCtrl, CHeaderCtrl)
	//{{AFX_MSG_MAP(CColorHeaderCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CColorHeaderCtrl::OnNMCustomdraw)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// CColorHeaderCtrl message handlers

void CColorHeaderCtrl::OnNMCustomdraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (!m_bCustomDraw)
		return;

	LPNMLVCUSTOMDRAW pNMCD = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here

	*pResult = CDRF_DODEFAULT;

	switch (pNMCD->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		*pResult = CDRF_NOTIFYITEMDRAW | CDRF_NOTIFYPOSTPAINT;
		break;
	case CDDS_ITEMPREPAINT:
	{
		static HFONT hNewFont = NULL;
		if (!hNewFont)
		{
			LOGFONT lf;
			GetFont()->GetLogFont(&lf);
			lf.lfQuality = NONANTIALIASED_QUALITY;
			hNewFont = ::CreateFontIndirect(&lf);
		}
		SetTextColor(pNMCD->nmcd.hdc, m_crText);
		SelectObject(pNMCD->nmcd.hdc, hNewFont);
		*pResult = CDRF_NEWFONT;
	}
	break;
	case CDDS_POSTPAINT:
		CRect rect;
		GetClientRect(&rect);

		CDC MemDC;
		CBitmap bitmap;
		CDC* pDC = CDC::FromHandle(pNMCD->nmcd.hdc);
		MemDC.CreateCompatibleDC(pDC);
		if (bitmap.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height()))
		{
			CBitmap* pOldBitmap = static_cast<CBitmap*>(MemDC.SelectObject(&bitmap));
			if (!m_bPrint)
			{
				m_bPrint = TRUE;
				::SendMessage(pNMCD->nmcd.hdr.hwndFrom, WM_PRINTCLIENT, (WPARAM)MemDC.GetSafeHdc(), PRF_CLIENT);
				CBrush brush;
				brush.CreateSolidBrush(m_crBackground);
				CBrush* pOldBrush = static_cast<CBrush*>(pDC->SelectObject(&brush));
				pDC->FillRect(&rect, &brush);
				pDC->SelectObject(pOldBrush);
				DeleteObject(&brush);

				if (!m_bTheme)
				{
					const HTHEME hTheme = OpenThemeData(pNMCD->nmcd.hdr.hwndFrom, L"HEADER");
					if (hTheme)
					{
						CDC TestDC;
						TestDC.CreateCompatibleDC(pDC);
						CBitmap bitmapTest;
						bitmapTest.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
						CBitmap* pOldBitmapTest = static_cast<CBitmap*>(TestDC.SelectObject(&bitmapTest));
						DrawThemeBackground(hTheme, TestDC.GetSafeHdc(), HP_HEADERITEM, HIS_NORMAL, &rect, NULL);
						m_crBackgroundTheme = TestDC.GetPixel(1, 1);
						DrawThemeBackground(hTheme, TestDC.GetSafeHdc(), HP_HEADERITEM, HIS_HOT, &rect, NULL);
						m_crBackgroundHotTheme = TestDC.GetPixel(1, 1);
						DrawThemeBackground(hTheme, TestDC.GetSafeHdc(), HP_HEADERITEM, HIS_PRESSED, &rect, NULL);
						m_crBackgroundPressedTheme = TestDC.GetPixel(1, 1);
						TestDC.SelectObject(pOldBitmapTest);
						DeleteObject(bitmapTest);
						DeleteDC(TestDC);
						CloseThemeData(hTheme);

						m_bTheme = TRUE;
					}
				}

				CDC HotDC, PressedDC;
				HotDC.CreateCompatibleDC(pDC);
				PressedDC.CreateCompatibleDC(pDC);
				CBitmap bitmapHot, bitmapPressed;
				bitmapHot.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
				CBitmap* pOldBitmapHot = static_cast<CBitmap*>(HotDC.SelectObject(&bitmapHot));
				bitmapPressed.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
				CBitmap* pOldBitmapPressed = static_cast<CBitmap*>(PressedDC.SelectObject(&bitmapPressed));
				// default color => custom
				pDC->TransparentBlt(0, 0, rect.Width(), rect.Height(), &MemDC, 0, 0, rect.Width(), rect.Height(), m_crBackgroundTheme);
				// Copy pDC to 2 temporary DC
				HotDC.BitBlt(0, 0, rect.Width(), rect.Height(), pDC, 0, 0, SRCCOPY);
				PressedDC.BitBlt(0, 0, rect.Width(), rect.Height(), pDC, 0, 0, SRCCOPY);
				// hot brush in PressedDC
				CBrush brushHot;
				brushHot.CreateSolidBrush(m_crBackgroundHot);
				CBrush* pOldBrushHot = static_cast<CBrush*>(PressedDC.SelectObject(&brushHot));
				PressedDC.FillRect(&rect, &brushHot);
				PressedDC.SelectObject(pOldBrushHot);
				DeleteObject(brushHot);
				// Hot color (Light Blue) => Pressed
				PressedDC.TransparentBlt(0, 0, rect.Width(), rect.Height(), &HotDC, 0, 0, rect.Width(), rect.Height(), m_crBackgroundHotTheme);
				// Pressed brush in pDC
				CBrush brushPressed;
				brushPressed.CreateSolidBrush(m_crBackgroundPressed);
				CBrush* pOldBrushPressed = static_cast<CBrush*>(pDC->SelectObject(&brushPressed));
				pDC->FillRect(&rect, &brushPressed);
				pDC->SelectObject(pOldBrushPressed);
				DeleteObject(brushPressed);
				// Hot => Pressed
				pDC->TransparentBlt(0, 0, rect.Width(), rect.Height(), &PressedDC, 0, 0, rect.Width(), rect.Height(), m_crBackgroundPressedTheme);

				HotDC.SelectObject(pOldBitmapHot);
				DeleteObject(bitmapHot);
				DeleteDC(HotDC);

				PressedDC.SelectObject(pOldBitmapPressed);
				DeleteObject(bitmapPressed);
				DeleteDC(PressedDC);
			}
			else
			{
				m_bPrint = FALSE;
			}
			MemDC.SelectObject(pOldBitmap);
			DeleteObject(bitmap);
		}
		DeleteDC(MemDC);
		break;
	}
}

void CColorHeaderCtrl::UpdateCustomDrawFlag()
{
	if (GetSysColor(COLOR_WINDOW) != m_crBackground ||
		GetSysColor(COLOR_WINDOWTEXT) != m_crText)
		m_bCustomDraw = TRUE;
	else
		m_bCustomDraw = FALSE;
}
