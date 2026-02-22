      #ifndef Unit1H
#define Unit1H

#include <System.Classes.hpp>
#include <System.SysUtils.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>

#include <IdBaseComponent.hpp>
#include <IdComponent.hpp>
#include <IdContext.hpp>
#include <IdCustomTCPServer.hpp>
#include <IdTCPServer.hpp>

// «бер≥гаЇмо н≥к дл€ кожного п≥дключенн€ в AContext->Data (Data = TObject*)
class TNickHolder : public TObject
{
public:
    UnicodeString Nick;
};

class TForm1 : public TForm
{
__published:
    TEdit *edAddress;
    TEdit *edPort;
    TButton *btnSwitch;
    TMemo *mLog;
    TListBox *lbUsers;
    TPanel *pTime;
    TTimer *Timer1;
    TTimer *Timer2;

    TIdTCPServer *IdTCPServer1;

    void __fastcall FormCreate(TObject *Sender);
    void __fastcall btnSwitchClick(TObject *Sender);
    void __fastcall Timer1Timer(TObject *Sender);
    void __fastcall Timer2Timer(TObject *Sender);

    void __fastcall IdTCPServer1Connect(TIdContext *AContext);
    void __fastcall IdTCPServer1Disconnect(TIdContext *AContext);
    void __fastcall IdTCPServer1Execute(TIdContext *AContext);

public:
    __fastcall TForm1(TComponent* Owner);

private:
    TDateTime Time_Working;
    void __fastcall BroadcastUsers();
};

extern PACKAGE TForm1 *Form1;

#endif
