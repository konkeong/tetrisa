//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#pragma hdrstop

#include "fmDocu.h"
//---------------------------------------------------------------------------
#pragma resource "*.dfm"
TfrmDocu *frmDocu;
//---------------------------------------------------------------------------
__fastcall TfrmDocu::TfrmDocu(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TfrmDocu::mmoDocuDblClick(TObject *Sender)
{
	frmDocu->Close();
}
//---------------------------------------------------------------------------
void __fastcall TfrmDocu::FormCreate(TObject *Sender)
{
	mmoDocu->Color = (TColor)cl3DLight;
	mmoDocu->Font->Name = "Courier New";
	mmoDocu->Font->Height = 14;
	mmoDocu->Font->Pitch = fpFixed;  /* fpDefault, fpVariable */
	mmoDocu->Lines->Clear();

	mmoDocu->Lines->Add("Documentation of TetrisA");
	mmoDocu->Lines->Add("========================");
	mmoDocu->Lines->Add("18 Apr 2003  Version 1.01.00");
	mmoDocu->Lines->Add("  added a guide-bar at the bottom of game board");
    mmoDocu->Lines->Add("");
	mmoDocu->Lines->Add("10 Sep 2001  Version 1.00.01");
	mmoDocu->Lines->Add("  fmScoreDisp change FontSize to 8");
	mmoDocu->Lines->Add("  can read/save option to ini file");
    mmoDocu->Lines->Add("  fix minor bugs");
    mmoDocu->Lines->Add("");
	mmoDocu->Lines->Add("08 Sep 2001  Version 1.00");
	mmoDocu->Lines->Add("  First version.");
	mmoDocu->Lines->Add("    Implementing game Tetris");
	mmoDocu->Lines->Add("    with the twist of extra pieces");
	mmoDocu->Lines->Add("  Still got some minor bugs, not going to");
	mmoDocu->Lines->Add("    fix them, maybe never ;->");
	mmoDocu->Lines->Add("");
	mmoDocu->Lines->Add("07 Sep 2001");
	mmoDocu->Lines->Add("  Very impressed with myself, the core coding");
	mmoDocu->Lines->Add("    took about 15 hours. Not including the R&D");
	mmoDocu->Lines->Add("    for TCanvas, mloopq, BitBlt, board about 10");
	mmoDocu->Lines->Add("    hour. This is done when I am clear-headed.");
	mmoDocu->Lines->Add("    Which I seldom is nowadays :-)");
	mmoDocu->Lines->Add("  Hope to finish the touch-up soon.");
	mmoDocu->Lines->Add("");
	mmoDocu->Lines->Add("06 Sep 2001");
	mmoDocu->Lines->Add("  Whole day coding...Backbone is okay.");
	mmoDocu->Lines->Add("  Adding in the extra pieces.");
	mmoDocu->Lines->Add("  Should cut-trim-optionize code for release");
	mmoDocu->Lines->Add("");
	mmoDocu->Lines->Add("12 Aug 2001");
	mmoDocu->Lines->Add("  Impulse to do the game stemmed from ");
	mmoDocu->Lines->Add("    high of Genting Drinking Trip.");
	mmoDocu->Lines->Add("  To have Tetris with source code so");
	mmoDocu->Lines->Add("    I can customize it.");
	mmoDocu->Lines->Add("  Tetris is originated by Alexei Pazhitnov");
	mmoDocu->Lines->Add("  (give credits where it is dued :-D )");
	mmoDocu->Lines->Add("");
	mmoDocu->Lines->Add("");
	mmoDocu->Lines->Add("");

	return;
}
//---------------------------------------------------------------------------
