
#include <math.h>
#include "mlogo.h"

const double PI=3.141592654;

/***************************************************************/
/* kk 04/02/00
*/
int drawLogo(HDC hDC, long x1, long y1, long x2, long y2)
{
	POINT grpOne[5];
	POINT grpTwo[4];
	long lTrash;
	long lRet;
	HGDIOBJ hBrush;
	HRGN hRgn;
	long radius;
	POINT origin;

	if (x2 <= x1)
		return -1;
	if (y2 <= y1)
		return -2;
	if ( (x2 - x1) > (y2 - y1) )
		radius = (y2 - y1) / 2;
	else
		radius = (x2 - x1) / 2;

	origin.x = x1 + ((x2 - x1) / 2);
	origin.y = y1 + ((y2 - y1) / 2);

	grpOne[0].x = origin.x;
	grpOne[0].y = origin.y - radius;
	grpOne[1].x = origin.x + radius * cos((18.0 / 180) * PI);
	grpOne[1].y = origin.y - radius * sin((18.0 / 180) * PI);
	grpOne[2].x = origin.x + radius * cos((54.0 / 180) * PI);
	grpOne[2].y = origin.y + radius * sin((54.0 / 180) * PI);
	grpOne[3].x = origin.x - radius * cos((54.0 / 180) * PI);
	grpOne[3].y = origin.y + radius * sin((54.0 / 180) * PI);
	grpOne[4].x = origin.x - radius * cos((18.0 / 180) * PI);
	grpOne[4].y = origin.y - radius * sin((18.0 / 180) * PI);

	grpTwo[0].x = origin.x;
	grpTwo[0].y = origin.y - radius;
	grpTwo[1].x = origin.x + radius * cos((18.0 / 180) * PI);
	grpTwo[1].y = origin.y - radius * sin((18.0 / 180) * PI);
	grpTwo[2].x = origin.x + radius * cos((54.0 / 180) * PI);
	grpTwo[2].y = origin.y + radius * sin((54.0 / 180) * PI);
	grpTwo[3].x = origin.x;
	grpTwo[3].y = origin.y;


	// fill with pentagon
	hBrush = GetStockObject(WHITE_BRUSH);
	if (hBrush == NULL)
		return -3;
	hRgn = CreatePolygonRgn(grpOne, 5, WINDING);
	if (hRgn != NULL)
	{
		lRet = FillRgn(hDC, hRgn, hBrush);
		lTrash = DeleteObject(hRgn);
	}

	// draw border
	Polygon(hDC, grpOne, 5);

	// fill with sector
	hBrush = GetStockObject(BLACK_BRUSH);
	if (hBrush == NULL)
		return -4;
	hRgn = CreatePolygonRgn(grpTwo, 4, WINDING);
	if (hRgn != NULL)
	{
		lRet = FillRgn(hDC, hRgn, hBrush);
		lTrash = DeleteObject(hRgn);
	}

	TextOut(hDC, grpOne[3].x+3, grpOne[3].y+3, "WulphSOFT", 9);

 	return 0;
}

