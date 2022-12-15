//---------------------------------------------------------------------------
#ifndef fmOptionH
#define fmOptionH
//---------------------------------------------------------------------------
#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
//---------------------------------------------------------------------------
class TfrmOption : public TForm
{
__published:	// IDE-managed Components
    TButton *cmdOk;
    TButton *cmdCancel;
    TScrollBar *sbrCol;
    TScrollBar *sbrRow;
    TLabel *lblBoardSize;
    TLabel *lblCol;
    TLabel *lblRow;
    TComboBox *cboStartLevel;
    TLabel *lblStartLevel;
    TCheckBox *chkHasExtraBlock;
    TScrollBar *sbrGameSpeed;
    TLabel *lblGameSpeed;
    TCheckBox *chkShowBack;
    void __fastcall cmdOkClick(TObject *Sender);
    void __fastcall cmdCancelClick(TObject *Sender);
    
    
    void __fastcall sbrColChange(TObject *Sender);
    void __fastcall sbrColScroll(TObject *Sender,
    TScrollCode ScrollCode, int &ScrollPos);
    void __fastcall sbrRowChange(TObject *Sender);
    void __fastcall sbrRowScroll(TObject *Sender,
    TScrollCode ScrollCode, int &ScrollPos);
    void __fastcall sbrGameSpeedChange(TObject *Sender);
    void __fastcall sbrGameSpeedScroll(TObject *Sender,
    TScrollCode ScrollCode, int &ScrollPos);
    
    
private:	// User declarations
public:		// User declarations
    __fastcall TfrmOption(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern TfrmOption *frmOption;
//---------------------------------------------------------------------------
#endif
