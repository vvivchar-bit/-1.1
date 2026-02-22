       #include <vcl.h>
#pragma hdrstop

#include "Unit1.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TForm1 *Form1;

__fastcall TForm1::TForm1(TComponent* Owner)
    : TForm(Owner)
{
}

void __fastcall TForm1::FormCreate(TObject *Sender)
{
    Caption = "Сервер (Indy)";
    Application->Title = "Сервер";

    edAddress->Text = "127.0.0.1";
    if (edPort->Text.IsEmpty()) edPort->Text = "9000";

    btnSwitch->Tag = 0;
    btnSwitch->Caption = "Start!";

    pTime->Caption = TimeToStr(Time());
    mLog->Clear();
    lbUsers->Clear();

    Timer1->Interval = 1000;
    Timer1->Enabled = true;

    Timer2->Interval = 1000;
    Timer2->Enabled = false;

    Time_Working = 0;

    IdTCPServer1->Active = false;
    IdTCPServer1->DefaultPort = StrToIntDef(edPort->Text, 9000);
}

void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
    pTime->Caption = TimeToStr(Time());
}

void __fastcall TForm1::Timer2Timer(TObject *Sender)
{
    Time_Working += 1.0 / 86400.0;
    Caption = "Сервер працює: " + TimeToStr(Time_Working) +
              " | користувачів: " + IntToStr(lbUsers->Items->Count);
}

// Розсилка списку користувачів всім клієнтам
void __fastcall TForm1::BroadcastUsers()
{
    UnicodeString payload = "#" + lbUsers->Items->Text; // список рядками

    TList *ctxs = IdTCPServer1->Contexts->LockList();
    try
    {
        for (int i = 0; i < ctxs->Count; i++)
        {
            TIdContext *ctx = (TIdContext*)ctxs->Items[i];
            ctx->Connection->IOHandler->WriteLn(payload);
        }
    }
    __finally
    {
        IdTCPServer1->Contexts->UnlockList();
    }
}

void __fastcall TForm1::btnSwitchClick(TObject *Sender)
{
    if (btnSwitch->Tag == 0)
    {
        IdTCPServer1->DefaultPort = StrToIntDef(edPort->Text, 9000);
        IdTCPServer1->Active = true;

        if (IdTCPServer1->Active)
        {
            mLog->Lines->Add("[" + TimeToStr(Time()) + "] Сервер створений!");
            edAddress->Enabled = false;
            edPort->Enabled = false;

            btnSwitch->Tag = 1;
            btnSwitch->Caption = "Shutdown!";

            Time_Working = 0;
            Timer2->Enabled = true;
        }
        else
        {
            mLog->Lines->Add("[" + TimeToStr(Time()) + "] Помилка запуску сервера!");
        }
    }
    else
    {
        IdTCPServer1->Active = false;

        mLog->Lines->Add("[" + TimeToStr(Time()) + "] Сервер зруйнований!");
        edAddress->Enabled = true;
        edPort->Enabled = true;

        btnSwitch->Tag = 0;
        btnSwitch->Caption = "Start!";

        Timer2->Enabled = false;
        lbUsers->Clear();
        Caption = "Сервер (Indy)";
    }
}

// === Indy події ===
// Протокол:
// 1) Реєстрація: "#nickname"
// 2) Повідомлення: "To#From~Msg" (To = All або конкретний нік)
// Сервер розсилає: "From~Msg"

void __fastcall TForm1::IdTCPServer1Connect(TIdContext *AContext)
{
    AContext->Data = new TNickHolder(); // збереження ніка

    TThread::Queue(nullptr, [this, AContext]()
    {
        mLog->Lines->Add("[" + TimeToStr(Time()) + "] Підключення: " + AContext->Binding->PeerIP);
    });
}

void __fastcall TForm1::IdTCPServer1Disconnect(TIdContext *AContext)
{
    TNickHolder *h = (TNickHolder*)AContext->Data;
    UnicodeString nick = h ? h->Nick : "";

    if (!nick.IsEmpty())
    {
        TThread::Queue(nullptr, [this, nick]()
        {
            int idx = lbUsers->Items->IndexOf(nick);
            if (idx >= 0) lbUsers->Items->Delete(idx);

            mLog->Lines->Add("[" + TimeToStr(Time()) + "] Вийшов: " + nick);
        });

        TThread::Queue(nullptr, [this]() { BroadcastUsers(); });
    }

    delete h;
    AContext->Data = nullptr;
}

void __fastcall TForm1::IdTCPServer1Execute(TIdContext *AContext)
{
    UnicodeString line = AContext->Connection->IOHandler->ReadLn();
    if (line.IsEmpty()) return;

    TNickHolder *h = (TNickHolder*)AContext->Data;

    // Реєстрація: #nickname
    if (line[1] == '#')
    {
        UnicodeString nick = line.SubString(2, line.Length());
        if (h) h->Nick = nick;

        TThread::Queue(nullptr, [this, nick]()
        {
            if (lbUsers->Items->IndexOf(nick) < 0)
                lbUsers->Items->Add(nick);

            mLog->Lines->Add("[" + TimeToStr(Time()) + "] Прийшов: " + nick);
        });

        TThread::Queue(nullptr, [this]() { BroadcastUsers(); });
        return;
    }

    // Повідомлення: To#From~Msg
    int pHash  = line.Pos("#");
    int pTilde = line.Pos("~");
    if (pHash == 0 || pTilde == 0) return;

    UnicodeString To   = line.SubString(1, pHash - 1);
    UnicodeString From = line.SubString(pHash + 1, pTilde - pHash - 1);
    UnicodeString Msg  = line.SubString(pTilde + 1, line.Length());

    TThread::Queue(nullptr, [this, From, Msg]()
    {
        mLog->Lines->Add("[" + TimeToStr(Time()) + "] <" + From + "> " + Msg);
    });

    if (To == "All")
    {
        TList *ctxs = IdTCPServer1->Contexts->LockList();
        try
        {
            for (int i = 0; i < ctxs->Count; i++)
            {
                TIdContext *ctx = (TIdContext*)ctxs->Items[i];
                ctx->Connection->IOHandler->WriteLn(From + "~" + Msg);
            }
        }
        __finally { IdTCPServer1->Contexts->UnlockList(); }
    }
    else
    {
        // приватно: знайти підключення з ніком To
        TList *ctxs = IdTCPServer1->Contexts->LockList();
        try
        {
            for (int i = 0; i < ctxs->Count; i++)
            {
                TIdContext *ctx = (TIdContext*)ctxs->Items[i];
                TNickHolder *hh = (TNickHolder*)ctx->Data;

                if (hh && hh->Nick == To)
                {
                    ctx->Connection->IOHandler->WriteLn(From + "~" + Msg);
                    break;
                }
            }
        }
        __finally { IdTCPServer1->Contexts->UnlockList(); }
    }
}
