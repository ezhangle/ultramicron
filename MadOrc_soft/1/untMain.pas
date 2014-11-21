unit untMain;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, iaRS232, StdCtrls, iaPostCMDTypes;

type
  TForm1 = class(TForm)
    Memo1: TMemo;
    Memo2: TMemo;
    Label1: TLabel;
    Label2: TLabel;
    Button1: TButton;
    Button2: TButton;
    Button3: TButton;
    Button4: TButton;
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure Button1Click(Sender: TObject);
    procedure Button2Click(Sender: TObject);
    procedure Button3Click(Sender: TObject);
    procedure Button4Click(Sender: TObject);
  private
    RS232: TiaRS232;
    fBuf: TiaBuf;
    fBufCount: Integer;
    procedure DoOnReceiveEvent(Sender: TObject; Const aData: TiaBuf;
      aCount: Cardinal);
    procedure DoOnSendedEvent(Sender: TObject; Const aData: TiaBuf;
      aCount: Cardinal);
  protected
    procedure CreateAnswer(aData: TiaBuf);
  public
    { Public declarations }
  end;

var
  Form1: TForm1;

implementation

{$R *.dfm}

procedure TForm1.FormCreate(Sender: TObject);
begin
  RS232 := TiaRS232.Create;
  RS232.OnRSReceived := DoOnReceiveEvent;
  RS232.OnRSSended := DoOnSendedEvent;
  SetLength(fBuf, 12);
  fBufCount := 0;
end;

procedure TForm1.FormDestroy(Sender: TObject);
begin
  FreeAndNil(RS232);
end;

procedure TForm1.Button1Click(Sender: TObject);
begin
  RS232.Properties.PortNum := 1;
  RS232.Open;
end;

procedure TForm1.Button2Click(Sender: TObject);
begin
  RS232.Close;
end;

procedure TForm1.Button3Click(Sender: TObject);
begin
  RS232.StartListner;
end;

procedure TForm1.Button4Click(Sender: TObject);
begin
  RS232.StopListner;
end;

procedure TForm1.CreateAnswer(aData: TiaBuf);
Var
  vAns: TiaBuf;
  vX: Byte;
  f: Integer;
begin
  SetLength(vAns, 12);
  If aData[0] = CMD_READ_FRAMECODE then
    begin
      vAns[0] := CMD_READ_FRAMECODE;
      vAns[1] := $01;
      vAns[10] := $0f;
    end;

  If aData[0] = CMD_READ_BASEDATETIME then
    begin
      vAns[0] := CMD_READ_BASEDATETIME;
      vAns[1] := $10;      // День
      vAns[2] := $02;      // Месяц
      vAns[3] := $06;      // Год от 2000
      vAns[4] := $0e;      // час
      vAns[5] := $07;      // минута

      vAns[6] := $00;      // Код рамки
      vAns[7] := $00;      //
      vAns[8] := $00;      //
      vAns[9] := $0f;      //

      vAns[10] := $05; //Количество записей
    end;

  If aData[0] = CMD_READ_RECORD then
    If aData[1] = 0 then
      begin
        vAns[0] := CMD_READ_RECORD;
        vAns[1] := $37;
        vAns[2] := $38;
        vAns[3] := $35;
        vAns[4] := $32;
        vAns[5] := $33;
        vAns[6] := $39;
        vAns[7] := $35;
        vAns[8] := $31;
        vAns[9] := $00;
        vAns[10] := $32;
      end
    else
      begin
        vAns[0] := CMD_READ_RECORD;
        vAns[1] := $31;
        vAns[2] := $32;
        vAns[3] := $33;
        vAns[4] := $34;
        vAns[5] := $35;
        vAns[6] := $36;
        vAns[7] := $37;
        vAns[8] := $38;
        vAns[9] := $00;
        vAns[10] := $32;
      end;

  If aData[0] = CMD_WRITE_BASEDATETIME then
    begin
      vAns[0] := aData[0];
      vAns[1] := aData[1];
      vAns[2] := aData[2];
      vAns[3] := aData[3];
      vAns[4] := aData[4];
      vAns[5] := aData[5];
      vAns[6] := aData[6];
      vAns[7] := aData[7];
      vAns[8] := aData[8];
      vAns[9] := aData[9];
      vAns[10] := aData[10];
      vAns[11] := aData[11];
    end;

  If aData[0] = CMD_TURNOFF_TRANSMITTER then
    begin
      vAns[0] := aData[0];
    end;

  vX := 0;
  For F := 0 to 10 do
    vX := vX xor vAns[F];

  vAns[11] := vX;

  RS232.Send(vAns);
end;

procedure TForm1.DoOnReceiveEvent(Sender: TObject; const aData: TiaBuf;
  aCount: Cardinal);
Var
  F: Integer;
  ss: String;
  vCount: Integer;
  vBufCount: Integer;
begin
  If fBufCount >= 12 then
    fBufCount := 0;

  vCount := Length(aData);
  vBufCount := fBufCount;

  For F := vBufCount to vCount + vBufCount - 1 do
    Begin
      if fBufCount >= 12 then
        Break;
      fBuf[F] := aData[F - vBufCount];
      Inc(fBufCount);
    end;

  if fBufCount >= 12 then
    begin
      ss := '';
      For F := 0 To fBufCount - 1 do
        ss := ss + IntToHex(fBuf[F],2) + ' ';

      Memo1.Lines.Add(ss);
      CreateAnswer(fBuf);
    end;

  If vBufCount + vCount > 12 then
    Begin
      fBufCount := 0;
      For F := 0 to vCount - (vBufCount - 12) - 1 do
        begin
          fBuf[F] := aData[F + (vBufCount - 12)];
          Inc(fBufCount);
        end;
    end;  
end;

procedure TForm1.DoOnSendedEvent(Sender: TObject; const aData: TiaBuf;
  aCount: Cardinal);
Var
  F: INteger;
  ss: String;
begin
  ss := '';
  For F := 0 To Length(aData) - 1 do
    ss := ss + IntToHex(aData[F],2) + ' ';

  Memo2.Lines.Add(ss);
end;

end.
