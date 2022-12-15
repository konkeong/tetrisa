//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#pragma hdrstop

#include "fmScoreA.h"

#include "mscore.h"
#include "fmMain.h"

//---------------------------------------------------------------------------
#pragma resource "*.dfm"
TfrmScoreAsk *frmScoreAsk;
//---------------------------------------------------------------------------
__fastcall TfrmScoreAsk::TfrmScoreAsk(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TfrmScoreAsk::FormClose(TObject *Sender,
    TCloseAction &Action)
{
    struct StScore tblScore[MAX_REC_SCORE];
    struct StScore recScore;
    int nRet;

    nRet = readScoreFromFile(&tblScore[0]);

    recScore.score = frmMain->mScore;
    recScore.time = time(0);
    sprintf(recScore.name, "%-*.*s", sizeof(recScore.name), sizeof(recScore.name), edtName->Text.c_str());

    nRet = insertScore(&tblScore[0], &recScore);
    if (nRet != 0)
    {
        Application->MessageBox("Fail to record score (err01)!", Application->Title.c_str(), MB_OK + MB_ICONEXCLAMATION);
        return;
    }

    nRet = writeScoreToFile(&tblScore[0]);
    if (nRet != 0)
    {
        Application->MessageBox("Fail to record score (err02)!", Application->Title.c_str(), MB_OK + MB_ICONEXCLAMATION);
        return;
    }

}
//---------------------------------------------------------------------------
void __fastcall TfrmScoreAsk::cmdOkClick(TObject *Sender)
{
    frmScoreAsk->Close();    
}
//---------------------------------------------------------------------------
