object GameForm: TGameForm
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'Ball Tetris'
  ClientHeight = 569
  ClientWidth = 526
  Color = clBtnFace
  DoubleBuffered = True
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = 'Segoe UI'
  Font.Style = []
  OnClose = FormClose
  OnKeyDown = FormKeyDown
  TextHeight = 15
  object GameArea: TPaintBox
    Left = 120
    Top = 8
    Width = 400
    Height = 550
    Color = clRed
    ParentColor = False
    OnPaint = GameAreaPaint
  end
  object ScoreLabel: TLabel
    Left = 8
    Top = 546
    Width = 41
    Height = 15
    Caption = 'Score: 0'
  end
  object NextBalls: TPaintBox
    Left = 8
    Top = 8
    Width = 100
    Height = 89
    OnPaint = NextBallsPaint
  end
  object Game: TTimer
    Interval = 8
    OnTimer = GameTimer
    Left = 720
    Top = 568
  end
end
