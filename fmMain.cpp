//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#pragma hdrstop

#include <time.h>

#include "fmMain.h"
#include "fmAbout.h"
#include "fmDocu.h"
#include "fmOption.h"
#include "fmScoreA.h"
#include "fmScoreD.h"

#include "mtetrisa.h"
#include "mloopq.h"
#include "mscore.h"
#include "kutil.h"

// additional flags
#define DEBUG_PIECE 0   /* 0=when_release */
#define CLOSE_ALL 1  /* 1=when_release */

//---------------------------------------------------------------------------
#pragma resource "*.dfm"
TfrmMain *frmMain;
//---------------------------------------------------------------------------
#define BOARD_CELL_COLOR_BLANK 0x00FFFFFF
#define DEFT_TIMER_INTERVAL 1600

#define FNAME_INI "tetrisa.ini\0"

// capture windows message
WNDPROC OldWindowProc;

enum EM_GAME_STATE {
    EM_GS_NO_GAME=0
    , EM_GS_PLAYING=1
    , EM_GS_PAUSE=2
};
#ifndef __cplusplus
typedef int EM_GAME_STATE;
#endif

enum EM_GAME_MOVE {
    EM_GM_DOWN=0
    , EM_GM_LEFT=1
    , EM_GM_RIGHT=2
    , EM_GM_UP=3
    , EM_GM_DROP=4
};
#ifndef __cplusplus
typedef int EM_GAME_MOVE;
#endif

enum EM_BLOCK_FILL {
    EM_BF_NONE=0
    , EM_BF_MOVE=1
    , EM_BF_FIXED=2
};
#ifndef __cplusplus
typedef int EM_BLOCK_FILL;
#endif

enum EM_ACTION_MOVE {
    EM_ACT_MOVE_USER=0
    , EM_ACT_MOVE_TIMER=0
};
#ifndef __cplusplus
typedef int EM_ACTION_MOVE;
#endif

EM_GAME_STATE gameState;
LoopQueue lq;

/* debug */
bool fclear=true;

