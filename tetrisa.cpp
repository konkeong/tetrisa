//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------
USEFORM("fmMain.cpp", frmMain);
USERES("tetrisa.res");
USEFORM("fmAbout.cpp", frmAbout);
USEFORM("fmDocu.cpp", frmDocu);
USEUNIT("mlogo.cpp");
USEFILE("mlogo.h");
USEFILE("mtetrisa.h");
USEUNIT("mloopq.cpp");
USEFILE("mloopq.h");
USEFORM("fmOption.cpp", frmOption);
USEFORM("fmScoreA.cpp", frmScoreAsk);
USEFORM("fmScoreD.cpp", frmScoreDisp);
USEUNIT("mscore.c");
USEFILE("mscore.h");
USEUNIT("Kutil.c");
USEFILE("Kutil.h");
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	try
	{
		Application->Initialize();
		Application->Title = "Tetrisa";
		Application->CreateForm(__classid(TfrmMain), &frmMain);
        Application->CreateForm(__classid(TfrmAbout), &frmAbout);
        Application->CreateForm(__classid(TfrmDocu), &frmDocu);
        Application->CreateForm(__classid(TfrmOption), &frmOption);
        Application->CreateForm(__classid(TfrmScoreAsk), &frmScoreAsk);
        Application->CreateForm(__classid(TfrmScoreDisp), &frmScoreDisp);
        Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	return 0;
}
//---------------------------------------------------------------------------