/***************************************************************/
/* kk 03/05/00
*/
int drawLogoB(HDC hDC, long x1, long y1, long x2, long y2)
{
	POINT grpOne[5];
	POINT grpTwo[4];
	long lTrash;
	long lRet;
	HGDIOBJ hBrush;
	HRGN hRgn;
	long radius;
	POINT origin;
	double angleA;  /* in radian */
	double angleB;
	double angleC;
	double lengthA;
	double lengthB;
	double lengthC;


	if (x2 <= x1)
		return -1;
	if (y2 <= y1)
		return -2;
	if ( (x2 - x1) > (y2 - y1) )
		radius = (y2 - y1) / 2;
	else
		radius = (x2 - x1) / 2;

	origin.x = x1 + ((x2 - x1) / 2);
	origin.y = y1 + ((y2 - y1) / 2);

	grpOne[0].x = origin.x;
	grpOne[0].y = origin.y - radius;
	grpOne[1].x = origin.x + radius * cos((18.0 / 180) * PI);
	grpOne[1].y = origin.y - radius * sin((18.0 / 180) * PI);
	grpOne[2].x = origin.x + radius * cos((54.0 / 180) * PI);
	grpOne[2].y = origin.y + radius * sin((54.0 / 180) * PI);
	grpOne[3].x = origin.x - radius * cos((54.0 / 180) * PI);
	grpOne[3].y = origin.y + radius * sin((54.0 / 180) * PI);
	grpOne[4].x = origin.x - radius * cos((18.0 / 180) * PI);
	grpOne[4].y = origin.y - radius * sin((18.0 / 180) * PI);

	// fill with pentagon
	hBrush = GetStockObject(WHITE_BRUSH);
	if (hBrush == NULL)
		return -3;
	hRgn = CreatePolygonRgn(grpOne, 5, WINDING);
	if (hRgn != NULL)
	{
		lRet = FillRgn(hDC, hRgn, hBrush);
		lTrash = DeleteObject(hRgn);
	}

	// draw border
	Polygon(hDC, grpOne, 5);

	// calculation
	lengthA = (5.0 / 4) * sin(72.0 / 180 * PI) / (sin(54.0 / 180 * PI) * (1 + sin(54.0 / 180 * PI)));
	lengthB = (1 + sin(54.0 / 180 * PI)) / lengthA;
	angleA = atan((sin(126.0 / 180 * PI)) / (lengthB + cos(126.0 / 180 * PI)));

	lengthC = sqrt((lengthA * lengthA) + 1 - (2 * lengthA * cos(54.0 / 180 * PI)));
	angleB = asin(lengthA * sin(54.0 / 180 * PI) / lengthC);

	angleC = (PI / 2.0) - angleB;

	// right side
	grpTwo[0].x = origin.x + radius * cos((54.0 / 180) * PI);
	grpTwo[0].y = origin.y + radius * sin((54.0 / 180) * PI);
	grpTwo[1].x = origin.x + radius * cos((18.0 / 180) * PI);
	grpTwo[1].y = origin.y - radius * sin((18.0 / 180) * PI);
	grpTwo[2].x = origin.x + lengthC * radius * cos(angleC);
	grpTwo[2].y = origin.y - lengthC * radius * sin(angleC);
	grpTwo[3].x = origin.x;
	grpTwo[3].y = origin.y + radius * sin((54.0 / 180) * PI);;

	hBrush = GetStockObject(BLACK_BRUSH);
	if (hBrush == NULL)
		return -4;
	hRgn = CreatePolygonRgn(grpTwo, 4, WINDING);
	if (hRgn != NULL)
	{
		lRet = FillRgn(hDC, hRgn, hBrush);
		lTrash = DeleteObject(hRgn);
	}

	// left side
	grpTwo[0].x = origin.x - radius * cos((54.0 / 180) * PI);
	grpTwo[0].y = origin.y + radius * sin((54.0 / 180) * PI);
	grpTwo[1].x = origin.x - radius * cos((18.0 / 180) * PI);
	grpTwo[1].y = origin.y - radius * sin((18.0 / 180) * PI);
	grpTwo[2].x = origin.x - lengthC * radius * cos(angleC);
	grpTwo[2].y = origin.y - lengthC * radius * sin(angleC);
	grpTwo[3].x = origin.x;
	grpTwo[3].y = origin.y + radius * sin((54.0 / 180) * PI);;

	hBrush = GetStockObject(BLACK_BRUSH);
	if (hBrush == NULL)
		return -4;
	hRgn = CreatePolygonRgn(grpTwo, 4, WINDING);
	if (hRgn != NULL)
	{
		lRet = FillRgn(hDC, hRgn, hBrush);
		lTrash = DeleteObject(hRgn);
	}

	TextOut(hDC, grpOne[3].x+3, grpOne[3].y+3, "WulphSOFT", 9);

	return 0;
}