//---------------------------------------------------------------------------
__fastcall TfrmMain::TfrmMain(TComponent* Owner)
	: TForm(Owner)
{
#if CLOSE_ALL
    Label1->Visible = false;
    Label2->Visible = false;
    imgBlk000->Visible = false;
    imgBlk001->Visible = false;
    imgBlk002->Visible = false;
    imgBlk003->Visible = false;
    imgBlk004->Visible = false;
    imgBlk005->Visible = false;
    imgBlk006->Visible = false;
    imgBlk007->Visible = false;
    Label4->Visible = false;
    Edit1->Visible = false;
#endif

    // before initBoard();
    board_row = 20;
    board_col = 11;
    mLevelStart = 1;
    mLevel = 1;
	mScore = 0;

    initBoard();
    drawBoxFill(imgBoard, 0, 0, imgBoard->ClientWidth, imgBoard->ClientHeight, clWhite);

    mfIntelligentHelp = true;
	mfExtraBlock = true;
    mfDrawBack = false;

    timer_interval = DEFT_TIMER_INTERVAL;

	gameState = EM_GS_NO_GAME;
	tmrBeat->Enabled = false;
    tmrBeat->Interval = timer_interval;

    randomize();

    updateBack();
}
//---------------------------------------------------------------------------
LRESULT CALLBACK NewWindowProc(HWND hWnd, UINT msg, WPARAM w, LPARAM l)
{
	switch (msg)
	{
	case WM_NCACTIVATE:
        if (w == false)
        {
            if (gameState == EM_GS_PLAYING)
                gameState = EM_GS_PAUSE;
		}
	}
	return CallWindowProc((FARPROC)OldWindowProc, hWnd, msg, w, l);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormCreate(TObject *Sender)
{
    OldWindowProc = (WNDPROC)SetWindowLong(this->Handle,
            GWL_WNDPROC, (long)NewWindowProc);
    readOptionsToIni();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormDestroy(TObject *Sender)
{
    SetWindowLong(this->Handle, GWL_WNDPROC, (long)OldWindowProc);
    saveOptionsToIni();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::mnuExitClick(TObject *Sender)
{
	Application->Terminate();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::mnuAboutClick(TObject *Sender)
{
	frmAbout->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::mnuDocuClick(TObject *Sender)
{
	frmDocu->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormKeyDown(TObject *Sender, WORD &Key,
	TShiftState Shift)
{

    if (Shift == (TShiftState() << ssCtrl))
    {
		if (gameState == EM_GS_PLAYING)
			gameProcess(EM_GM_DROP);
//Application->MessageBox("DROP!", Application->Title.c_str(), MB_OK);
        return;
    }
	switch (Key)
	{
	case VK_LEFT:
		if (gameState == EM_GS_PLAYING)
			gameProcess(EM_GM_LEFT);
		break;
	case VK_RIGHT:
		if (gameState == EM_GS_PLAYING)
			gameProcess(EM_GM_RIGHT);
		break;
	case VK_DOWN:
		if (gameState == EM_GS_PLAYING)
			gameProcess(EM_GM_DOWN);
		break;
	case VK_UP:
		if (gameState == EM_GS_PLAYING)
			gameProcess(EM_GM_UP);
		break;
	case 'P':
	case 'p':
		if (gameState == EM_GS_NO_GAME)
		{
#if DEBUG_PIECE
            //start playing
			gameState = EM_GS_PLAYING;
			tmrBeat->Enabled = true;
#else
    // do nothing
#endif
		}
		else if (gameState == EM_GS_PLAYING)
		{
			Application->MessageBox("Pause!", Application->Title.c_str(), MB_OK);
			gameState = EM_GS_PAUSE;
			tmrBeat->Enabled = false;
		}
        else if (gameState == EM_GS_PAUSE)
		{
            //resume playing
			gameState = EM_GS_PLAYING;
			tmrBeat->Enabled = true;
		}
		break;
	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::drawBoxFill(TImage *img, long X1, long Y1, long X2, long Y2, TColor clr)
{
    TRect rect01;
    TBrush *brushOld;

    brushOld = img->Canvas->Brush;

    rect01 = Rect(X1, Y1, X2, Y2);
    img->Canvas->Brush->Color = clr;
    img->Canvas->FillRect(rect01);

    img->Canvas->Brush = brushOld;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::clearImage(TImage *img)
{
    drawBoxFill(img, 0, 0, img->ClientWidth, img->ClientHeight, clBtnFace);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::Label1Click(TObject *Sender)
{
    updateBack();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::Label2Click(TObject *Sender)
{
	updatePiece();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::tmrBeatTimer(TObject *Sender)
{
	if (gameState == EM_GS_PLAYING)
    {
        Label3->Caption = "Game is running. Press 'p' to pause.";
        Label3->Color = clBtnFace;
        Label3->Font->Color = clWindowText;
#if DEBUG_PIECE
#else
		gameProcess(EM_GM_DOWN);
#endif
    }
    else if (gameState == EM_GS_PAUSE)
    {
        Label3->Caption = "Game is paused! Press 'p' to resume play.";
        Label3->Color = clYellow;
        Label3->Font->Color = clRed;
        tmrBeat->Enabled = false;
    }
}
//---------------------------------------------------------------------------
void TfrmMain::updateScore()
{
    lblScore->Caption = "Score: " + AnsiString(mScore);
}

void TfrmMain::updateLevel()
{
    lblLevel->Caption = "Level: " + AnsiString(mLevel);
    switch (mLevel)
    {
    case 1:
        board_back_color = (long)clLime;
        break;
    case 2:
        board_back_color = (long)clTeal;
        break;
    case 3:
        board_back_color = (long)clGreen;
        break;
    case 4:
        board_back_color = (long)clYellow;
        break;
    case 5:
        board_back_color = (long)clAqua;
        break;
    case 6:
        board_back_color = (long)clBlue;
        break;
    case 7:
        board_back_color = (long)clSilver;
		break;
    case 8:
        board_back_color = (long)clRed;
        break;
	case 9:
        board_back_color = (long)clPurple;
        break;
    case 10:
        board_back_color = (long)clFuchsia;
        break;
    default:
        board_back_color = (long)clWhite;
        break;
    }
}

int TfrmMain::updateBack()
{
    long x1, y1, x2, y2;

		imgBoard->Canvas->Pen->Color = clBlack;
		imgBoard->Canvas->Pen->Mode = pmCopy;
		imgBoard->Canvas->Pen->Style = psSolid;
		imgBoard->Canvas->Pen->Width = 1;

		imgBoard->Canvas->Brush->Color = (TColor)board_back_color;
		imgBoard->Canvas->Brush->Style = bsSolid;

    x1 = board_orig_x - board_side;
    y1 = board_orig_y - board_side;
    x2 = board_orig_x + (board_col * board_side) + board_side;
    y2 = board_orig_y + (board_row * board_side) + board_side;
	imgBoard->Canvas->Rectangle(x1, y1, x2, y2);

		imgBoard->Canvas->Brush->Color = clWhite;
        imgBoard->Canvas->Brush->Style = bsSolid;

	x1 = board_orig_x-1;  /* -1 to back a bit */
	y1 = board_orig_y-1;
    x2 = board_orig_x + (board_col * board_side);
    y2 = board_orig_y + (board_row * board_side);
	imgBoard->Canvas->Rectangle(x1, y1, x2, y2);

#if 1
	bool fRet;
    long img_x, img_y;
    img_x = (board_col * board_side - imgBack->ClientWidth) / 2;
    img_y = (board_row * board_side - imgBack->ClientHeight) / 2;
    if (img_x < 0)
        img_x = 0;
    if (img_y < 0)
        img_y = 0;
if (mfDrawBack)
 {
	fRet = BitBlt(imgBoard->Canvas->Handle
            , board_orig_x, board_orig_y
            , (board_col * board_side), (board_row * board_side)
            , imgBack->Canvas->Handle
            , img_x, img_y, SRCCOPY);
	if (fRet == FALSE)
	{
		Application->MessageBox("Fail bitblt", "error", MB_OK);
	}
 }
#else
    int i, j;
    for (j=0; j<board_row; j++)
    {
        for (i=0; i<board_col; i++)
        {
    x1 = board_orig_x + i * board_side;
    y1 = board_orig_y + j * board_side;
    x2 = x1 + board_side;
    y2 = y1 + board_side;
	imgBoard->Canvas->Rectangle(x1, y1, x2, y2);

        }
	}
#endif

    return 0;
}

int TfrmMain::updatePiece()
{
	int i;
	int x;
	int y;
	int offx;
	int offy;
	int width;
	int height;
	int cx1;
	int cy1;
	int cx2;
	int cy2;

	offx = board_orig_x;
	offy = board_orig_y;
	width = board_side;
	height = board_side;

	// draw based on
	if (mGotData == 1)
	{
		// use Keep to cover back
		for (i=0; i<4; i++)
		{
			x = stKeep[i].x;
			y = stKeep[i].y;

		cx1 = offx + (x * width);
		cy1 = offy + (y * height);

		cx2 = cx1 + width - 1;
		cy2 = cy1 + width - 1;

		imgBoard->Canvas->Pen->Color = clWhite;
		imgBoard->Canvas->Pen->Mode = pmCopy;
		imgBoard->Canvas->Pen->Style = psSolid;
		imgBoard->Canvas->Pen->Width = 1;

		imgBoard->Canvas->Brush->Color = clWhite;
		imgBoard->Canvas->Brush->Style = bsSolid;

		imgBoard->Canvas->Rectangle(cx1, cy1, cx2, cy2);

		cx1 = offx + (x * width);
		cy1 = offy + (y * height);

	bool fRet;
	long img_x, img_y;
	img_x = (board_col * board_side - imgBack->ClientWidth) / 2;
	img_y = (board_row * board_side - imgBack->ClientHeight) / 2;
	if (img_x < 0)
		img_x = 0;
	if (img_y < 0)
		img_y = 0;
if (mfDrawBack)
 {
	fRet = BitBlt(imgBoard->Canvas->Handle
			, cx1, cy1
			, width, height
			, imgBack->Canvas->Handle
			, img_x + (x*width), img_y + (y*height), SRCCOPY);
	if (fRet == FALSE)
	{
		Application->MessageBox("Fail bitblt", "error", MB_OK);
	}
 }

		}

	// clear data in board
	for (i=0; i<4; i++)
	{
		if (board_cell_occup[stKeep[i].y][stKeep[i].x] == EM_BF_MOVE)
		{
			board_cell_occup[stKeep[i].y][stKeep[i].x] = EM_BF_NONE;
			board_cell_color[stKeep[i].y][stKeep[i].x] = BOARD_CELL_COLOR_BLANK;
		}
	}

	}

	// draw piece base on new coord
	for (i=0; i<4; i++)
	{
		x = stPiece[i].x;
		y = stPiece[i].y;

		imgBoard->Canvas->Pen->Color = clBlack;
		imgBoard->Canvas->Pen->Mode = pmCopy;
		imgBoard->Canvas->Pen->Style = psSolid;
		imgBoard->Canvas->Pen->Width = 1;

		imgBoard->Canvas->Brush->Color = (TColor)stPiece[i].color;
		imgBoard->Canvas->Brush->Style = bsSolid;

		cx1 = offx + (x * width);
		cy1 = offy + (y * height);

		cx2 = cx1 + width - 1;
		cy2 = cy1 + width - 1;

		imgBoard->Canvas->Rectangle(cx1, cy1, cx2, cy2);
	}
	// store data in board
	for (i=0; i<4; i++)
	{
		board_cell_occup[stPiece[i].y][stPiece[i].x] = EM_BF_MOVE;
		board_cell_color[stPiece[i].y][stPiece[i].x] = stPiece[i].color;
	}

	// keep
	for (i=0; i<4; i++)
	{
		stKeep[i].x = stPiece[i].x;
		stKeep[i].y = stPiece[i].y;
		stKeep[i].color = stPiece[i].color;
	}
	mGotData = 1;

	// kk 18/04/03
	// add in the guide-bar
	long X1, Y1;
	long X2, Y2;

	X1 = board_orig_x;
	Y1 = board_orig_y + board_row * board_side + board_side;
	X2 = board_orig_x + board_col * board_side;
	Y2 = Y1 + board_side;
	drawBoxFill(imgBoard, X1, Y1, X2, Y2, clWhite);

	int leftmost, rightmost;
	leftmost = board_col + 1;
	rightmost = -1;
	for (i=0; i<4; i++)
	{
		if (stPiece[i].x <= leftmost)
			leftmost = stPiece[i].x;
		if (stPiece[i].x >= rightmost)
			rightmost = stPiece[i].x;
	}
	X1 = board_orig_x + leftmost * board_side;
	Y1 = board_orig_y + board_row * board_side + board_side + (board_side/3);
	X2 = board_orig_x + (rightmost+1) * board_side;
	Y2 = Y1 + (board_side/3);
	drawBoxFill(imgBoard, X1, Y1, X2, Y2, clRed);

	return 0;
}

int TfrmMain::updateNextPiece()
{
	int r;
	int i;
	struct StBlock stNextPiece[4];
    struct StBlock stHold;
	int cx1, cy1;
    int cx2, cy2;
    int offx, offy;
    int minx, miny;
	int maxx, maxy;
    int x, y;
    int width, height;
    int lenx, leny;

    r = lq.peek();
    getNewPiece(&stNextPiece[0], &stHold, r, 5, 5);

    imgNextPiece->Canvas->Pen->Color = clBlack;
	imgNextPiece->Canvas->Pen->Mode = pmCopy;
	imgNextPiece->Canvas->Pen->Style = psSolid;
	imgNextPiece->Canvas->Pen->Width = 1;

	imgNextPiece->Canvas->Brush->Color = clWhite;
	imgNextPiece->Canvas->Brush->Style = bsSolid;

    cx1 = 0;
    cy1 = 0;
    cx2 = imgNextPiece->ClientWidth;
    cy2 = imgNextPiece->ClientHeight;
    imgNextPiece->Canvas->Rectangle(cx1, cy1, cx2, cy2);

    minx = 100;
	miny = 100;
    maxx = 0;
    maxy = 0;
    for (i=0; i<4; i++)
	{
        if (stNextPiece[i].x < minx)
            minx = stNextPiece[i].x;
        if (stNextPiece[i].y < miny)
            miny = stNextPiece[i].y;
        if (stNextPiece[i].x > maxx)
            maxx = stNextPiece[i].x;
        if (stNextPiece[i].y > maxy)
            maxy = stNextPiece[i].y;
    }
    if (minx < miny)
        miny = minx;
    if (miny < minx)
        minx = miny;
    if (maxx > maxy)
        maxy = maxx;
    if (maxy < maxx)
        maxx = maxy;

	width = board_side;
	height = board_side;
    lenx = (maxx-minx) > 0 ? maxx-minx : 1;
    leny = (maxy-miny) > 0 ? maxy-miny : 1;
	offx = (imgNextPiece->ClientWidth - width * lenx) / 2.0;
    offy = (imgNextPiece->ClientHeight - height * leny) / 2.0;
    if (offx <= 4)
        offx = 4;
	if (offy <= 4)
        offy = 4;
	for (i=0; i<4; i++)
	{
		x = stNextPiece[i].x - minx;
		y = stNextPiece[i].y - miny;

		imgNextPiece->Canvas->Pen->Color = clBlack;
		imgNextPiece->Canvas->Pen->Mode = pmCopy;
		imgNextPiece->Canvas->Pen->Style = psSolid;
		imgNextPiece->Canvas->Pen->Width = 1;

		imgNextPiece->Canvas->Brush->Color = (TColor)stNextPiece[i].color;
		imgNextPiece->Canvas->Brush->Style = bsSolid;

		cx1 = offx + (x * width);
		cy1 = offy + (y * height);

		cx2 = cx1 + width - 1;
		cy2 = cy1 + width - 1;

		imgNextPiece->Canvas->Rectangle(cx1, cy1, cx2, cy2);
	}

    return 0;
}

int TfrmMain::initBoard()
{
	int i, j;

	mGotData = 0;
	mDownUser = 0;
	mDownTimer = 0;

	board_back_color = RGB(255, 255, 255);

	board_side = 12;
    block_start_x = board_col / 2;
    block_start_y = 3;

	imgBoard->ClientWidth = (MAX_COL * board_side) + board_side + board_side;
	imgBoard->ClientHeight = (MAX_ROW * board_side) + board_side + board_side + board_side;

	board_orig_x = (imgBoard->ClientWidth - board_col * board_side) / 2;
	board_orig_y = (imgBoard->ClientHeight - board_row * board_side - board_side) / 2;

	for (j=0; j<MAX_ROW; j++)
	{
		for (i=0; i<MAX_COL; i++)
		{
			board_cell_occup[j][i] = EM_BF_NONE;
			board_cell_color[j][i] = BOARD_CELL_COLOR_BLANK;
		}
	}

	block_extra_max_row_spacing = MIN_ROW;

    return 0;
}

int TfrmMain::updateBoard()
{
    int i, j;
	int x;
	int y;
	int offx;
	int offy;
	int width;
	int height;
	int cx1;
	int cy1;
	int cx2;
	int cy2;

	offx = board_orig_x;
	offy = board_orig_y;
	width = board_side;
	height = board_side;

    for (j=0; j<board_row; j++)
    {
        for (i=0; i<board_col; i++)
        {
            if (board_cell_occup[j][i] == EM_BF_FIXED)
            {
        		x = i;
		        y = j;

		imgBoard->Canvas->Pen->Color = clBlack;
		imgBoard->Canvas->Pen->Mode = pmCopy;
		imgBoard->Canvas->Pen->Style = psSolid;
		imgBoard->Canvas->Pen->Width = 1;

		imgBoard->Canvas->Brush->Color = (TColor)board_cell_color[j][i];
		imgBoard->Canvas->Brush->Style = bsSolid;

		cx1 = offx + (x * width);
		cy1 = offy + (y * height);

		cx2 = cx1 + width - 1;
		cy2 = cy1 + width - 1;

		imgBoard->Canvas->Rectangle(cx1, cy1, cx2, cy2);
			}
        }
	}

    return 0;
}

int TfrmMain::redrawBoardAndPiece()
{
    updateBack();
    updatePiece();
    updateBoard();
    return 0;
}

int TfrmMain::canOccupyCell(struct StBlock stBlkNew[4], struct StBlock stBlkOld[4])
{
    int i, j;
    bool fSameLocMovingPiece;

    for (i=0; i<4; i++)
    {
        if (board_cell_occup[stBlkNew[i].y][stBlkNew[i].x] != EM_BF_NONE)
        {
            fSameLocMovingPiece = false;
            for (j=0; j<4; j++)
            {
				if ( (stBlkNew[i].x == stBlkOld[j].x)
                        && (stBlkNew[i].y == stBlkOld[j].y))
                {
                    fSameLocMovingPiece = true;
                    break;
                }
            }
            if (fSameLocMovingPiece == false)
                goto abort;
        }
    }

    return 0;
abort:
    return -1;
}

/*
    calculate next position
    if ok no overlap, out of bound, end of row
        then draw new pos and record
    if no, ignore this action
*/
int TfrmMain::movePiece(int move)
{
	int i;
    struct StBlock stTemp[4];
    int nextBlkState;

	switch (move)
	{
	case EM_GM_LEFT:
//		Application->MessageBox("LEFT", "", MB_OK);
		for (i=0; i<4; i++)
		{
			stTemp[i].x = stPiece[i].x - 1;
            stTemp[i].y = stPiece[i].y;
            stTemp[i].color = stPiece[i].color;
		}
		for (i=0; i<4; i++)
        {
            if (stTemp[i].x < 0)
                goto abort;
            if (canOccupyCell(stTemp, stPiece) != 0)
                goto abort;
        }
        copyBlocks(&stPiece[0], &stTemp[0]);
		stCrux.x = stCrux.x - 1;
		updatePiece();
		break;
	case EM_GM_RIGHT:
//		Application->MessageBox("RIGHT", "", MB_OK);
		for (i=0; i<4; i++)
		{
			stTemp[i].x = stPiece[i].x + 1;
            stTemp[i].y = stPiece[i].y;
            stTemp[i].color = stPiece[i].color;
		}
		for (i=0; i<4; i++)
        {
            if (stTemp[i].x >= board_col)
                goto abort;
            if (canOccupyCell(stTemp, stPiece) != 0)
                goto abort;
        }
        copyBlocks(&stPiece[0], &stTemp[0]);
		stCrux.x = stCrux.x + 1;
		updatePiece();
		break;
	case EM_GM_DOWN:
//		Application->MessageBox("DOWN", "", MB_OK);
		for (i=0; i<4; i++)
		{
			stTemp[i].x = stPiece[i].x;
            stTemp[i].y = stPiece[i].y + 1;
            stTemp[i].color = stPiece[i].color;
		}
		for (i=0; i<4; i++)
        {
            if (stTemp[i].y >= board_row)
                goto abort;
            if (canOccupyCell(stTemp, stPiece) != 0)
                goto abort;
        }
        copyBlocks(&stPiece[0], &stTemp[0]);
		stCrux.y = stCrux.y + 1;
		updatePiece();
		break;
	case EM_GM_UP:
//		Application->MessageBox("UP", "", MB_OK);
        if (rotatePiece(stTemp, &nextBlkState) != 0)
            goto abort;
    	for (i=0; i<4; i++)
        {
            if (stTemp[i].x < 0)
                goto abort;
            if (stTemp[i].x >= board_col)
                goto abort;
            if (stTemp[i].y < 0)
                goto abort;
            if (stTemp[i].y >= board_row)
                goto abort;

            if (canOccupyCell(stTemp, stPiece) != 0)
                goto abort;
        }
        blockState = nextBlkState;  // set new state
        copyBlocks(&stPiece[0], &stTemp[0]);
        // no change in crux
   		updatePiece();
		break;
    case EM_GM_DROP:
//Label3->Caption = "Drop not implemented";
        // done by the EM_GM_DOWN
        break;
	}
	return 0;
abort:
    return -1;
}

int TfrmMain::highestRow()
{
    int nRet;
    int i, j;

    nRet = board_row;

    for (j=0; j<board_row; j++)
    {
        for (i=0; i<board_col; i++)
        {
            if (board_cell_occup[j][i] == EM_BF_FIXED)
            {
                nRet = j;
                goto finDone;
            }
        }
    }
finDone:
    return nRet;
}

int TfrmMain::genRandPiece()
{
	int nRet;
	int r;

	nRet = 0;

	r = random(7);
	nRet = 0 + r + 1;

	return nRet;
}

int TfrmMain::genRandPiece_A()
{
	int nRet;
	int r;

	nRet = 0;

	if (mfExtraBlock == false)
		return (nRet = genRandPiece());

    if (highestRow() < block_extra_max_row_spacing + mLevel)
        return (nRet = genRandPiece());

    // flag: BLK_000
             
    r = random(100);
	if      ( (r >= 0) && (r < 70) )
	{
		r = random(7);
		nRet = 0 + r + 1;
	}
	else if ( (r >= 70) && (r < 80) )
	{
		r = random(2);
		nRet = 7 + r + 1;
	}
	else if ( (r >= 80) && (r < 95) )
	{
		r = random(17);
		nRet = 9 + r + 1;
	}
	else
	{
		r = random(7);
		nRet = 26 + r + 1;
	}

	return nRet;
}

int TfrmMain::canMoveDownOneStep()
{
    int i;
    struct StBlock stTemp[4];
    bool fCanMoveDown;

    fCanMoveDown = false;

    for (i=0; i<4; i++)
    {
        stTemp[i].x = stPiece[i].x;
        stTemp[i].y = stPiece[i].y + 1;
        stTemp[i].color = stPiece[i].color;
    }
	for (i=0; i<4; i++)
    {
        if (stTemp[i].y >= board_row)
            goto outHere1;
        if (canOccupyCell(stTemp, stPiece) != 0)
            goto outHere1;
    }
    fCanMoveDown = true;
outHere1:
    if (fCanMoveDown == true)
        return 0;
    else
        return -1;
}

int TfrmMain::gameFinish()
{
    Label3->Caption = "Finish!!!";
    Label3->Color = clPurple;
    Label3->Font->Color = clRed;
	gameState = EM_GS_NO_GAME;
    tmrBeat->Enabled = false;

    mnuNew->Enabled = true;
    mnuTerminate->Enabled = false;
    mnuScore->Enabled = true;
    mnuOptions->Enabled = true;

    char bufText[128];
    sprintf(bufText, "You have scored (%d) at the Level (%d).", mScore, mLevel);
    frmScoreAsk->lblPrompt->Caption = AnsiString(bufText);
    frmScoreAsk->edtName->Text = "";
    frmScoreAsk->ShowModal();

    mnuScoreClick(this);

    return 0;
}

int TfrmMain::gameStart()
{
	gameState = EM_GS_PLAYING;
	tmrBeat->Enabled = true;

    mnuNew->Enabled = false;
    mnuTerminate->Enabled = true;
    mnuScore->Enabled = false;
    mnuOptions->Enabled = false;

    return 0;
}

int TfrmMain::gameProcess(int move)
{
    int nRet1, nRet2;
	int i, j;
    int k;
    int r;
    struct StBlock stTemp[4];
    struct StBlock stHold;
    struct StBlock stNull;
    int score;
    int row_eliminated;
    int num_col_filled;

    if ((move != EM_GM_DOWN) && (move != EM_GM_DROP))
    {
        // quit if not possible to move left, right, up
        nRet1 = movePiece(move);
        if (nRet1 != 0)
            return nRet1;
        return 1;
    }
    if (move == EM_GM_DOWN)
    {
        nRet2 = canMoveDownOneStep();
        if (nRet2 == 0)
        {
            nRet1 = movePiece(move);
            if (nRet1 != 0)
                goto touchDown;
            return 2;
        }
    }
    if (move == EM_GM_DROP)
    {
        do
        {
            nRet2 = canMoveDownOneStep();
            if (nRet2 == 0)
            {
                nRet1 = movePiece(EM_GM_DOWN);
                if (nRet1 != 0)
                    goto touchDown;
            }
        } while (nRet2 == 0);
    }


touchDown:
    if (move == EM_GM_DROP)
    {
        // forfeit the chance to rotate anymore
        mDownUser = 1;
    }
    if (mDownUser == 0)
    {
        mDownUser = 1;
        return 2;
    }
    mDownUser = 0;

    tmrBeat->Enabled = false;

	for (i=0; i<4; i++)
    {
        board_cell_occup[stPiece[i].y][stPiece[i].x] = EM_BF_FIXED;
        board_cell_color[stPiece[i].y][stPiece[i].x] = stPiece[i].color;
    }

    /* calculate score
        elimate rows, update stuff
        next level?
    */
    row_eliminated = 0;
    for (j=0; j<board_row; j++)
    {
        num_col_filled = 0;
        for (i=0; i<board_col; i++)
        {
            if (board_cell_occup[j][i] == EM_BF_FIXED)
                num_col_filled++;
        }
        if (num_col_filled == board_col)
        {
            board_row_filled[j] = 1;
            row_eliminated++;
        }
        else
            board_row_filled[j] = 0;
    }

    for (k=0; k<board_row; k++)
    {
        if (board_row_filled[k] == 1)
        {
            if (k == 0)
            {
                j = 0;
                for (i=0; i<board_col; i++)
                {
                    board_cell_occup[j][i] = EM_BF_NONE;
                    board_cell_color[j][i] = BOARD_CELL_COLOR_BLANK;
                }
            }
            else
            {
                for (j=0; j<k; j++)
                {
                    for (i=0; i<board_col; i++)
                    {
                        /* bottom up */
                        board_cell_occup[k-j][i] = board_cell_occup[k-j-1][i];
                        board_cell_color[k-j][i] = board_cell_color[k-j-1][i];
                    }
                }
                j = 0;
                for (i=0; i<board_col; i++)
                {
                    board_cell_occup[j][i] = EM_BF_NONE;
                    board_cell_color[j][i] = BOARD_CELL_COLOR_BLANK;
                }
            }
        }
    }

    switch (row_eliminated)
    {
    case 0:
        score = 1;
        break;
    case 1:
        score = 20;
        break;
    case 2:
        score = 40;
        break;
    case 3:
        score = 80;
        break;
    case 4:
        score = 160;
        break;
    default:
        score = 200;
        break;
    }
    mScore = mScore + score;
    updateScore();


    i = mScore / 200;
    i = i % 10;
    i = i + 1;
    if (i > mLevel)
    {
        mLevel = i;
        tmrBeat->Interval = timer_interval * (10-mLevel+1)/10;
Label3->Caption = "Change to Level " + AnsiString(mLevel);
Label3->Color = clYellow;
Label3->Font->Color = clRed;
    }
    else if ((i == 1) && (mLevel == 10))
    {
        mLevel = i;
        tmrBeat->Interval = timer_interval * (10-mLevel+1)/10;
Label3->Caption = "Change to Level " + AnsiString(mLevel);
Label3->Color = clYellow;
Label3->Font->Color = clRed;
    }
    updateLevel();

    /* new piece */
    r = lq.pop();

    /* check overlap or not, reached the top */
    getNewPiece(&stTemp[0], &stHold, r, block_start_x, block_start_y);
    stNull.x = MAX_COL + MAX_COL;
    stNull.y = MAX_ROW + MAX_ROW;
    stNull.color = stHold.color;
    if (canOccupyCell(stTemp, &stNull) != 0)
    {
        gameFinish();
        return 101;
    }
    copyBlock(&stCrux, &stHold);
    copyBlocks(&stPiece[0], &stTemp[0]);
    updateNewPiece(r, block_start_x, block_start_y);
    updateNextPiece();

    // no help
    if (mfIntelligentHelp == false)
    {
        r = genRandPiece_A();
        lq.push(r);
    }
    else
    {
        /*
            box of (>3) * 2
            vertical bar of (>4) * 1
            vertical bar of 2 * 1, with left_right wing
        */
        r = 0;

        if (mUseIntelligentHelp == 1)
        {
            mUseIntelligentHelp = 0;
            goto finIntelligent;
        }

        /* bottom up */

        // case 1
        for (j=0; j<board_row-3+1; j++)
        {
            for (i=0; i<board_col-2+1; i++)
            {
                if (i==0)
                {
                    if ( (board_cell_occup[board_row-1-j-0][i] == EM_BF_NONE)
                            && (board_cell_occup[board_row-1-j-0][i+1] == EM_BF_NONE)
                            && (board_cell_occup[board_row-1-j-0][i+2] == EM_BF_FIXED)
                            && (board_cell_occup[board_row-1-j-1][i] == EM_BF_NONE)
                            && (board_cell_occup[board_row-1-j-1][i+1] == EM_BF_NONE)
                            && (board_cell_occup[board_row-1-j-1][i+2] == EM_BF_FIXED)
                            && (board_cell_occup[board_row-1-j-2][i] == EM_BF_NONE)
                            && (board_cell_occup[board_row-1-j-2][i+1] == EM_BF_NONE)
                            && (board_cell_occup[board_row-1-j-2][i+2] == EM_BF_FIXED)
                    )
                    {
                        r = BLK_001;
                        if ( (lq.isInData(r)) || (blockType == r) )
                        {
//Memo1->Lines->Add("case 1a  omit");
                            r = 0;
                        }
                        else
                        {
//Memo1->Lines->Add("case 1a");
                            mUseIntelligentHelp = 1;
                        }
                        goto finIntelligent;
                    }
                }
                else if (i == board_col-2)
                {
                    if ( (board_cell_occup[board_row-1-j-0][i-1] == EM_BF_FIXED)
                            && (board_cell_occup[board_row-1-j-0][i] == EM_BF_NONE)
                            && (board_cell_occup[board_row-1-j-0][i+1] == EM_BF_NONE)
                            && (board_cell_occup[board_row-1-j-1][i-1] == EM_BF_FIXED)
                            && (board_cell_occup[board_row-1-j-1][i] == EM_BF_NONE)
                            && (board_cell_occup[board_row-1-j-1][i+1] == EM_BF_NONE)
                            && (board_cell_occup[board_row-1-j-2][i-1] == EM_BF_FIXED)
                            && (board_cell_occup[board_row-1-j-2][i] == EM_BF_NONE)
                            && (board_cell_occup[board_row-1-j-2][i+1] == EM_BF_NONE)
                    )
                    {
                        r = BLK_001;
                        if ( (lq.isInData(r)) || (blockType == r) )
                        {
//Memo1->Lines->Add("case 1b  omit");
                            r = 0;
                        }
                        else
                        {
//Memo1->Lines->Add("case 1b");
                            mUseIntelligentHelp = 1;
                        }
                        goto finIntelligent;
                    }
                }
                else
                {
                    if ( (board_cell_occup[board_row-1-j-0][i+0] == EM_BF_FIXED)
                            && (board_cell_occup[board_row-1-j-0][i] == EM_BF_NONE)
                            && (board_cell_occup[board_row-1-j-0][i+1] == EM_BF_NONE)
                            && (board_cell_occup[board_row-1-j-0][i+2] == EM_BF_FIXED)
                            && (board_cell_occup[board_row-1-j-1][i-1] == EM_BF_FIXED)
                            && (board_cell_occup[board_row-1-j-1][i] == EM_BF_NONE)
                            && (board_cell_occup[board_row-1-j-1][i+1] == EM_BF_NONE)
                            && (board_cell_occup[board_row-1-j-1][i+2] == EM_BF_FIXED)
                            && (board_cell_occup[board_row-1-j-2][i-1] == EM_BF_FIXED)
                            && (board_cell_occup[board_row-1-j-2][i] == EM_BF_NONE)
                            && (board_cell_occup[board_row-1-j-2][i+1] == EM_BF_NONE)
                            && (board_cell_occup[board_row-1-j-2][i+2] == EM_BF_FIXED)
                    )
                    {
                        r = BLK_001;
                        if ( (lq.isInData(r)) || (blockType == r) )
                        {
//Memo1->Lines->Add("case 1c  omit");
                            r = 0;
                        }
                        else
                        {
//Memo1->Lines->Add("case 1c");
                            mUseIntelligentHelp = 1;
                        }
                        goto finIntelligent;
                    }
                }
            }
        }

        // case 2
        for (j=0; j<board_row-4+1; j++)
        {
            for (i=0; i<board_col-1+1; i++)
            {
                if (i==0)
                {
                    if ( (board_cell_occup[board_row-1-j-0][i] == EM_BF_NONE)
                            && (board_cell_occup[board_row-1-j-0][i+1] == EM_BF_FIXED)
                            && (board_cell_occup[board_row-1-j-1][i] == EM_BF_NONE)
                            && (board_cell_occup[board_row-1-j-1][i+1] == EM_BF_FIXED)
                            && (board_cell_occup[board_row-1-j-2][i] == EM_BF_NONE)
                            && (board_cell_occup[board_row-1-j-2][i+1] == EM_BF_FIXED)
                            && (board_cell_occup[board_row-1-j-3][i] == EM_BF_NONE)
                            && (board_cell_occup[board_row-1-j-3][i+1] == EM_BF_FIXED)
                    )
                    {
                        r = BLK_002;
                        if ( (lq.isInData(r)) || (blockType == r) )
                        {
//Memo1->Lines->Add("case 2a  omit");
                            r = 0;
                        }
                        else
                        {
//Memo1->Lines->Add("case 2a");
                            mUseIntelligentHelp = 1;
                        }
                        goto finIntelligent;
                    }
                }
                else if (i == board_col-1)
                {
                    if ( (board_cell_occup[board_row-1-j-0][i-1] == EM_BF_FIXED)
                            && (board_cell_occup[board_row-1-j-0][i] == EM_BF_NONE)
                            && (board_cell_occup[board_row-1-j-1][i-1] == EM_BF_FIXED)
                            && (board_cell_occup[board_row-1-j-1][i] == EM_BF_NONE)
                            && (board_cell_occup[board_row-1-j-2][i-1] == EM_BF_FIXED)
                            && (board_cell_occup[board_row-1-j-2][i] == EM_BF_NONE)
                            && (board_cell_occup[board_row-1-j-3][i-1] == EM_BF_FIXED)
                            && (board_cell_occup[board_row-1-j-3][i] == EM_BF_NONE)
                    )
                    {
                        r = BLK_002;
                        if ( (lq.isInData(r)) || (blockType == r) )
                        {
//Memo1->Lines->Add("case 2b  omit");
                            r = 0;
                        }
                        else
                        {
//Memo1->Lines->Add("case 2b");
                            mUseIntelligentHelp = 1;
                        }
                        goto finIntelligent;
                    }
                }
                else
                {
                    if ( (board_cell_occup[board_row-1-j-0][i-1] == EM_BF_FIXED)
                            && (board_cell_occup[board_row-1-j-0][i] == EM_BF_NONE)
                            && (board_cell_occup[board_row-1-j-0][i+1] == EM_BF_FIXED)
                            && (board_cell_occup[board_row-1-j-1][i-1] == EM_BF_FIXED)
                            && (board_cell_occup[board_row-1-j-1][i] == EM_BF_NONE)
                            && (board_cell_occup[board_row-1-j-1][i+1] == EM_BF_FIXED)
                            && (board_cell_occup[board_row-1-j-2][i-1] == EM_BF_FIXED)
                            && (board_cell_occup[board_row-1-j-2][i] == EM_BF_NONE)
                            && (board_cell_occup[board_row-1-j-2][i+1] == EM_BF_FIXED)
                            && (board_cell_occup[board_row-1-j-3][i-1] == EM_BF_FIXED)
                            && (board_cell_occup[board_row-1-j-3][i] == EM_BF_NONE)
                            && (board_cell_occup[board_row-1-j-3][i+1] == EM_BF_FIXED)
                    )
                    {
                        r = BLK_002;
                        if ( (lq.isInData(r)) || (blockType == r) )
                        {
//Memo1->Lines->Add("case 2c  omit");
                            r = 0;
                        }
                        else
                        {
//Memo1->Lines->Add("case 2c");
                            mUseIntelligentHelp = 1;
                        }
                        goto finIntelligent;
                    }
                }
            }

        }

        // case 3
        for (j=0; j<board_row-3+1; j++)
        {
            for (i=0; i<board_col-1+1; i++)
            {
                if (i==0)
                {
                    if ( (board_cell_occup[board_row-1-j-0][i] == EM_BF_NONE)
                            && (board_cell_occup[board_row-1-j-0][i+1] == EM_BF_FIXED)
                            && (board_cell_occup[board_row-1-j-1][i] == EM_BF_NONE)
                            && (board_cell_occup[board_row-1-j-1][i+1] == EM_BF_FIXED)
                            && (board_cell_occup[board_row-1-j-2][i] == EM_BF_NONE)
                            && (board_cell_occup[board_row-1-j-2][i+1] == EM_BF_NONE)
                    )
                    {
                        r = BLK_007;
                        if ( (lq.isInData(r)) || (blockType == r) )
                        {
//Memo1->Lines->Add("case 3a - inv L  omit");
                            r = 0;
                        }
                        else
                        {
//Memo1->Lines->Add("case 3a - inv L");
                            mUseIntelligentHelp = 1;
                        }
                        goto finIntelligent;
                    }
                }
                else if (i == board_col-1)
                {
                    if ( (board_cell_occup[board_row-1-j-0][i-1] == EM_BF_FIXED)
                            && (board_cell_occup[board_row-1-j-0][i] == EM_BF_NONE)
                            && (board_cell_occup[board_row-1-j-1][i-1] == EM_BF_FIXED)
                            && (board_cell_occup[board_row-1-j-1][i] == EM_BF_NONE)
                            && (board_cell_occup[board_row-1-j-2][i-1] == EM_BF_FIXED)
                            && (board_cell_occup[board_row-1-j-2][i] == EM_BF_NONE)
                    )
                    {
                        r = BLK_006;
                        if ( (lq.isInData(r)) || (blockType == r) )
                        {
//Memo1->Lines->Add("case 3b - vor L  omit");
                            r = 0;
                        }
                        else
                        {
//Memo1->Lines->Add("case 3b - vor L");
                            mUseIntelligentHelp = 1;
                        }
                        goto finIntelligent;
                    }
                }
                else
                {
                    if ( (board_cell_occup[board_row-1-j-0][i-1] == EM_BF_FIXED)
                            && (board_cell_occup[board_row-1-j-0][i] == EM_BF_NONE)
                            && (board_cell_occup[board_row-1-j-0][i+1] == EM_BF_FIXED)
                            && (board_cell_occup[board_row-1-j-1][i-1] == EM_BF_FIXED)
                            && (board_cell_occup[board_row-1-j-1][i] == EM_BF_NONE)
                            && (board_cell_occup[board_row-1-j-1][i+1] == EM_BF_FIXED)

                            && (board_cell_occup[board_row-1-j-2][i] == EM_BF_NONE)

                    )
                    {
                        if (board_cell_occup[board_row-1-j-2][i-1] == EM_BF_NONE)
                        {
                            r = BLK_006;
                        if ( (lq.isInData(r)) || (blockType == r) )
                            {
//Memo1->Lines->Add("case 3c - vor L  omit");
                                r = 0;
                            }
                            else
                            {
//Memo1->Lines->Add("case 3c - vor L");
                                mUseIntelligentHelp = 1;
                            }
                            goto finIntelligent;
                        }
                        if (board_cell_occup[board_row-1-j-2][i+1] == EM_BF_NONE)
                        {
                            r = BLK_007;
                        if ( (lq.isInData(r)) || (blockType == r) )
                            {
//Memo1->Lines->Add("case 3d - inv L  omit");
                                r = 0;
                            }
                            else
                            {
//Memo1->Lines->Add("case 3d - inv L");
                                mUseIntelligentHelp = 1;
                            }
                            goto finIntelligent;
                        }
                    }
                }
            }
        }

finIntelligent:
        if (r == 0)
			r = genRandPiece_A();
        lq.push(r);
    }

    updateBoard();
    redrawBoardAndPiece();


    tmrBeat->Enabled = true;
    return 0;
}

void TfrmMain::copyBlock(struct StBlock *stA, struct StBlock *stB)
{
    stA->x = stB->x;
    stA->y = stB->y;
    stA->color = stB->color;
}

void TfrmMain::copyBlocks(struct StBlock stA[4], struct StBlock stB[4])
{
    int i;
    for (i=0; i<4; i++)
    {
        copyBlock(&stA[i], &stB[i]);
    }
}

int TfrmMain::nextBlockState(int blkType, int blkState)
{
    int nRet;

    switch (blkType)
    {
	case BLK_000:
        nRet = 0;
		break;
    case BLK_001:
        nRet = 0;
        break;
    case BLK_002:
        if (blkState == 1)
            nRet = 0;
        else
            nRet = blkState + 1;
        break;
    case BLK_003:
        if (blkState == 1)
            nRet = 0;
        else
            nRet = blkState + 1;
        break;
    case BLK_004:
        if (blkState == 1)
            nRet = 0;
        else
            nRet = blkState + 1;
        break;
    case BLK_005:
        if (blkState == 3)
            nRet = 0;
        else
            nRet = blkState + 1;
        break;
    case BLK_006:
        if (blkState == 3)
            nRet = 0;
        else
            nRet = blkState + 1;
        break;
    case BLK_007:
        if (blkState == 3)
            nRet = 0;
        else
            nRet = blkState + 1;
        break;
    case BLK_008:
        if (blkState == 3)
            nRet = 0;
        else
            nRet = blkState + 1;
        break;
    case BLK_009:
        if (blkState == 3)
            nRet = 0;
        else
            nRet = blkState + 1;
        break;
    case BLK_010:
        if (blkState == 1)
            nRet = 0;
        else
            nRet = blkState + 1;
        break;
    case BLK_011:
        if (blkState == 1)
            nRet = 0;
        else
            nRet = blkState + 1;
        break;
    case BLK_012:
        if (blkState == 3)
            nRet = 0;
        else
            nRet = blkState + 1;
        break;
    case BLK_013:
        if (blkState == 3)
            nRet = 0;
        else
            nRet = blkState + 1;
        break;
    case BLK_014:
        if (blkState == 3)
            nRet = 0;
        else
            nRet = blkState + 1;
        break;
    case BLK_015:
        if (blkState == 3)
            nRet = 0;
        else
            nRet = blkState + 1;
        break;
    case BLK_016:
        if (blkState == 1)
            nRet = 0;
        else
            nRet = blkState + 1;
        break;
    case BLK_017:
        if (blkState == 1)
            nRet = 0;
        else
            nRet = blkState + 1;
        break;
    case BLK_018:
        if (blkState == 3)
            nRet = 0;
        else
            nRet = blkState + 1;
        break;
    case BLK_019:
        if (blkState == 3)
            nRet = 0;
        else
            nRet = blkState + 1;
        break;
    case BLK_020:
        if (blkState == 3)
            nRet = 0;
        else
            nRet = blkState + 1;
        break;
    case BLK_021:
        if (blkState == 3)
            nRet = 0;
        else
            nRet = blkState + 1;
        break;
    case BLK_022:
        if (blkState == 3)
            nRet = 0;
        else
            nRet = blkState + 1;
        break;
    case BLK_023:
        if (blkState == 3)
            nRet = 0;
        else
            nRet = blkState + 1;
        break;
    case BLK_024:
        if (blkState == 3)
            nRet = 0;
        else
            nRet = blkState + 1;
        break;
    case BLK_025:
        if (blkState == 3)
            nRet = 0;
        else
            nRet = blkState + 1;
        break;
    case BLK_026:
        if (blkState == 3)
            nRet = 0;
        else
            nRet = blkState + 1;
        break;
    case BLK_027:
        if (blkState == 3)
            nRet = 0;
        else
            nRet = blkState + 1;
        break;
    case BLK_028:
        if (blkState == 3)
            nRet = 0;
        else
            nRet = blkState + 1;
        break;
    case BLK_029:
        if (blkState == 1)
            nRet = 0;
        else
            nRet = blkState + 1;
        break;
    case BLK_030:
        if (blkState == 1)
            nRet = 0;
        else
            nRet = blkState + 1;
        break;
    case BLK_031:
        if (blkState == 3)
            nRet = 0;
        else
            nRet = blkState + 1;
        break;
    case BLK_032:
        if (blkState == 1)
            nRet = 0;
        else
            nRet = blkState + 1;
        break;
    case BLK_033:
        nRet = 0;
        break;
    default:
        return -1;
    }
    return nRet;
}

int TfrmMain::fillBlock(struct StBlock stBlk[4], struct StBlock *stCrx, int blkType, int blkState)
{
    switch (blkType)
    {
    case BLK_000:
        switch (blkState)
        {
		case 0:
			break;
        default:
            return -2;
		}
		break;
    case BLK_001:
        switch (blkState)
        {
        case 0:
            stBlk[0].x = stCrx->x;
            stBlk[0].y = stCrx->y;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x + 1;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x;
            stBlk[2].y = stCrx->y + 1;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x + 1;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        default:
            return -2;
        }
        break;
    case BLK_002:
        switch (blkState)
        {
        case 0:
            stBlk[0].x = stCrx->x - 1;
            stBlk[0].y = stCrx->y;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x + 1;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x + 2;
            stBlk[3].y = stCrx->y;
            stBlk[3].color = stCrx->color;
            break;
        case 1:
            stBlk[0].x = stCrx->x;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x;
            stBlk[2].y = stCrx->y + 1;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x;
            stBlk[3].y = stCrx->y + 2;
            stBlk[3].color = stCrx->color;
            break;
        default:
            return -2;
        }
        break;
    case BLK_003:
        switch (blkState)
        {
        case 0:
            stBlk[0].x = stCrx->x - 1;
            stBlk[0].y = stCrx->y + 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x;
            stBlk[1].y = stCrx->y + 1;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x + 1;
            stBlk[3].y = stCrx->y;
            stBlk[3].color = stCrx->color;
            break;
        case 1:
            stBlk[0].x = stCrx->x - 1;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x - 1;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        default:
            return -2;
        }
        break;
    case BLK_004:
        switch (blkState)
        {
        case 0:
            stBlk[0].x = stCrx->x - 1;
            stBlk[0].y = stCrx->y;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x;
            stBlk[2].y = stCrx->y + 1;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x + 1;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        case 1:
            stBlk[0].x = stCrx->x + 1;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x + 1;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        default:
            return -2;
        }
        break;
    case BLK_005:
        switch (blkState)
        {
        case 0:
            stBlk[0].x = stCrx->x - 1;
            stBlk[0].y = stCrx->y;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x + 1;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        case 1:
            stBlk[0].x = stCrx->x - 1;
            stBlk[0].y = stCrx->y;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x;
            stBlk[2].y = stCrx->y - 1;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        case 2:
            stBlk[0].x = stCrx->x;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x - 1;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x + 1;
            stBlk[3].y = stCrx->y;
            stBlk[3].color = stCrx->color;
            break;
        case 3:
            stBlk[0].x = stCrx->x;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x + 1;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        default:
            return -2;
        }
        break;
    case BLK_006:
        switch (blkState)
        {
        case 0:
            stBlk[0].x = stCrx->x - 1;
            stBlk[0].y = stCrx->y;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x + 1;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x - 1;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        case 1:
            stBlk[0].x = stCrx->x - 1;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x;
            stBlk[1].y = stCrx->y - 1;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        case 2:
            stBlk[0].x = stCrx->x - 1;
            stBlk[0].y = stCrx->y;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x + 1;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x + 1;
            stBlk[3].y = stCrx->y - 1;
            stBlk[3].color = stCrx->color;
            break;
        case 3:
            stBlk[0].x = stCrx->x;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x;
            stBlk[2].y = stCrx->y + 1;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x + 1;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        default:
            return -2;
        }
        break;
    case BLK_007:
        switch (blkState)
        {
        case 0:
            stBlk[0].x = stCrx->x - 1;
            stBlk[0].y = stCrx->y;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x + 1;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x + 1;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        case 1:
            stBlk[0].x = stCrx->x;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x;
            stBlk[2].y = stCrx->y + 1;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x - 1;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        case 2:
            stBlk[0].x = stCrx->x - 1;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x - 1;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x + 1;
            stBlk[3].y = stCrx->y;
            stBlk[3].color = stCrx->color;
            break;
        case 3:
            stBlk[0].x = stCrx->x;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x + 1;
            stBlk[1].y = stCrx->y - 1;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        default:
            return -2;
        }
        break;
    case BLK_008:
        switch (blkState)
        {
		case 0:
            stBlk[0].x = stCrx->x - 2;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x - 1;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x + 1;
            stBlk[3].y = stCrx->y;
            stBlk[3].color = stCrx->color;
			break;
		case 1:
            stBlk[0].x = stCrx->x + 1;
            stBlk[0].y = stCrx->y - 2;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x;
            stBlk[1].y = stCrx->y - 1;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
			break;
		case 2:
            stBlk[0].x = stCrx->x - 1;
            stBlk[0].y = stCrx->y;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x + 1;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x + 2;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
			break;
		case 3:
            stBlk[0].x = stCrx->x;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x;
            stBlk[2].y = stCrx->y + 1;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x - 1;
            stBlk[3].y = stCrx->y + 2;
            stBlk[3].color = stCrx->color;
			break;
        default:
            return -2;
		}
		break;
    case BLK_009:
        switch (blkState)
        {
		case 0:
            stBlk[0].x = stCrx->x - 1;
            stBlk[0].y = stCrx->y;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x + 1;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x + 2;
            stBlk[3].y = stCrx->y - 1;
            stBlk[3].color = stCrx->color;
			break;
		case 1:
            stBlk[0].x = stCrx->x;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x;
            stBlk[2].y = stCrx->y + 1;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x + 1;
            stBlk[3].y = stCrx->y + 2;
            stBlk[3].color = stCrx->color;
			break;
		case 2:
            stBlk[0].x = stCrx->x - 2;
            stBlk[0].y = stCrx->y + 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x - 1;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x + 1;
            stBlk[3].y = stCrx->y;
            stBlk[3].color = stCrx->color;
			break;
		case 3:
            stBlk[0].x = stCrx->x - 1;
            stBlk[0].y = stCrx->y - 2;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x;
            stBlk[1].y = stCrx->y - 1;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
			break;
        default:
            return -2;
		}
		break;
    case BLK_010:
        switch (blkState)
        {
		case 0:
            stBlk[0].x = stCrx->x - 2;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x - 1;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x + 1;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
			break;
		case 1:
            stBlk[0].x = stCrx->x + 1;
            stBlk[0].y = stCrx->y - 2;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x;
            stBlk[1].y = stCrx->y - 1;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x - 1;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
			break;
        default:
            return -2;
		}
		break;
    case BLK_011:
        switch (blkState)
        {
		case 0:
            stBlk[0].x = stCrx->x + 2;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x + 1;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x - 1;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
			break;
		case 1:
            stBlk[0].x = stCrx->x - 1;
            stBlk[0].y = stCrx->y - 2;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x;
            stBlk[1].y = stCrx->y - 1;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x + 1;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
			break;
        default:
            return -2;
		}
		break;
    case BLK_012:
        switch (blkState)
        {
        case 0:
            stBlk[0].x = stCrx->x - 1;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x - 1;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x + 1;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        case 1:
            stBlk[0].x = stCrx->x;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x + 1;
            stBlk[1].y = stCrx->y - 1;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x - 1;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        case 2:
            stBlk[0].x = stCrx->x - 1;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x + 1;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x + 1;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        case 3:
            stBlk[0].x = stCrx->x + 1;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x - 1;
            stBlk[2].y = stCrx->y + 1;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        default:
            return -2;
        }
        break;
    case BLK_013:
        switch (blkState)
        {
        case 0:
            stBlk[0].x = stCrx->x + 1;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x + 1;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x - 1;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        case 1:
            stBlk[0].x = stCrx->x - 1;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x;
            stBlk[2].y = stCrx->y + 1;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x + 1;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        case 2:
            stBlk[0].x = stCrx->x + 1;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x - 1;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x - 1;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        case 3:
            stBlk[0].x = stCrx->x - 1;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x;
            stBlk[1].y = stCrx->y - 1;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x + 1;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        default:
            return -2;
        }
        break;
    case BLK_014:
        switch (blkState)
        {
        case 0:
            stBlk[0].x = stCrx->x - 2;
            stBlk[0].y = stCrx->y;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x - 1;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x;
            stBlk[2].y = stCrx->y + 1;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x + 1;
            stBlk[3].y = stCrx->y;
            stBlk[3].color = stCrx->color;
            break;
        case 1:
            stBlk[0].x = stCrx->x;
            stBlk[0].y = stCrx->y - 2;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x;
            stBlk[1].y = stCrx->y - 1;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x - 1;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        case 2:
            stBlk[0].x = stCrx->x - 1;
            stBlk[0].y = stCrx->y;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x;
            stBlk[1].y = stCrx->y - 1;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x + 1;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x + 2;
            stBlk[3].y = stCrx->y;
            stBlk[3].color = stCrx->color;
            break;
        case 3:
            stBlk[0].x = stCrx->x;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x + 1;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x;
            stBlk[2].y = stCrx->y + 1;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x;
            stBlk[3].y = stCrx->y + 2;
            stBlk[3].color = stCrx->color;
            break;
        default:
            return -2;
        }
        break;
    case BLK_015:
        switch (blkState)
        {
        case 0:
            stBlk[0].x = stCrx->x - 1;
            stBlk[0].y = stCrx->y;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x;
            stBlk[1].y = stCrx->y + 1;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x + 1;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x + 2;
            stBlk[3].y = stCrx->y;
            stBlk[3].color = stCrx->color;
            break;
        case 1:
            stBlk[0].x = stCrx->x;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x - 1;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x;
            stBlk[2].y = stCrx->y + 1;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x;
            stBlk[3].y = stCrx->y + 2;
            stBlk[3].color = stCrx->color;
            break;
        case 2:
            stBlk[0].x = stCrx->x - 2;
            stBlk[0].y = stCrx->y;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x - 1;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x;
            stBlk[2].y = stCrx->y - 1;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x + 1;
            stBlk[3].y = stCrx->y;
            stBlk[3].color = stCrx->color;
            break;
        case 3:
            stBlk[0].x = stCrx->x;
            stBlk[0].y = stCrx->y - 2;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x;
            stBlk[1].y = stCrx->y - 1;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x + 1;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        default:
            return -2;
        }
        break;
    case BLK_016:
        switch (blkState)
        {
        case 0:
            stBlk[0].x = stCrx->x - 1;
            stBlk[0].y = stCrx->y;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x + 1;
            stBlk[2].y = stCrx->y + 1;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x + 2;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        case 1:
            stBlk[0].x = stCrx->x;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x - 1;
            stBlk[2].y = stCrx->y + 1;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x - 1;
            stBlk[3].y = stCrx->y + 2;
            stBlk[3].color = stCrx->color;
            break;
        default:
            return -2;
        }
        break;
    case BLK_017:
        switch (blkState)
        {
        case 0:
            stBlk[0].x = stCrx->x;
            stBlk[0].y = stCrx->y;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x + 1;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x - 2;
            stBlk[2].y = stCrx->y + 1;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x - 1;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        case 1:
            stBlk[0].x = stCrx->x;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x + 1;
            stBlk[2].y = stCrx->y + 1;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x + 1;
            stBlk[3].y = stCrx->y + 2;
            stBlk[3].color = stCrx->color;
            break;
        default:
            return -2;
        }
        break;
    case BLK_018:
        switch (blkState)
        {
        case 0:
            stBlk[0].x = stCrx->x - 2;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x - 1;
            stBlk[1].y = stCrx->y - 1;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x + 1;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        case 1:
            stBlk[0].x = stCrx->x + 1;
            stBlk[0].y = stCrx->y - 2;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x + 1;
            stBlk[1].y = stCrx->y - 1;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x - 1;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        case 2:
            stBlk[0].x = stCrx->x - 1;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x + 1;
            stBlk[2].y = stCrx->y + 1;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x + 2;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        case 3:
            stBlk[0].x = stCrx->x + 1;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x - 1;
            stBlk[2].y = stCrx->y + 1;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x - 1;
            stBlk[3].y = stCrx->y + 2;
            stBlk[3].color = stCrx->color;
            break;
        default:
            return -2;
        }
        break;
    case BLK_019:
        switch (blkState)
        {
        case 0:
            stBlk[0].x = stCrx->x + 1;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x + 2;
            stBlk[1].y = stCrx->y - 1;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x - 1;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        case 1:
            stBlk[0].x = stCrx->x - 1;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x + 1;
            stBlk[2].y = stCrx->y + 1;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x + 1;
            stBlk[3].y = stCrx->y + 2;
            stBlk[3].color = stCrx->color;
            break;
        case 2:
            stBlk[0].x = stCrx->x + 1;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x - 1;
            stBlk[2].y = stCrx->y + 1;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x - 2;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        case 3:
            stBlk[0].x = stCrx->x - 1;
            stBlk[0].y = stCrx->y - 2;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x - 1;
            stBlk[1].y = stCrx->y - 1;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x + 1;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        default:
            return -2;
        }
        break;
    case BLK_020:
        switch (blkState)
        {
        case 0:
            stBlk[0].x = stCrx->x - 1;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x;
            stBlk[1].y = stCrx->y - 1;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x + 1;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        case 1:
            stBlk[0].x = stCrx->x + 1;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x - 1;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x + 1;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        case 2:
            stBlk[0].x = stCrx->x;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x - 1;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x;
            stBlk[2].y = stCrx->y + 1;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x + 1;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        case 3:
            stBlk[0].x = stCrx->x;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x - 1;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x + 1;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x - 1;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        default:
            return -2;
        }
        break;
    case BLK_021:
        switch (blkState)
        {
        case 0:
            stBlk[0].x = stCrx->x;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x + 1;
            stBlk[1].y = stCrx->y - 1;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x - 1;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        case 1:
            stBlk[0].x = stCrx->x;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x - 1;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x + 1;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x + 1;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        case 2:
            stBlk[0].x = stCrx->x;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x + 1;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x - 1;
            stBlk[2].y = stCrx->y + 1;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        case 3:
            stBlk[0].x = stCrx->x - 1;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x - 1;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x + 1;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        default:
            return -2;
        }
        break;
    case BLK_022:
        switch (blkState)
        {
        case 0:
            stBlk[0].x = stCrx->x - 1;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x;
            stBlk[1].y = stCrx->y - 1;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x - 1;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x + 1;
            stBlk[3].y = stCrx->y;
            stBlk[3].color = stCrx->color;
            break;
        case 1:
            stBlk[0].x = stCrx->x;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x + 1;
            stBlk[1].y = stCrx->y - 1;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x + 1;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        case 2:
            stBlk[0].x = stCrx->x - 1;
            stBlk[0].y = stCrx->y;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x + 1;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x;
            stBlk[2].y = stCrx->y + 1;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x + 1;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        case 3:
            stBlk[0].x = stCrx->x;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x - 1;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x - 1;
            stBlk[2].y = stCrx->y + 1;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        default:
            return -2;
        }
        break;
    case BLK_023:
        switch (blkState)
        {
        case 0:
            stBlk[0].x = stCrx->x;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x + 1;
            stBlk[1].y = stCrx->y - 1;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x - 1;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x + 1;
            stBlk[3].y = stCrx->y;
            stBlk[3].color = stCrx->color;
            break;
        case 1:
            stBlk[0].x = stCrx->x;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x + 1;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x;
            stBlk[2].y = stCrx->y + 1;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x + 1;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        case 2:
            stBlk[0].x = stCrx->x - 1;
            stBlk[0].y = stCrx->y;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x + 1;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x - 1;
            stBlk[2].y = stCrx->y + 1;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        case 3:
            stBlk[0].x = stCrx->x - 1;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x;
            stBlk[1].y = stCrx->y - 1;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x - 1;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        default:
            return -2;
        }
        break;
    case BLK_024:
        switch (blkState)
        {
        case 0:
            stBlk[0].x = stCrx->x;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x - 1;
            stBlk[2].y = stCrx->y + 1;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x + 1;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        case 1:
            stBlk[0].x = stCrx->x - 1;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x + 1;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x - 1;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        case 2:
            stBlk[0].x = stCrx->x - 1;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x + 1;
            stBlk[1].y = stCrx->y - 1;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        case 3:
            stBlk[0].x = stCrx->x + 1;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x - 1;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x + 1;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        default:
            return -2;
        }
        break;
    case BLK_025:
        switch (blkState)
        {
        case 0:
            stBlk[0].x = stCrx->x - 1;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x;
            stBlk[1].y = stCrx->y - 1;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x + 1;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x + 1;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        case 1:
            stBlk[0].x = stCrx->x + 1;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x + 1;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x - 1;
            stBlk[2].y = stCrx->y + 1;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        case 2:
            stBlk[0].x = stCrx->x - 1;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x - 1;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x;
            stBlk[2].y = stCrx->y + 1;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x + 1;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        case 3:
            stBlk[0].x = stCrx->x;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x + 1;
            stBlk[1].y = stCrx->y - 1;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x - 1;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x - 1;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        default:
            return -2;
        }
        break;
    case BLK_026:
        switch (blkState)
        {
        case 0:
            stBlk[0].x = stCrx->x;
            stBlk[0].y = stCrx->y;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x + 1;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x - 1;
            stBlk[2].y = stCrx->y + 1;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x + 2;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        case 1:
            stBlk[0].x = stCrx->x;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x + 1;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x + 1;
            stBlk[2].y = stCrx->y + 1;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x;
            stBlk[3].y = stCrx->y + 2;
            stBlk[3].color = stCrx->color;
            break;
        case 2:
            stBlk[0].x = stCrx->x - 1;
            stBlk[0].y = stCrx->y;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x + 2;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x;
            stBlk[2].y = stCrx->y + 1;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x + 1;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        case 3:
            stBlk[0].x = stCrx->x + 1;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x;
            stBlk[2].y = stCrx->y + 1;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x + 1;
            stBlk[3].y = stCrx->y + 2;
            stBlk[3].color = stCrx->color;
            break;
        default:
            return -2;
        }
        break;
    case BLK_027:
        switch (blkState)
        {
        case 0:
            stBlk[0].x = stCrx->x + 1;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x + 2;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x - 1;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        case 1:
            stBlk[0].x = stCrx->x - 1;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x + 1;
            stBlk[2].y = stCrx->y + 1;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x;
            stBlk[3].y = stCrx->y + 2;
            stBlk[3].color = stCrx->color;
            break;
        case 2:
            stBlk[0].x = stCrx->x + 1;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x - 2;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x - 1;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        case 3:
            stBlk[0].x = stCrx->x;
            stBlk[0].y = stCrx->y - 2;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x - 1;
            stBlk[1].y = stCrx->y - 1;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x + 1;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        default:
            return -2;
        }
        break;
    case BLK_028:
        switch (blkState)
        {
        case 0:
            stBlk[0].x = stCrx->x - 1;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x - 2;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x + 1;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        case 1:
            stBlk[0].x = stCrx->x;
            stBlk[0].y = stCrx->y - 2;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x + 1;
            stBlk[1].y = stCrx->y - 1;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x - 1;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        case 2:
            stBlk[0].x = stCrx->x - 1;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x + 2;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x + 1;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        case 3:
            stBlk[0].x = stCrx->x + 1;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x - 1;
            stBlk[2].y = stCrx->y + 1;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x;
            stBlk[3].y = stCrx->y + 2;
            stBlk[3].color = stCrx->color;
            break;
        default:
            return -2;
        }
        break;
    case BLK_029:
        switch (blkState)
        {
        case 0:
            stBlk[0].x = stCrx->x;
            stBlk[0].y = stCrx->y;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x + 2;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x - 1;
            stBlk[2].y = stCrx->y + 1;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x + 1;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        case 1:
            stBlk[0].x = stCrx->x - 1;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x - 1;
            stBlk[2].y = stCrx->y + 1;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x;
            stBlk[3].y = stCrx->y + 2;
            stBlk[3].color = stCrx->color;
            break;
        default:
            return -2;
        }
        break;
    case BLK_030:
        switch (blkState)
        {
        case 0:
            stBlk[0].x = stCrx->x - 2;
            stBlk[0].y = stCrx->y;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x - 1;
            stBlk[2].y = stCrx->y + 1;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x + 1;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        case 1:
            stBlk[0].x = stCrx->x + 1;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x + 1;
            stBlk[2].y = stCrx->y + 1;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x;
            stBlk[3].y = stCrx->y + 2;
            stBlk[3].color = stCrx->color;
            break;
        default:
            return -2;
        }
        break;
    case BLK_031:
        switch (blkState)
        {
        case 0:
            stBlk[0].x = stCrx->x - 1;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x + 1;
            stBlk[1].y = stCrx->y - 1;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x + 1;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        case 1:
            stBlk[0].x = stCrx->x + 1;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x - 1;
            stBlk[2].y = stCrx->y + 1;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x + 1;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        case 2:
            stBlk[0].x = stCrx->x - 1;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x - 1;
            stBlk[2].y = stCrx->y + 1;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x + 1;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        case 3:
            stBlk[0].x = stCrx->x - 1;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x + 1;
            stBlk[1].y = stCrx->y - 1;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x - 1;
            stBlk[2].y = stCrx->y + 1;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x;
            stBlk[3].y = stCrx->y;
            stBlk[3].color = stCrx->color;
            break;
        default:
            return -2;
        }
        break;
    case BLK_032:
        switch (blkState)
        {
        case 0:
            stBlk[0].x = stCrx->x - 2;
            stBlk[0].y = stCrx->y - 2;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x - 1;
            stBlk[1].y = stCrx->y - 1;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x + 1;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        case 1:
            stBlk[0].x = stCrx->x + 2;
            stBlk[0].y = stCrx->y - 2;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x + 1;
            stBlk[1].y = stCrx->y - 1;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x - 1;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
            break;
        default:
            return -2;
        }
        break;
    case BLK_033:
        switch (blkState)
        {
		case 0:
            stBlk[0].x = stCrx->x;
            stBlk[0].y = stCrx->y - 1;
            stBlk[0].color = stCrx->color;
            stBlk[1].x = stCrx->x - 1;
            stBlk[1].y = stCrx->y;
            stBlk[1].color = stCrx->color;
            stBlk[2].x = stCrx->x + 1;
            stBlk[2].y = stCrx->y;
            stBlk[2].color = stCrx->color;
            stBlk[3].x = stCrx->x;
            stBlk[3].y = stCrx->y + 1;
            stBlk[3].color = stCrx->color;
			break;
        default:
            return -2;
		}
		break;
    default:
        return -1;
    }
    return 0;
}

int TfrmMain::rotatePiece(struct StBlock stBlk[4], int *nextBlkState)
{
    *nextBlkState = nextBlockState(blockType, blockState);
    if (*nextBlkState < 0)
        return -1;

    if (fillBlock(stBlk, &stCrux, blockType, *nextBlkState) != 0)
        return -2;

    return 0;
}

int TfrmMain::getNewPiece(struct StBlock stBlk[4], struct StBlock *stHold, int blkType, int x, int y)
{
    stHold->x = x;
    stHold->y = y;
    switch (blkType)
    {
	case BLK_000:
		break;
    case BLK_001:
        stHold->color = RGB(0, 0, 255);
        break;
    case BLK_002:
        stHold->color = RGB(255, 0, 0);
        break;
    case BLK_003:
        stHold->color = RGB(169, 0, 169);
        break;
    case BLK_004:
        stHold->color = RGB(0, 169, 169);
        break;
    case BLK_005:
        stHold->color = RGB(0, 255, 0);
        break;
    case BLK_006:
        stHold->color = RGB(255, 255, 0);
        break;
    case BLK_007:
        stHold->color = RGB(255, 0, 255);
        break;
    default:
        stHold->color = RGB(128, 128, 128);
        break;
    }

    fillBlock(stBlk, stHold, blkType, 0);
    return 0;
}

int TfrmMain::updateNewPiece(int blkType, int x, int y)
{
    struct StBlock stTemp[4];
    struct StBlock stHold;

    stCrux.x = x;
    stCrux.y = y;

    blockType = blkType;
    blockState = 0;

    getNewPiece(&stTemp[0], &stHold, blkType, x, y);
    copyBlock(&stCrux, &stHold);
    copyBlocks(&stPiece[0], &stTemp[0]);

    updatePiece();

    return 0;
}

int TfrmMain::readOptionsToIni()
{
    FILE *fp=(FILE *)0;
    char bufLine[256];
    int nLen;
    char *flds[20];
    int nflds;
    int iTmp;

    fp = fopen(FNAME_INI, "r");
    if (fp == (FILE *)0)
        goto abort;

    for ( ; ; )
    {
        nLen = readlineDos(fp, bufLine, sizeof(bufLine));
        if (nLen < 0)
            break;
        if (nLen == 0)
            continue;
        nflds = splitline(bufLine, flds, '=', 20);
        if (nflds != 2)
            continue;

        if      (strcmp(flds[0], "Board Column") == 0)
        {
            iTmp = atoi(flds[1]);
            if (iTmp < MIN_COL)
                iTmp = MIN_COL;
            frmMain->board_col = iTmp;
        }
        else if (strcmp(flds[0], "Board Row") == 0)
        {
            iTmp = atoi(flds[1]);
            if (iTmp < MIN_ROW)
                iTmp = MIN_ROW;
            frmMain->board_row = iTmp;
        }
        else if (strcmp(flds[0], "Start Level") == 0)
        {
            iTmp = atoi(flds[1]);
            if ( (iTmp <= 0) || (iTmp > 10) )
                iTmp = 1;
            frmMain->mLevel = iTmp;
        }
        else if (strcmp(flds[0], "Has Extra Block") == 0)
        {
            iTmp = atoi(flds[1]);
            if (iTmp == 0)
                frmMain->mfExtraBlock = false;
            else
                frmMain->mfExtraBlock = true;
        }
        else if (strcmp(flds[0], "Has Background") == 0)
        {
            iTmp = atoi(flds[1]);
            if (iTmp == 0)
                frmMain->mfDrawBack = false;
            else
                frmMain->mfDrawBack = true;
        }
        else if (strcmp(flds[0], "Game Speed") == 0)
        {
            iTmp = atoi(flds[1]);
            if ( (iTmp < 100) || (iTmp > 5000) )
                iTmp = DEFT_TIMER_INTERVAL;
            frmMain->timer_interval = iTmp;
        }
    }

    fclose(fp);
    return 0;
abort:
    if (fp != (FILE *)0)
        fclose(fp);
    return -1;
}

int TfrmMain::saveOptionsToIni()
{
    FILE *fp=(FILE *)0;
    char bufLine[256];

    fp = fopen(FNAME_INI, "w");
    if (fp == (FILE *)0)
        goto abort;

    sprintf(bufLine, "Board Column=%d", frmMain->board_col);
    fprintf(fp, "%s\n", bufLine);
    sprintf(bufLine, "Board Row=%d", frmMain->board_row);
    fprintf(fp, "%s\n", bufLine);
    sprintf(bufLine, "Start Level=%d", frmMain->mLevel);
    fprintf(fp, "%s\n", bufLine);
    sprintf(bufLine, "Has Extra Block=%d", (frmMain->mfExtraBlock) ? 1 : 0);
    fprintf(fp, "%s\n", bufLine);
    sprintf(bufLine, "Has Background=%d", (frmMain->mfDrawBack) ? 1 : 0);
    fprintf(fp, "%s\n", bufLine);
    sprintf(bufLine, "Game Speed=%d", frmMain->timer_interval);
    fprintf(fp, "%s\n", bufLine);

    fclose(fp);
    return 0;
abort:
    if (fp != (FILE *)0)
        fclose(fp);
    return -1;
}

//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormCloseQuery(TObject *Sender,
    bool &CanClose)
{
    if (gameState != EM_GS_NO_GAME)
    {
        if (Application->MessageBox("game in progress, do you really want to quit?",
            "", MB_YESNO + MB_ICONQUESTION + MB_DEFBUTTON2) == ID_NO)
        {
            CanClose = false;
            return;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::imgBlk000Click(TObject *Sender)
{
    updateBack();
    fclear = true;
    gameState = EM_GS_NO_GAME;
}
//---------------------------------------------------------------------------
void TfrmMain::fxnBlock(int blkType, int blkState)
{
    struct StBlock stTemp[4];

    if (fclear == true)
    {
        fclear = false;
        stCrux.x = 3;
        stCrux.y = 3;
        stCrux.color = RGB(255, 0, 0);
        blockType = blkType;
        blockState = blkState;
        fillBlock(stTemp, &stCrux, blockType, blockState);
        copyBlocks(&stPiece[0], &stTemp[0]);
        updatePiece();
        gameState = EM_GS_PLAYING;
        tmrBeat->Enabled = true;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::imgBlk001Click(TObject *Sender)
{
    fxnBlock(BLK_001, 0);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::imgBlk002Click(TObject *Sender)
{
    fxnBlock(BLK_002, 0);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::imgBlk003Click(TObject *Sender)
{
    fxnBlock(BLK_003, 0);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::imgBlk004Click(TObject *Sender)
{
    fxnBlock(BLK_004, 0);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::imgBlk005Click(TObject *Sender)
{
    fxnBlock(BLK_005, 0);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::imgBlk006Click(TObject *Sender)
{
    fxnBlock(BLK_006, 0);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::imgBlk007Click(TObject *Sender)
{
    fxnBlock(BLK_007, 0);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::Label4Click(TObject *Sender)
{
    int i, j;
    AnsiString anstr;

	Memo1->Lines->Clear();

    for (j=0; j<board_row; j++)
    {
        anstr = "";
        for (i=0; i<board_col; i++)
        {
            switch (board_cell_occup[j][i])
            {
            case EM_BF_NONE:
                anstr = anstr + "0" + " ";
                break;
            case EM_BF_MOVE:
                anstr = anstr + "1" + " ";
                break;
            case EM_BF_FIXED:
                anstr = anstr + "2" + " ";
                break;
            default:
                anstr = anstr + "X" + " ";
                break;
            }
        }
       	Memo1->Lines->Add(anstr);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::Label5Click(TObject *Sender)
{
	if (gameState == EM_GS_NO_GAME)
        mnuNewClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::Edit1KeyDown(TObject *Sender,
    WORD &Key, TShiftState Shift)
{
    int iBlk;
    if ( (Key == '\n') || (Key == '\r') )
    {
    iBlk = atoi(Edit1->Text.c_str());
    if ( (iBlk >= 0) && (iBlk <= 33) )
    {
        fxnBlock(iBlk, 0);
        tmrBeat->Enabled = false;
    }
    }

}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::mnuNewClick(TObject *Sender)
{
    int i;
    int r;

    initBoard();
    drawBoxFill(imgBoard, 0, 0, imgBoard->ClientWidth, imgBoard->ClientHeight, clWhite);
    updateBack();
    updateBoard();

    mScore = 0;
    mLevel = mLevelStart;

    updateScore();
    updateLevel();

    lq.clear();
    for (i=0; i<3; i++)
    {
        r = genRandPiece_A();
        lq.push(r);
    }

    r = lq.pop();
    updateNewPiece(r, block_start_x, block_start_y);
    updateNextPiece();

    redrawBoardAndPiece();

    // last function to be called
    gameStart();

}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::mnuOptionsClick(TObject *Sender)
{
    fOptionsChanged = false;


    frmOption->sbrCol->Min = MIN_COL;
    frmOption->sbrCol->Max = MAX_COL;
    frmOption->sbrRow->Min = MIN_ROW;
    frmOption->sbrRow->Max = MAX_ROW;

    frmOption->sbrCol->Position = frmMain->board_col;
    frmOption->lblCol->Caption = "Column " + AnsiString(frmMain->board_col);

    frmOption->sbrRow->Position = frmMain->board_row;
    frmOption->lblRow->Caption = "Row " + AnsiString(frmMain->board_row);

    frmOption->cboStartLevel->ItemIndex = frmMain->mLevel - 1;

    frmOption->chkHasExtraBlock->Checked = frmMain->mfExtraBlock;
    frmOption->chkShowBack->Checked = frmMain->mfDrawBack;

    frmOption->lblGameSpeed->Caption = "Game Speed (ms) " + AnsiString(frmMain->timer_interval);
    frmOption->sbrGameSpeed->Position = frmMain->timer_interval;


    frmOption->ShowModal();
    if (fOptionsChanged == true)
    {
        updateLevel();
        initBoard();
        drawBoxFill(imgBoard, 0, 0, imgBoard->ClientWidth, imgBoard->ClientHeight, clWhite);
        updateBack();
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::mnuScoreClick(TObject *Sender)
{
    struct StScore tblScore[MAX_REC_SCORE];
    int i;
    char bufWork[128];
    char bufTemp[64];
    time_t t;

    readScoreFromFile(&tblScore[0]);

    frmScoreDisp->Memo1->Lines->Clear();
    sprintf(bufWork, " No     Score             Time                      Name                ");
    frmScoreDisp->Memo1->Lines->Add(AnsiString(bufWork));
    sprintf(bufWork, " ==  ============  ===================  ================================");
    frmScoreDisp->Memo1->Lines->Add(AnsiString(bufWork));

    for (i=1; i<MAX_REC_SCORE; i++)
    {
        if (tblScore[i].time == 0)
        {
            sprintf(bufWork, " %2d  %12d  %-19.19s  %-32.32s", i, 0, "", "");
        }
        else
        {
            sprintf(bufWork, " %2d  %12d  ", i, tblScore[i].score);
            t = tblScore[i].time;
            strftime(bufTemp, sizeof(bufTemp), "%Y/%m/%d %H:%M:%S", localtime(&t));
            strcat(bufWork, bufTemp);
            strcat(bufWork, "  ");
            sprintf(bufTemp, "%-32.32s", tblScore[i].name);
            strcat(bufWork, bufTemp);
        }

        frmScoreDisp->Memo1->Lines->Add(AnsiString(bufWork));
    }

    frmScoreDisp->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::mnuRefreshClick(TObject *Sender)
{
    redrawBoardAndPiece();    
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::mnuTerminateClick(TObject *Sender)
{
    if (gameState == EM_GS_PLAYING)
        gameFinish();
}
//---------------------------------------------------------------------------
