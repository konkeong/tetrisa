//---------------------------------------------------------------------------
#ifndef fmScoreDH
#define fmScoreDH
//---------------------------------------------------------------------------
#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
//---------------------------------------------------------------------------
class TfrmScoreDisp : public TForm
{
__published:	// IDE-managed Components
    TButton *cmdOk;
    TMemo *Memo1;
    void __fastcall cmdOkClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TfrmScoreDisp(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern TfrmScoreDisp *frmScoreDisp;
//---------------------------------------------------------------------------
#endif