/***************************************************************/
/* kk 12/08/01
*/
int drawLogoC(HDC hDC, long x1, long y1, long x2, long y2)
{
	POINT grpOne[5];
	POINT grpTwo[4];
	long lTrash;
	long lRet;
	HGDIOBJ hBrush;
	HRGN hRgn;
	long radius;
	POINT origin;


	if (x2 <= x1)
		return -1;
	if (y2 <= y1)
		return -2;
	if ( (x2 - x1) > (y2 - y1) )
		radius = (y2 - y1) / 2;
	else
		radius = (x2 - x1) / 2;

	origin.x = x1 + ((x2 - x1) / 2);
	origin.y = y1 + ((y2 - y1) / 2);

	grpOne[0].x = origin.x;
	grpOne[0].y = origin.y - radius;
	grpOne[1].x = origin.x + radius * cos((18.0 / 180) * PI);
	grpOne[1].y = origin.y - radius * sin((18.0 / 180) * PI);
	grpOne[2].x = origin.x + radius * cos((54.0 / 180) * PI);
	grpOne[2].y = origin.y + radius * sin((54.0 / 180) * PI);
	grpOne[3].x = origin.x - radius * cos((54.0 / 180) * PI);
	grpOne[3].y = origin.y + radius * sin((54.0 / 180) * PI);
	grpOne[4].x = origin.x - radius * cos((18.0 / 180) * PI);
	grpOne[4].y = origin.y - radius * sin((18.0 / 180) * PI);

	// fill with pentagon
	hBrush = GetStockObject(WHITE_BRUSH);
	if (hBrush == NULL)
		return -3;
	hRgn = CreatePolygonRgn(grpOne, 5, WINDING);
	if (hRgn != NULL)
	{
		lRet = FillRgn(hDC, hRgn, hBrush);
		lTrash = DeleteObject(hRgn);
	}

	// draw border
	Polygon(hDC, grpOne, 5);

	// center
	grpTwo[0].x = origin.x;
	grpTwo[0].y = origin.y - radius;
	grpTwo[1].x = origin.x + radius * cos((18.0 / 180) * PI);
	grpTwo[1].y = origin.y - radius * sin((18.0 / 180) * PI);
	grpTwo[2].x = origin.x;
	grpTwo[2].y = origin.y + radius * ((5.0/8.0)/(cos((54.0/180) * PI)) - 1.0);
	grpTwo[3].x = origin.x - radius * cos((18.0 / 180) * PI);
	grpTwo[3].y = origin.y - radius * sin((18.0 / 180) * PI);

	hBrush = GetStockObject(BLACK_BRUSH);
	if (hBrush == NULL)
		return -4;
	hRgn = CreatePolygonRgn(grpTwo, 4, WINDING);
	if (hRgn != NULL)
	{
		lRet = FillRgn(hDC, hRgn, hBrush);
		lTrash = DeleteObject(hRgn);
	}

	TextOut(hDC, grpOne[3].x+3, grpOne[3].y+3, "WulphSOFT", 9);

 	return 0;
}

/***************************************************************/
/* kk 05/02/00
*/
int fillGDI(HDC hDC,
		long x1, long y1,
		long x2, long y2,
		int fillColor)
{
	POINT grpOne[4];
	long lTrash;
	long lRet;
	HGDIOBJ hBrush;
	HRGN hRgn;

	if (x2 <= x1)
		return -1;
	if (y2 <= y1)
		return -2;

	grpOne[0].x = x1;
	grpOne[0].y = y1;
	grpOne[1].x = x2;
	grpOne[1].y = y1;
	grpOne[2].x = x2;
	grpOne[2].y = y2;
	grpOne[3].x = x1;
	grpOne[3].y = y2;

	// fill
	hBrush = GetStockObject(fillColor);
	if (hBrush == NULL)
		return -3;
	hRgn = CreatePolygonRgn(grpOne, 4, WINDING);
	if (hRgn != NULL)
	{
		lRet = FillRgn(hDC, hRgn, hBrush);
		lTrash = DeleteObject(hRgn);
	}

	return 0;
}

/***************************************************************/
/* kk 05/02/00
*/
int clearWindow(HDC hDC,
		long x1, long y1,
		long x2, long y2,
		int fillColor)
{
	HBRUSH hBrush;
	HBRUSH hBrushOld;
	HPEN hPen;
	HPEN hPenOld;

	hBrush = GetStockObject(fillColor);
	if (hBrush == NULL)
		return -1;
	hBrushOld = SelectObject(hDC, hBrush);
	if (hBrushOld == NULL)
		return -2;

	/* invisible line */
	hPen = CreatePen(PS_NULL, 0, RGB(0, 0, 0));
	if (hPen == NULL)
		return -3;
	hPenOld = SelectObject(hDC, hPen);
    if (hPenOld == NULL)
		return -4;

	/* draw */
	Rectangle(hDC, x1, y1, x2, y2);

	SelectObject(hDC, hBrushOld);
	SelectObject(hDC, hPenOld);
	DeleteObject(hPen);

	return 0;
}
