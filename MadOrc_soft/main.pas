unit main;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, JvTrayIcon, JvComponentBase, JvHidControllerClass, ImgList, Registry,
  Menus, StdCtrls, XPMan, ExtCtrls, jpeg, JvExControls, JvPoweredBy,
  shellapi, JvExExtCtrls, MMSystem, About_f, iaRS232;

  const
  REPORT_LEN = 255;
  GEIGER_VID = 5824;
  GEIGER_PID = 1503;

  GEIGER_PNAME = 'USB Geiger';
  GEIGER_VNAME = 'madorc@gmx.com'; //
  cmRxByte = WM_USER;

type
  TmainFrm = class(TForm)
    HidController: TJvHidDeviceController;
    MyTray: TJvTrayIcon;
    TrayMenu: TPopupMenu;
    AutoStartupBtn: TMenuItem;
    ExitBtn: TMenuItem;
    AboutBtn: TMenuItem;
    OKBtn: TButton;
    XPManifest1: TXPManifest;
    MessTmr: TTimer;
    TextColorBtn: TMenuItem;
    BlackBtn: TMenuItem;
    WhiteBtn: TMenuItem;
    GreenBtn: TMenuItem;
    BlueBtn: TMenuItem;
    FuchsiaBtn: TMenuItem;
    ImageList1: TImageList;
    Panel2: TPanel;
    Panel3: TPanel;
    Label15: TLabel;
    Label18: TLabel;
    Label19: TLabel;
    Image1: TImage;
    Label21: TLabel;
    Label1: TLabel;
    Label22: TLabel;
    Label23: TLabel;
    Label25: TLabel;
    Panel1: TPanel;
    Panel4: TPanel;
    Label2: TLabel;
    Label4: TLabel;
    Label5: TLabel;
    Button1: TButton;
    Label6: TLabel;
    RadioButton1: TRadioButton;
    RadioButton2: TRadioButton;
    N1: TMenuItem;
    AlarmEnableBtn: TMenuItem;
    N10001: TMenuItem;
    Label7: TLabel;
    RadioButton3: TRadioButton;
    RadioButton4: TRadioButton;
    Memo1: TMemo;
    Memo2: TMemo;
    Button2: TButton;
    Button3: TButton;
    Button4: TButton;
    Button5: TButton;
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure Button2Click(Sender: TObject);
    procedure Button3Click(Sender: TObject);
    procedure Button4Click(Sender: TObject);
    procedure Button5Click(Sender: TObject);
    function HidControllerEnumerate(HidDev: TJvHidDevice;
      const Idx: Integer): Boolean;
    procedure ExitBtnClick(Sender: TObject);
    procedure OKBtnClick(Sender: TObject);
    procedure AutoStartupBtnClick(Sender: TObject);
    procedure MessTmrTimer(Sender: TObject);
    procedure BlackBtnClick(Sender: TObject);
    procedure WhiteBtnClick(Sender: TObject);
    procedure GreenBtnClick(Sender: TObject);
    procedure BlueBtnClick(Sender: TObject);
    procedure FuchsiaBtnClick(Sender: TObject);
    procedure RedBtnClick(Sender: TObject);
    procedure YellowBtnClick(Sender: TObject);
    procedure SilverBtnClick(Sender: TObject);
    procedure AquaBtnClick(Sender: TObject);
    procedure OliveBtnClick(Sender: TObject);
    procedure MyTrayDoubleClick(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure MyTrayClick(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure Button1Click(Sender: TObject);
    procedure N1Click(Sender: TObject);
    procedure AboutBtnClick(Sender: TObject);
    procedure AlarmEnableBtnClick(Sender: TObject);
    procedure N10001Click(Sender: TObject);
     private
    { Private declarations }
    RS232: TiaRS232;
    fBuf: TiaBuf;
    fBufCount: Integer;
    procedure DoOnReceiveEvent(Sender: TObject; Const aData: TiaBuf;
      aCount: Cardinal);
    procedure DoOnSendedEvent(Sender: TObject; Const aData: TiaBuf;
      aCount: Cardinal);
 protected
//    procedure CreateAnswer(aData: TiaBuf);

  public
    procedure RefreshRAD;
    procedure SaveReg;
//    function GetDevInfo: boolean;
//    function RequestRAD: boolean;
    function CheckDev: boolean;
    procedure MakeIcon(f_fon: ulong);
end;

var
  mainFrm: TmainFrm;
  FeatureReportLen: integer = 0;
  USBReport: array[0..REPORT_LEN] of byte;
  WorkDevice: TJvHidDevice;
  DevPresent: boolean = false;
  pingback: integer;
  AutoStartup: boolean = true;
  AlarmEnable: boolean = true;
  alarmlevel: ulong = 40;
  needexit: boolean = false;
  tempcol: TColor = clLime;
  Fon_units: string;
  count_validate_percent: uint;
  IMPS: uint = 0;
  Fon: ulong = 0;
  count_validate: uchar = 0;
  count_interval: uchar = 0;
  blinker: boolean;
  impps: array[0..61] of uint;
  fonps: array[0..61] of ulong;
  fonpermin: array[0..61] of ulong;
  divgraphminute: ulong =1;
  fonperhour: array[0..25] of ulong;
  divgraphhour: ulong =1;
  fonpm: ulong;
  fonpmm: ulong;
  divgraphimp: uint = 3;
  dosefull: ulong;
  doseday: ulong;
  doseadd: ulong;
  i: uchar;
  temperature: uchar;
  tempsign: boolean;
  usbdatatype: uchar;
  divgraph: ulong =6;
  graph_x: uint;
  graph_y: uint;
  graph_y2: uint;
  g_color: uint;
  d_second: uchar = 0;
  d_minute: uchar = 0;
  d_hour: uchar = 0;
  d_day: ulong = 0;
  maxfon: ulong = 0;
implementation

{$R *.dfm}
{$R sounds.res}

procedure TmainFrm.SaveReg;
var
  reg: TRegistry;
begin
  reg := TRegistry.Create;                               // Открываем реестр
  reg.RootKey := HKEY_CURRENT_USER;                      // Для текущего пользователя
  reg.OpenKey('Software\USB_Geiger\USB_Geiger', true); // Открываем раздел
  reg.WriteBool('autorun', AutoStartup);
  reg.WriteBool('alarmenable', AlarmEnable);
  reg.WriteInteger('alarmlevel', AlarmLevel);
  reg.CloseKey;                                          // Закрываем раздел
  reg.OpenKey('Software\Microsoft\Windows\CurrentVersion\Run', true);  // Открываем раздел
  if AutoStartup then
    reg.WriteString('USB_Geiger', application.ExeName)       // Записываем
  else
    reg.DeleteValue('USB_Geiger');                          // или удаляем информацию
  reg.CloseKey;                                          // Закрываем раздел
  reg.Free;
end;




procedure TmainFrm.MakeIcon(f_fon: ulong);
var
 ii: uint;
begin

  if (not DevPresent) then
    begin
  ImageList1.GetIcon(0,MyTray.Icon);
  MyTray.Hint := 'Дозиметр не подключен';
    end
    else
    begin


  MyTray.Hint := 'USB Geiger: '+IntToStr(Fon)+' '+fon_units;

      if ((count_validate >0) and blinker) then
      begin
  ImageList1.GetIcon(1,MyTray.Icon);
  blinker := false;
      end
      else
      begin // cv
      blinker:=true;
  if (Fon < 41) and (Fon_units <> 'МР/Ч') then
      begin
      Panel1.Color := clGreen;
      Label7.Caption := '[ нормальный фон ]';
      ImageList1.GetIcon(2,MyTray.Icon);
      end
      else if (Fon < 121) and (Fon_units <> 'МР/Ч') then
      begin
      Panel1.Color := clOlive;
      Label7.Caption := '[ повышенный фон ]';
      ImageList1.GetIcon(3,MyTray.Icon);
      end
      else
      begin
      Panel1.Color := clMaroon;
      Label7.Caption := '[ ОПАСНЫЙ ФОН ]';
      ImageList1.GetIcon(4,MyTray.Icon);
      end;
      end; // cv
    end;
//
  Image1.Canvas.Brush.Color := clWhite;
  Image1.Canvas.Pen.Color := clWhite;
  Image1.Canvas.Rectangle(4, 0,305,100);
  Image1.Canvas.Pen.Color := clBlack;
  Image1.Canvas.Rectangle(2, 0,3,100);
  Image1.Canvas.Rectangle(0, 98,305,97);

  if RadioButton1.Checked then
begin
// граф ИМПУЛЬСОВ
  graph_x:=5;
  graph_y:=96;
    for ii := 1 to 60 do
    begin
    if (impps[ii]>5) then g_color:=clRED
    else if (impps[ii]>3) then g_color:=clYELLOW
    else g_color:=clGREEN;

    graph_y2:=graph_y-((impps[ii]*95) div divgraphimp)-1;
    Image1.Canvas.Pen.Color := g_color;
    Image1.Canvas.Brush.Color := g_color;
    Image1.Canvas.Rectangle(graph_x, graph_y, graph_x+4, graph_y2+1);


    Image1.Canvas.Pen.Color := clBlack;
    Image1.Canvas.Rectangle(graph_x-1, 98,graph_x,100);

    graph_x:=graph_x+5;
    end;

      Fon_units := 'Имп.';
    Label22.Caption := IntToStr(divgraphimp);
    Label23.Caption := fon_units;
// граф ИМПУЛЬСОВ
end
else if RadioButton2.Checked then
begin
  // граф фона сек
  graph_x:=5;
  graph_y:=96;
    for ii := 1 to 60 do
    begin
    if (fonps[ii]>120) then g_color:=clRED
    else if (fonps[ii]>40) then g_color:=clYELLOW
    else g_color:=clGREEN;

    graph_y2:=graph_y-((fonps[ii]*95) div divgraph)-1;
    Image1.Canvas.Pen.Color := g_color;
    Image1.Canvas.Brush.Color := g_color;
    Image1.Canvas.Rectangle(graph_x, graph_y, graph_x+4, graph_y2+1);


    Image1.Canvas.Pen.Color := clBlack;
    Image1.Canvas.Rectangle(graph_x-1, 98,graph_x,100);

    graph_x:=graph_x+5;
    end;

    if (divgraph > 1500) then
    begin
      Fon_units := 'МР/Ч';

      divgraph := divgraph div 1000;
    end
    else
    begin
      Fon_units := 'МкР/Ч';
    end;
    Label22.Caption := IntToStr(divgraph);
    Label23.Caption := fon_units;
// граф фона сек
end
else if RadioButton3.Checked then
begin
  // граф фона мин
  graph_x:=5;
  graph_y:=96;
    for ii := 1 to 60 do
    begin
    if (fonpermin[ii]>120) then g_color:=clRED
    else if (fonpermin[ii]>40) then g_color:=clYELLOW
    else g_color:=clGREEN;

    graph_y2:=graph_y-((fonpermin[ii]*95) div divgraphminute)-1;
    Image1.Canvas.Pen.Color := g_color;
    Image1.Canvas.Brush.Color := g_color;
    Image1.Canvas.Rectangle(graph_x, graph_y, graph_x+4, graph_y2+1);


    Image1.Canvas.Pen.Color := clBlack;
    Image1.Canvas.Rectangle(graph_x-1, 98,graph_x,100);

    graph_x:=graph_x+5;
    end;

    if (divgraphminute > 1500) then
    begin
      Fon_units := 'МР/Ч';

      divgraphminute := divgraphminute div 1000;
    end
    else
    begin
      Fon_units := 'МкР/Ч';
    end;
    Label22.Caption := IntToStr(divgraphminute);
    Label23.Caption := fon_units;
// граф фона мин
end
else if RadioButton4.Checked then
begin
  // граф фона час
  graph_x:=5;
  graph_y:=96;
    for ii := 1 to 60 do
    begin
    if (fonperhour[ii]>120) then g_color:=clRED
    else if (fonperhour[ii]>40) then g_color:=clYELLOW
    else g_color:=clGREEN;

    graph_y2:=graph_y-((fonperhour[ii]*95) div divgraphhour)-1;
    Image1.Canvas.Pen.Color := g_color;
    Image1.Canvas.Brush.Color := g_color;
    Image1.Canvas.Rectangle(graph_x, graph_y, graph_x+9, graph_y2+1);


    Image1.Canvas.Pen.Color := clBlack;
    Image1.Canvas.Rectangle(graph_x-1, 98,graph_x,100);

    graph_x:=graph_x+12;
    end;

    if (divgraphhour > 1500) then
    begin
      Fon_units := 'МР/Ч';

      divgraphhour := divgraphhour div 1000;
    end
    else
    begin
      Fon_units := 'МкР/Ч';
    end;
    Label22.Caption := IntToStr(divgraphhour);
    Label23.Caption := fon_units;
// граф фона час
end;

// пока уберём    Label3.Caption := '[Датчик] - Импульсов в секунду: '+IntToStr(IMPS);
    Label4.Caption := 'За '+IntToStr(d_day)+' дней '+IntToStr(d_hour)+' часов '+IntToStr(d_minute)+' минут,';

    if (d_minute > 0) then
    begin
      doseadd := (fonpmm div 60);
    end else doseadd := 0;

    if ((dosefull+doseday+doseadd) > 1500) then
    begin
    Label5.Caption := 'суммарная доза: '+IntToStr((dosefull+doseday+doseadd) div 1000)+' Миллирентген';
    end
    else
    begin
    Label5.Caption := 'суммарная доза: '+IntToStr(dosefull+doseday+doseadd)+' Микрорентген';
    end;

    if (maxfon > 1500) then
    begin
      maxfon := maxfon div 1000;
      Label6.Caption := 'Максимальный фон за это время: '+IntToStr(maxfon)+' МР/Ч';
    end
    else
    begin
      Label6.Caption := 'Максимальный фон за это время: '+IntToStr(maxfon)+' МкР/Ч';
    end;

    // Label3.Caption := 'T: '+IntToStr(temperature)+' C';

end;

procedure TmainFrm.RefreshRAD;
begin
    MakeIcon(Fon);
end;

procedure TmainFrm.FormCreate(Sender: TObject);
var
  reg: TRegistry;
begin

  RS232 := TiaRS232.Create;
  RS232.OnRSReceived := DoOnReceiveEvent;
  RS232.OnRSSended := DoOnSendedEvent;
  SetLength(fBuf, 8);
  fBufCount := 0;


  MyTray.IconIndex := -1;
  MessTmr.Enabled := true;
  reg := TRegistry.Create;                              // Открываем реестр
  reg.RootKey := HKEY_CURRENT_USER;
  if reg.OpenKey('Software\USB_Geiger\USB_Geiger', false) then
  begin
    try
      AutoStartup := reg.ReadBool('autorun');
    except
      AutoStartup := false;
    end;
    try
      AlarmEnable := reg.ReadBool('alarmenable');
    except
      AlarmEnable := false;
    end;
    try
      AlarmLevel := reg.ReadInteger('alarmlevel');
    except
      AlarmLevel := 40;
    end;
  end else begin    // если нет раздела -> прога еще не запускалась -> ставим на автозагрузку
    reg.OpenKey('Software\USB_Geiger\USB_Geiger', true);
    AutoStartup := true;
    try
      reg.WriteBool('autorun', AutoStartup);
    except
    end;
    try
      reg.WriteBool('alarmenable', AlarmEnable);
    except
    end;
    try
      reg.WriteInteger('alarmlevel', AlarmLevel);
    except
    end;
    reg.CloseKey;                                          // Закрываем раздел
    try
      reg.OpenKey('Software\Microsoft\Windows\CurrentVersion\Run', true); // Открываем раздел
      reg.WriteString('USB_Geiger', application.ExeName)       // Записываем
    except
    end;
  end;
  reg.CloseKey;                                        // Закрываем раздел
  reg.Free;
  AutoStartupBtn.Checked := AutoStartup;
  AlarmEnableBtn.Checked := AlarmEnable;
end;


function TmainFrm.CheckDev: boolean;
begin
   // DevPresent := true;
  pingback := random(251); //0..253
  fillchar(USBReport, REPORT_LEN, 0);
  USBReport[1] := 3; // код запроса пинга
  USBReport[2] := byte(pingback); // кидаем рандомный байт; ожидается,
  try                           // что девайс вернет pingback+1
    Result := WorkDevice.SetFeature(USBReport, FeatureReportLen);
  except
    Result := false;
  end;
end;

//function TmainFrm.RequestRAD: boolean;
//begin
//  fillchar(USBReport, REPORT_LEN, 0);
//  USBReport[1] := 89;
//  try
//    Result := WorkDevice.SetFeature(USBReport, FeatureReportLen);
//  except
//    Result := false;
//  end;
//end;

function TmainFrm.HidControllerEnumerate(HidDev: TJvHidDevice;
  const Idx: Integer): Boolean;
var
  VN, PN: string;
begin
  if needexit then
    application.Terminate;
  DevPresent :=false;
  Result := true;
VN := Trim(HidDev.VendorName); // на всяк случай
PN := Trim(HidDev.ProductName);
//  if ((PN = GEIGER_PNAME) and (VN = GEIGER_VNAME)) then
//  begin
      WorkDevice := HidDev;

    FeatureReportLen := WorkDevice.Caps.FeatureReportByteLength;
//    Result := GetDevInfo;
//    if Result then begin
//      RequestRAD;
//    end;
//    Result := false;
   DevPresent :=true;
//  end;
  if needexit then
    application.Terminate;
end;

procedure TmainFrm.ExitBtnClick(Sender: TObject);
begin
  //application.Terminate;
  needexit := true; // вежливо просим свалить из памяти
end;

procedure TmainFrm.OKBtnClick(Sender: TObject);
begin
  MyTray.HideApplication;
end;

procedure TmainFrm.AutoStartupBtnClick(Sender: TObject);
begin
  AutoStartup := AutoStartupBtn.Checked;
  SaveReg;
end;

procedure TmainFrm.MessTmrTimer(Sender: TObject);
var
iz: uchar;
begin
  RefreshRAD;
  HidController.Enumerate;
  // таймер
  fonpm := fonpm+fonps[d_second];
  d_second:=d_second+1;
  if(d_second = 60)then
  begin
  d_second:=0;
    fonpmm := fonpmm + (fonpm div 60);


    fonpermin[0] := fonpm div 60;
    divgraphminute := 1;
    for iz := 60 downto 1 do
    begin
    fonpermin[iz] := fonpermin[iz-1];
    if fonpermin[iz] > divgraphminute then divgraphminute := fonpermin[iz];
    end;
    fonpermin[0] := 0;


    fonpm := 0;
  d_minute:=d_minute+1;
      if (d_minute = 60) then
      begin

    fonperhour[0] := fonpmm div 60;
    divgraphhour := 1;
    for iz := 24 downto 1 do
    begin
    fonperhour[iz] := fonperhour[iz-1];
    if fonperhour[iz] > divgraphhour then divgraphhour := fonperhour[iz];
    end;
    fonperhour[0] := 0;

      d_minute:=0;
      doseday:= doseday + (fonpmm div 60);
      fonpmm := 0;
      d_hour:=d_hour+1;
          if (d_hour=24) then
          begin
          d_hour:=0;
          d_day:=d_day+1;
          dosefull := dosefull + doseday;
          doseday:=0;
          end;
      end;
  end;
  // таймер
    if needexit then
    application.Terminate;
end;

procedure TmainFrm.MyTrayClick(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
begin
if Button <> mbRight then
begin
  if (not DevPresent) then
      MyTray.BalloonHint('Ошибка:','Дозиметр не подключен',TBalloonType(3),5000,true)
    else
      MyTray.BalloonHint('Текущий фон:',IntToStr(Fon)+' '+fon_units,TBalloonType(2),5000,true);
end;
      AlarmEnableBtn.Caption := 'Тревога ['+IntToStr(alarmlevel)+'МкР/Ч]';
end;

procedure TmainFrm.N10001Click(Sender: TObject);
begin
   alarmlevel:=3000;
  SaveReg;
end;

procedure TmainFrm.MyTrayDoubleClick(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
begin
  MyTray.ShowApplication;
end;

procedure TmainFrm.N1Click(Sender: TObject);
begin
  MyTray.ShowApplication;
end;

procedure TmainFrm.AlarmEnableBtnClick(Sender: TObject);
begin
  AlarmEnable := AlarmEnableBtn.Checked;
  SaveReg;
end;

procedure TmainFrm.BlackBtnClick(Sender: TObject);
begin
  alarmlevel:=25;
  SaveReg;
end;

procedure TmainFrm.WhiteBtnClick(Sender: TObject);
begin
  alarmlevel:=40;
  SaveReg;
end;

procedure TmainFrm.GreenBtnClick(Sender: TObject);
begin
    alarmlevel:=120;
  SaveReg;
end;

procedure TmainFrm.BlueBtnClick(Sender: TObject);
begin
    alarmlevel:=400;
  SaveReg;
end;

procedure TmainFrm.Button1Click(Sender: TObject);
var im:uchar;
begin
dosefull:=0;
doseday:=0;
d_day:=0;
d_hour:=0;
d_minute:=0;
d_second:=0;
maxfon:=0;
fonpmm:=0;
for im := 0 to 61 do fonpermin[im] := 0;
for im := 0 to 25 do fonperhour[im] :=0;

end;



procedure TmainFrm.FuchsiaBtnClick(Sender: TObject);
begin
    alarmlevel:=1000;
  SaveReg;
end;

procedure TmainFrm.RedBtnClick(Sender: TObject);
begin
  tempcol := clRed;
  SaveReg;
end;

procedure TmainFrm.YellowBtnClick(Sender: TObject);
begin
  tempcol := clYellow;
  SaveReg;
end;

procedure TmainFrm.SilverBtnClick(Sender: TObject);
begin
  tempcol := clSilver;
  SaveReg;
end;

procedure TmainFrm.AboutBtnClick(Sender: TObject);
begin
try
About.ShowModal;
except
end;
end;

procedure TmainFrm.AquaBtnClick(Sender: TObject);
begin
  tempcol := clAqua;
  SaveReg;
end;

procedure TmainFrm.OliveBtnClick(Sender: TObject);
begin
  tempcol := clOlive;
  SaveReg;
end;



procedure TmainFrm.FormDestroy(Sender: TObject);
begin
  FreeAndNil(RS232);
end;

procedure TmainFrm.Button5Click(Sender: TObject);
begin
  RS232.Properties.PortNum := 3;
  RS232.Open;
end;

procedure TmainFrm.Button2Click(Sender: TObject);
begin
  RS232.Close;
end;

procedure TmainFrm.Button3Click(Sender: TObject);
begin
  RS232.StartListner;
end;

procedure TmainFrm.Button4Click(Sender: TObject);
begin
  RS232.StopListner;
end;

procedure TmainFrm.DoOnReceiveEvent(Sender: TObject; const aData: TiaBuf;
  aCount: Cardinal);
Var
  ia:uint;
  F: Integer;
  ss: String;
  vCount: Integer;
  vBufCount: Integer;
  Result: Boolean;
begin
  If fBufCount >= 8 then
    fBufCount := 0;

  vCount := Length(aData);
  vBufCount := fBufCount;

  For F := vBufCount to vCount + vBufCount - 1 do
    Begin
      if fBufCount >= 10 then
        Break;
      fBuf[F] := aData[F - vBufCount];
      Inc(fBufCount);
    end;

  if fBufCount >= 10 then
    begin
      ss := '';
      For F := 0 To fBufCount - 1 do
        ss := ss + IntToHex(fBuf[F],2) + ' ';

      Memo1.Lines.Add(ss);
      //-----------------------------------------------------------------------------------
begin
// DevPresent := true;
//  Result := CheckDev; // проверка наличия
//  if not Result then begin
//    DevPresent := false;
//    exit;
//  end;

  sleep(1);   // x3, mb это не надо (на всяк случай)
//  fillchar(USBReport, REPORT_LEN, 0); // принимаем данные
//  try
//    Result := WorkDevice.GetFeature(USBReport, FeatureReportLen);
//  except
//    Result := false;
//  end;

//  if Result then begin
if ((fBuf[0] = $d1) and (fBuf[9] = $d2)) then begin
  Fon :=0;
  IMPS :=0;
  count_validate := 0;
  count_interval := 0;
    IMPS := (fBuf[1] shl 8)+fBuf[2]; // собираем 2 чара
    Fon := ((fBuf[3] shl 16)+(fBuf[4] shl 8))+fBuf[5]; // собираем 3 чара
    count_validate := 30;
    count_interval := 30;

    // обработчик тревоги
    if (alarmenable and (fon > alarmlevel) and (d_minute mod 2 = 0) and (d_second mod 10 = 0)) then
    begin
    MyTray.BalloonHint('ТРЕВОГА!','Фон более '+IntToStr(alarmlevel)+'МкР/Ч',TBalloonType(3),5000,true);
    PlaySound('alarm', hInstance, SND_RESOURCE);
    end;

    // обработчик тревоги

    if maxfon < fon then maxfon:=fon;

    // вычисляем пределы импульсов
    impps[0] := IMPS;
    divgraphimp := 3;
    for ia := 60 downto 1 do //?
    begin
    impps[ia] := impps[ia-1];
    if impps[ia] > divgraphimp then divgraphimp := impps[ia];
    end;
    impps[0] := 0;

    // вычисляем пределы фона
    fonps[0] := fon;
    divgraph := 6;
    for ia := 61 downto 1 do
    begin
    fonps[ia] := fonps[ia-1];
    if fonps[ia] > divgraph then divgraph := fonps[ia];
    end;
    fonps[0] := 0;

    if (fon > 1500) then
    begin
      Fon_units := 'МР/Ч';

      fon := fon div 1000;
    end
    else
    begin
      Fon_units := 'МкР/Ч';
    end;

    Label15.Caption := IntToStr(fon);
    Label18.Caption := fon_units;

    if count_interval >0 then
    count_validate_percent := (100*(count_interval-count_validate)) div count_interval;

    Label19.Caption := 'Интервал счёта '+IntToStr(count_interval)+' секунд(ы) - '+IntToStr(count_validate_percent)+'%';

//    if (USBReport[3] <> (pingback + 1)) then
//    begin
//    Result := false;    // пинг
//    DevPresent := false;
//    end
//    else
    DevPresent := true;

  end;

//  sleep(1);   // x3, mb это не надо (на всяк случай)
end;
      //-----------------------------------------------------------------------------------
    end;

  If vBufCount + vCount > 10 then
    Begin
      fBufCount := 0;
      For F := 0 to vCount - (vBufCount - 10) - 1 do
        begin
          fBuf[F] := aData[F + (vBufCount - 10)];
          Inc(fBufCount);
        end;
    end;
end;

procedure TmainFrm.DoOnSendedEvent(Sender: TObject; const aData: TiaBuf;
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
