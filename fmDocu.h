//---------------------------------------------------------------------------
#ifndef fmDocuH
#define fmDocuH
//---------------------------------------------------------------------------
#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
//---------------------------------------------------------------------------
class TfrmDocu : public TForm
{
__published:	// IDE-managed Components
	TMemo *mmoDocu;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall mmoDocuDblClick(TObject *Sender);
	
    
private:	// User declarations
public:		// User declarations
	__fastcall TfrmDocu(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern TfrmDocu *frmDocu;
//---------------------------------------------------------------------------
#endif
