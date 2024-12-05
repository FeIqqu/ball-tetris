//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TGameForm : public TForm
{
__published:	// IDE-managed Components
	TTimer *Game;
	TPaintBox *GameArea;
	TLabel *ScoreLabel;
	TPaintBox *NextBalls;
	void __fastcall GameTimer(TObject *Sender);
	void __fastcall GameAreaPaint(TObject *Sender);
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall NextBallsPaint(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TGameForm(TComponent* Owner);
	void ResetGame();
};
//---------------------------------------------------------------------------
extern PACKAGE TGameForm *GameForm;
//---------------------------------------------------------------------------
#endif
