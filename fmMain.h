//---------------------------------------------------------------------------
#ifndef fmMainH
#define fmMainH
//---------------------------------------------------------------------------
#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\Menus.hpp>
#include <vcl\ComCtrls.hpp>
#include <vcl\ExtCtrls.hpp>
//---------------------------------------------------------------------------

#define MAX_ROW 24
#define MAX_COL 15
#define MIN_ROW 10
#define MIN_COL 8

struct StBlock
{
	int x;
	int y;
	int color;
};

class TfrmMain : public TForm
{
__published:	// IDE-managed Components
	TMainMenu *mmnuMain;
	TMenuItem *mnuFile;
	TMenuItem *mnuExit;
	TMenuItem *mnuHelp;
	TMenuItem *mnuAbout;
	TMenuItem *mnuSepHelp01;
	TMenuItem *mnuDocu;
	TStatusBar *stabarStatus;
	TPanel *pnlSepMenu;
	TImage *imgBack;
	TImage *imgBoard;
	TImage *imgPiece;
	TLabel *Label1;
	TLabel *Label2;
	TTimer *tmrBeat;
    TLabel *Label3;
    TImage *imgBlk001;
    TImage *imgBlk002;
    TImage *imgBlk003;
    TImage *imgBlk004;
    TImage *imgBlk005;
    TImage *imgBlk006;
    TImage *imgBlk007;
    TImage *imgBlk000;
    TLabel *Label4;
    TMemo *Memo1;
    TLabel *Label5;
    TImage *imgNextPiece;
    TLabel *lblLevel;
    TLabel *lblScore;
    TEdit *Edit1;
    TMenuItem *mnuGame;
    TMenuItem *mnuNew;
    TMenuItem *mnuScore;
    TMenuItem *mnuSepGame01;
    TMenuItem *mnuOptions;
    TMenuItem *mnuRefresh;
    TMenuItem *mnuTerminate;
	void __fastcall mnuExitClick(TObject *Sender);
	void __fastcall mnuAboutClick(TObject *Sender);
	void __fastcall mnuDocuClick(TObject *Sender);
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
	TShiftState Shift);
	void __fastcall Label1Click(TObject *Sender);
	void __fastcall Label2Click(TObject *Sender);
	void __fastcall tmrBeatTimer(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall FormDestroy(TObject *Sender);
    void __fastcall FormCloseQuery(TObject *Sender,
    bool &CanClose);

    void __fastcall imgBlk000Click(TObject *Sender);
    void __fastcall imgBlk001Click(TObject *Sender);
    void __fastcall imgBlk002Click(TObject *Sender);
    void __fastcall imgBlk003Click(TObject *Sender);
    void __fastcall imgBlk004Click(TObject *Sender);
    void __fastcall imgBlk005Click(TObject *Sender);
    void __fastcall imgBlk006Click(TObject *Sender);
    void __fastcall imgBlk007Click(TObject *Sender);
    void __fastcall Label4Click(TObject *Sender);
    void __fastcall Label5Click(TObject *Sender);
    void __fastcall Edit1KeyDown(TObject *Sender, WORD &Key,
    TShiftState Shift);
    
    void __fastcall mnuNewClick(TObject *Sender);
    void __fastcall mnuOptionsClick(TObject *Sender);
    void __fastcall mnuScoreClick(TObject *Sender);
    void __fastcall mnuRefreshClick(TObject *Sender);
    void __fastcall mnuTerminateClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TfrmMain(TComponent* Owner);

	// data
	int mGotData;
	struct StBlock stKeep[4];
	struct StBlock stPiece[4];
	struct StBlock stCrux;
	int blockType;
	int blockState;
    int mDownUser;
    int mDownTimer;
    int mScore;
    int mLevel;
    int mLevelStart;
    bool mfIntelligentHelp;
    int mUseIntelligentHelp;
    bool mfExtraBlock;
    bool mfDrawBack;
    int timer_interval;

    char board_cell_occup[MAX_ROW][MAX_COL];
    long board_cell_color[MAX_ROW][MAX_COL];
    int board_row_filled[MAX_ROW];
    int board_row;
    int board_col;
    int board_orig_x;
    int board_orig_y;
    int board_side;
    int block_start_x;
    int block_start_y;
    int block_extra_max_row_spacing;
    long board_back_color;

    bool fOptionsChanged;

	// functions
    void __fastcall drawBoxFill(TImage *img, long X1, long Y1, long X2, long Y2, TColor clr);
    void __fastcall clearImage(TImage *img);

	int updatePiece();
	int movePiece(int move);

    int fillBlock(struct StBlock stBlk[4], struct StBlock *stCrx, int blkType, int blkState);
    void copyBlock(struct StBlock *stA, struct StBlock *stB);
    void copyBlocks(struct StBlock stA[4], struct StBlock stB[4]);
    void fxnBlock(int blkType, int blkState);
    int rotatePiece(struct StBlock stBlk[4], int *nextBlkState);
    int nextBlockState(int blkType, int blkState);
    int gameProcess(int move);
    int updateNewPiece(int blkType, int x, int y);
    int updateBoard();
    int canMoveDownOneStep();
    int canOccupyCell(struct StBlock stBlkNew[4], struct StBlock stBlkOld[4]);
    int getNewPiece(struct StBlock stBlk[4], struct StBlock *stHold, int blkType, int x, int y);
    int updateNextPiece();
    int gameFinish();
    int redrawBoardAndPiece();
    int gameStart();
    void updateScore();
    void updateLevel();
	int genRandPiece();
	int genRandPiece_A();
    int highestRow();
    int updateBack();
    int initBoard();
    int readOptionsToIni();
    int saveOptionsToIni();
};
//---------------------------------------------------------------------------
extern TfrmMain *frmMain;
//---------------------------------------------------------------------------
#endif
