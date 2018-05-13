/*
*********************************************************************************************************
*                                             uC/GUI V3.98
*                        Universal graphic software for embedded applications
*
*                       (c) Copyright 2002, Micrium Inc., Weston, FL
*                       (c) Copyright 2002, SEGGER Microcontroller Systeme GmbH
*
*              �C/GUI is protected by international copyright laws. Knowledge of the
*              source code may not be used to write a similar product. This file may
*              only be used in accordance with a license and should not be redistributed
*              in any way. We appreciate your understanding and fairness.
*
----------------------------------------------------------------------
File        : GUI_DrawPolyLine.c
Purpose     : Implementation of GUI_DrawPolyline
---------------------------END-OF-HEADER------------------------------
*/

#include "C2D.h"

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _DrawPolyLine
*
* Draw Poly line, which does not have to be a closed shape
*/
static void _DrawPolyLine(const GUI_POINT*pPoints, int NumPoints, int x0, int y0) {
	unsigned PixelCnt;
	PixelCnt = 0;
	GL_MoveTo(pPoints->x + x0, pPoints->y + y0);
	while (--NumPoints > 0) {
		pPoints++;
		GL_DrawLineToEx(pPoints->x + x0, pPoints->y + y0, &PixelCnt);
	}
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_DrawPolyLine
*/
void GUI_DrawPolyLine(const GUI_POINT* pPoints, int NumPoints, int x0, int y0) {
	GUI_LOCK();
#if (GUI_WINSUPPORT)
	WM_ADDORG(x0, y0);
	WM_ITERATE_START(NULL); {
#endif
		_DrawPolyLine(pPoints, NumPoints, x0, y0);
#if (GUI_WINSUPPORT)
	} WM_ITERATE_END();
#endif
	GUI_UNLOCK();
}

/*************************** End of file ****************************/

