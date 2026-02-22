object Form1: TForm1
  Left = 0
  Top = 0
  Caption = 'Form1'
  ClientHeight = 441
  ClientWidth = 624
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = 'Segoe UI'
  Font.Style = []
  OnCreate = FormCreate
  TextHeight = 15
  object edAddress: TEdit
    Left = 24
    Top = 8
    Width = 193
    Height = 23
    TabOrder = 0
    Text = 'edAddress'
  end
  object edPort: TEdit
    Left = 24
    Top = 48
    Width = 193
    Height = 23
    TabOrder = 1
    Text = '9000'
  end
  object btnSwitch: TButton
    Left = 264
    Top = 47
    Width = 161
    Height = 25
    Caption = 'btnSwitch'
    TabOrder = 2
    OnClick = btnSwitchClick
  end
  object mLog: TMemo
    Left = 24
    Top = 104
    Width = 401
    Height = 329
    Lines.Strings = (
      'mLog')
    TabOrder = 3
  end
  object lbUsers: TListBox
    Left = 440
    Top = 104
    Width = 176
    Height = 329
    ItemHeight = 15
    TabOrder = 4
  end
  object pTime: TPanel
    Left = 256
    Top = 8
    Width = 169
    Height = 33
    Caption = 'pTime'
    TabOrder = 5
  end
  object Timer1: TTimer
    OnTimer = Timer1Timer
    Left = 448
    Top = 8
  end
  object Timer2: TTimer
    OnTimer = Timer2Timer
    Left = 488
    Top = 8
  end
  object IdTCPServer1: TIdTCPServer
    Bindings = <>
    DefaultPort = 0
    OnConnect = IdTCPServer1Connect
    OnDisconnect = IdTCPServer1Disconnect
    OnExecute = IdTCPServer1Execute
    Left = 552
    Top = 8
  end
end
