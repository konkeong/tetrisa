//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#pragma hdrstop

#include "fmOption.h"
#include "fmMain.h"
//---------------------------------------------------------------------------
#pragma resource "*.dfm"
TfrmOption *frmOption;
//---------------------------------------------------------------------------
__fastcall TfrmOption::TfrmOption(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TfrmOption::cmdOkClick(TObject *Sender)
{
    frmMain->board_col = sbrCol->Position;
    frmMain->board_row = sbrRow->Position;

    frmMain->mLevelStart = cboStartLevel->ItemIndex + 1;

    frmMain->mfExtraBlock = chkHasExtraBlock->Checked;
    frmMain->mfDrawBack = chkShowBack->Checked;

    frmMain->timer_interval = sbrGameSpeed->Position;

    frmMain->fOptionsChanged = true;
    frmOption->Close();
}
//---------------------------------------------------------------------------
void __fastcall TfrmOption::cmdCancelClick(TObject *Sender)
{
    frmMain->fOptionsChanged = false;
    frmOption->Close();
}
//---------------------------------------------------------------------------
void __fastcall TfrmOption::sbrColChange(TObject *Sender)
{
    lblCol->Caption = "Column " + AnsiString(sbrCol->Position);
}
//---------------------------------------------------------------------------
void __fastcall TfrmOption::sbrColScroll(TObject *Sender,
    TScrollCode ScrollCode, int &ScrollPos)
{
    lblCol->Caption = "Column " + AnsiString(sbrCol->Position);
}
//---------------------------------------------------------------------------
void __fastcall TfrmOption::sbrRowChange(TObject *Sender)
{
    lblRow->Caption = "Row " + AnsiString(sbrRow->Position);    
}
//---------------------------------------------------------------------------
void __fastcall TfrmOption::sbrRowScroll(TObject *Sender,
    TScrollCode ScrollCode, int &ScrollPos)
{
    lblRow->Caption = "Row " + AnsiString(sbrRow->Position);    
}
//---------------------------------------------------------------------------
void __fastcall TfrmOption::sbrGameSpeedChange(TObject *Sender)
{
    lblGameSpeed->Caption = "Game Speed (ms) " + AnsiString(sbrGameSpeed->Position);    
}
//---------------------------------------------------------------------------
void __fastcall TfrmOption::sbrGameSpeedScroll(TObject *Sender,
    TScrollCode ScrollCode, int &ScrollPos)
{
    lblGameSpeed->Caption = "Game Speed (ms) " + AnsiString(sbrGameSpeed->Position);    
}
//---------------------------------------------------------------------------
