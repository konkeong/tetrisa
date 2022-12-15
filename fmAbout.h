//---------------------------------------------------------------------------
#ifndef fmAboutH
#define fmAboutH
//---------------------------------------------------------------------------
#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmAbout : public TForm
{
__published:	// IDE-managed Components
	TButton *btnOK;
	TGroupBox *grpboxAbout;
	TImage *imgWulph;
	TLabel *lblTitle;
	TLabel *lblCopyright;
	TLabel *lblEmail;
	TLabel *lblDate;
    TLabel *lblOther;
	void __fastcall btnOKClick(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TfrmAbout(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern TfrmAbout *frmAbout;
//---------------------------------------------------------------------------
#endif
