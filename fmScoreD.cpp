//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#pragma hdrstop

#include "fmScoreD.h"

#include "mscore.h"

//---------------------------------------------------------------------------
#pragma resource "*.dfm"
TfrmScoreDisp *frmScoreDisp;
//---------------------------------------------------------------------------
__fastcall TfrmScoreDisp::TfrmScoreDisp(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TfrmScoreDisp::cmdOkClick(TObject *Sender)
{
    frmScoreDisp->Close();    
}
//---------------------------------------------------------------------------
