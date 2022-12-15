#ifndef _MLOGO_H_
#define _MLOGO_H_

#include <windows.h>

int drawLogo(HDC hDC, long x1, long y1, long x2, long y2);
int drawLogoB(HDC hDC, long x1, long y1, long x2, long y2);
int drawLogoC(HDC hDC, long x1, long y1, long x2, long y2);
int fillGDI(HDC hDC,
		long x1, long y1,
		long x2, long y2,
		int fillColor);
int clearWindow(HDC hDC,
		long x1, long y1,
		long x2, long y2,
		int fillColor);

#endif  /* _MLOGO_ */
