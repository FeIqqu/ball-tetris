object MainForm: TMainForm
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'Ball Tetris'
  ClientHeight = 441
  ClientWidth = 624
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = 'Segoe UI'
  Font.Style = []
  TextHeight = 15
  object Label1: TLabel
    Left = 168
    Top = 8
    Width = 298
    Height = 96
    Caption = 'Ball Tetris'
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -72
    Font.Name = 'Segoe UI'
    Font.Style = []
    ParentFont = False
  end
  object NewGameButton: TButton
    Left = 248
    Top = 176
    Width = 145
    Height = 49
    Caption = #1053#1086#1074#1072#1103' '#1080#1075#1088#1072
    TabOrder = 0
    OnClick = NewGameButtonClick
  end
  object AboutButton: TButton
    Left = 248
    Top = 240
    Width = 145
    Height = 49
    Caption = #1054' '#1087#1088#1086#1075#1088#1072#1084#1084#1077
    TabOrder = 1
    OnClick = AboutButtonClick
  end
  object ExitButton: TButton
    Left = 248
    Top = 304
    Width = 145
    Height = 49
    Caption = #1042#1099#1093#1086#1076
    TabOrder = 2
    OnClick = ExitButtonClick
  end
end
