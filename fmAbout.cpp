//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#pragma hdrstop

#include "fmAbout.h"
#include "mlogo.h"

//---------------------------------------------------------------------------
#pragma resource "*.dfm"
TfrmAbout *frmAbout;
//---------------------------------------------------------------------------
__fastcall TfrmAbout::TfrmAbout(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TfrmAbout::btnOKClick(TObject *Sender)
{
	frmAbout->Close();
}
//---------------------------------------------------------------------------
void __fastcall TfrmAbout::FormCreate(TObject *Sender)
{
	lblTitle->Caption = "TetrisA (Version 1.01.00)";
	lblCopyright->Caption = "Copyright (C) 2001-2003 WulphSOFT";
	lblDate->Caption = "Aug 2001 - Apr 2003";
	lblEmail->Caption = "wulph@hotmail.com";

	clearWindow(this->Canvas->Handle,
			0, 0,
			this->ClientWidth, this->ClientHeight,
			LTGRAY_BRUSH);
	drawLogoC(imgWulph->Canvas->Handle,
			5, 5,
			imgWulph->ClientWidth-5, imgWulph->ClientHeight-5);

	return;
}
//---------------------------------------------------------------------------
