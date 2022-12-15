//---------------------------------------------------------------------------
#ifndef fmScoreAH
#define fmScoreAH
//---------------------------------------------------------------------------
#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
//---------------------------------------------------------------------------
class TfrmScoreAsk : public TForm
{
__published:	// IDE-managed Components
    TButton *cmdOk;
    TLabel *lblPrompt;
    TLabel *lblName;
    
    TEdit *edtName;
    void __fastcall FormClose(TObject *Sender,
    TCloseAction &Action);
    void __fastcall cmdOkClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TfrmScoreAsk(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern TfrmScoreAsk *frmScoreAsk;
//---------------------------------------------------------------------------
#endif
