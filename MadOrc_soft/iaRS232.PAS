{******************************************************************************}
{   Класс для передачи данных на внешние устройства по линии RS232(Com-порт)   }
{   и получения данных от внешних устройств.                                   }
{   Так же класс позволяет прослушивать заданный COM-порт. Слушается порт      }
{   в отдельном потоке с именем iaCOMListner!!!                                }
{   Поддерживает интерфейс IiaProvider для стандартизации приема/передачи      }
{                                                                              }
{                                                                              }
{   Автор: Избяков А.М.                                                        }
{   e-mail: mr_tigra@mail.ru                                                   }
{                                                                   19.09.2005 }
{******************************************************************************}

unit iaRS232;

interface

uses
  SysUtils, iaCOMProperties, Windows, SyncObjs, classes;

const
//Константы настроек порта по умолчанию
  DEF_PORTNUM = 1;
  DEF_BAUDRATE = CBR_19200;
  DEF_PARITY = NOPARITY;
  DEF_STOPBITS = ONESTOPBIT;
  DEF_BYTESIZE = DATABITS_8;
  DEF_RTS_CONTROL = RTS_CONTROL_DISABLE;
  DEF_DTR_CONTROL = DTR_CONTROL_DISABLE;
  DEF_READINTERVALTIMEOUT = 100;
  DEF_READTOTALTIMEOUTMULTIPLIER = 10;
  DEF_READTOTALTIMEOUTCONSTANT = 200;
  DEF_WRITETOTALTIMEOUTMULTIPLIER = 10;
  DEF_WRITETOTALTIMEOUTCONSTANT = 200;
  DEF_RTS = False;
  DEF_DTR = False;
  DEF_USE_SYNCHRONIZ_RECEIVE = True;

//Коды сообщений и ошибок
  //Работа с портом
  MSG_RS_PORTINACTIVE            = '-iaRS232- Порт не открыт';
  MSG_RS_PORTINACTIVE_UIN        = 101;
  MSG_RS_PORTACTIVE              = '-iaRS232- Порт открыт';
  MSG_RS_PORTACTIVE_UIN          = 102;
  MSG_RS_PORTALREDYOPEN          = '-iaRS232- Порт уже открыт';
  MSG_RS_PORTALREDYOPEN_UIN      = 103;
  MSG_RS_PORTALREDYCLOSE         = '-iaRS232- Порт уже закрыт';
  MSG_RS_PORTALREDYCLOSE_UIN     = 104;
  MSG_RS_PORTBUSY                = '-iaRS232- Идет пересылка';
  MSG_RS_PORTBUSY_UIN            = 105;
  MSG_RS_PORTBUSYOPEN            = '-iaRS232- Идет пересылка или порт открыт';
  MSG_RS_PORTBUSYOPEN_UIN        = 106;

  //Настройка и открытие порта
  ERR_RS_PORTOPENERROR           = '-iaRS232- Ошибка открытия порта';
  ERR_RS_PORTOPENERROR_UIN       = 200;
  ERR_RS_SETPORTPROPERROR        = '-iaRS232- Ошибка установки свойств порта';
  ERR_RS_SETPORTPROPERROR_UIN    = 201;

  //Сообщения о ходе передачи/приема
  ERR_RS_NOERROR                 = '-iaRS232- Нет ошибок';
  ERR_RS_NOERROR_UIN             = 300;
  ERR_RS_NOECHO                  = '-iaRS232- Нет эха переходника';
  ERR_RS_NOECHO_UIN              = 301;
  ERR_RS_WRONGECHO               = '-iaRS232- Неправильное эхо переходника';
  ERR_RS_WRONGECHO_UIN           = 302;
  ERR_RS_NOANSWER                = '-iaRS232- Нет ответа';
  ERR_RS_NOANSWER_UIN            = 303;
  ERR_RS_READANSWERERROR         = '-iaRS232- Ошибка чтения ответа';
  ERR_RS_READANSWERERROR_UIN     = 304;
  ERR_RS_WRITEDATAERROR          = '-iaRS232- Ошибка записи данных';
  ERR_RS_WRITEDATAERROR_UIN      = 305;

//Констнатны для установки флагов в TDCB
  dcb_Binary              = $00000001;
  dcb_ParityCheck         = $00000002;
  dcb_OutxCtsFlow         = $00000004;
  dcb_OutxDsrFlow         = $00000008;
  dcb_DtrControlMask      = $00000030;
  dcb_DtrControlDisable   = $00000000;
  dcb_DtrControlEnable    = $00000010;
  dcb_DtrControlHandshake = $00000020;
  dcb_DsrSensivity        = $00000040;
  dcb_TXContinueOnXoff    = $00000080;
  dcb_OutX                = $00000100;
  dcb_InX                 = $00000200;
  dcb_ErrorChar           = $00000400;
  dcb_NullStrip           = $00000800;
  dcb_RtsControlMask      = $00003000;
  dcb_RtsControlDisable   = $00000000;
  dcb_RtsControlEnable    = $00001000;
  dcb_RtsControlHandshake = $00002000;
  dcb_RtsControlToggle    = $00003000;
  dcb_AbortOnError        = $00004000;
  dcb_Reserveds           = $FFFF8000;

//Тип для данных
type
  TiaBuf = Array of byte;

//указатель на данные
type
  PiaBuf = ^TiaBuf;

//Тип для хранения последней ошибки
Type
  RiaLastError = packed record
    msg: string;
    uin: Cardinal;
  end;

//События
Type
  TiaOnRSSended = procedure(Sender: TObject; Const aData: TiaBuf; aCount:
      Cardinal) of object;
Type
  TiaOnRSReceived = procedure(Sender: TObject; Const aData: TiaBuf; aCount:
      Cardinal) of object;
Type
  TiaOnRSError = procedure(Sender: TObject; Const aData: TiaBuf; aCount:
      Cardinal; aMSG: string; aUIN: Cardinal; var aRaiseException: Boolean) of
      object;

//Тип исключения для класса
type
  EiaRS232_Error = class(Exception)
  private
    fErrorCodeRS232: Cardinal;
  public
    property ErrorCodeRS232: Cardinal read fErrorCodeRS232
      write fErrorCodeRS232;
  end;

  TThreadNameInfo = record
    // must be 0x1000
    FType: LongWord;
    {$WARNINGS OFF}
    // pointer to name (in user address space)
    FName: PChar;
    {$WARNINGS ON}
    // thread ID (-1 indicates caller thread)
    FThreadID: LongWord;
    // reserved for future use, must be zero
    FFlags: LongWord;
  end;

  TiaRS232 = class; //Forward

  // Именованный поток для прослушивания порта
  TiaComListner = class(TThread)
    private
      // Событие по которому тормазится поток отвечающий за прослушку порта
      fComListnerEvent: TSimpleEvent;
      FRSObject: TiaRs232;
      procedure SetName;
    protected
      procedure Execute; override;
    public
      constructor Create(aSuspended: Boolean); virtual;
      destructor Destroy; override;
      procedure Start;
      procedure Stop;
    published
      property RSObject: TiaRs232 read FRSObject write FRSObject;
  end;

//Интерфейс для посылки/приема команд  
  IiaRS232 = interface(IInterface)
    // Открыть порт, очистить буферы
    procedure Open(const aManageErrorEvent: Boolean = True; const
        aDoErrorEvent: Boolean = True);
    // Закрыть порт, прекратить прием/перадачу, остановить прослушку, очистить буферы
    // aManageErrorEvent - Если true, то в событии вызываемом процедурой можно
    //   управлять возбуждением исключения
    // aDoErrorEvent - Если true выполнять событие при ошибке
    procedure Close(const aManageErrorEvent: Boolean = True; const
        aDoErrorEvent: Boolean = True);
    function Purge(const aTX_Abort: Boolean = True; const aRX_Abort: Boolean =
        True; const aTX_Clear: Boolean = True; const aRX_Clear: Boolean = True):
        Boolean;
    procedure Receive(var aStr: String; aCount: Cardinal; var aReadedCount:
        Cardinal; const aDoSuccessEvent: Boolean = True; const aManageErrorEvent:
        Boolean = True; const aDoErrorEvent: Boolean = True); overload;
    // Получить aCount байт в aData буфер
    // aReadedCount - Количество фактически прочитанных байт
    // aDoSuccessEvent - Если true выполнять событи при успешном выполнении
    // aManageErrorEvent - Если true, то в событии вызываемом процедурой можно
    //   управлять возбуждением исключения
    // aDoErrorEvent - Если true выполнять событие при ошибке
    // Если aCount = 0, то читаем все, что есть в буфере на этот момент!
    procedure Receive(var aData: TiaBuf; aCount: Cardinal; var aReadedCount:
        Cardinal; const aDoSuccessEvent: Boolean = True; const aManageErrorEvent:
        Boolean = True; const aDoErrorEvent: Boolean = True); overload;
    procedure Send(aStr: String; const aDoSuccessEvent: Boolean = True; const
        aManageErrorEvent: Boolean = True; const aDoErrorEvent: Boolean = True);
        overload;
    // Отослать буфер aData
    procedure Send(aData: TiaBuf; const aDoSuccessEvent: Boolean = True; const
        aManageErrorEvent: Boolean = True; const aDoErrorEvent: Boolean = True);
        overload;
    // Настройки
    function GetCOMProp: TiaCOMProperties;
    function GetActive: Boolean;
    // Открыт порт или нет
    property Active: Boolean read GetActive;
    property Properties: TiaCOMProperties read GetCOMProp;
  end;

//Сокращенный интерфейс для посылки/приема команд
  IiaRS232_Compact = interface(IInterface)
    // Открыть порт, очистить буферы
    procedure Open(const aManageErrorEvent: Boolean = True; const
        aDoErrorEvent: Boolean = True);
    // Закрыть порт, прекратить прием/перадачу, остановить прослушку, очистить буферы
    // aManageErrorEvent - Если true, то в событии вызываемом процедурой можно
    //   управлять возбуждением исключения
    // aDoErrorEvent - Если true выполнять событие при ошибке
    procedure Close(const aManageErrorEvent: Boolean = True; const
        aDoErrorEvent: Boolean = True);
    function Purge(const aTX_Abort: Boolean = True; const aRX_Abort: Boolean =
        True; const aTX_Clear: Boolean = True; const aRX_Clear: Boolean = True):
        Boolean;
    procedure Receive(var aStr: String; aCount: Cardinal; var aReadedCount:
        Cardinal; const aDoSuccessEvent: Boolean = True; const aManageErrorEvent:
        Boolean = True; const aDoErrorEvent: Boolean = True); overload;
    // Получить aCount байт в aData буфер
    // aReadedCount - Количество фактически прочитанных байт
    // aDoSuccessEvent - Если true выполнять событи при успешном выполнении
    // aManageErrorEvent - Если true, то в событии вызываемом процедурой можно
    //   управлять возбуждением исключения
    // aDoErrorEvent - Если true выполнять событие при ошибке
    // Если aCount = 0, то читаем все, что есть в буфере на этот момент!
    procedure Receive(var aData: TiaBuf; aCount: Cardinal; var aReadedCount:
        Cardinal; const aDoSuccessEvent: Boolean = True; const aManageErrorEvent:
        Boolean = True; const aDoErrorEvent: Boolean = True); overload;
    procedure Send(aStr: String; const aDoSuccessEvent: Boolean = True; const
        aManageErrorEvent: Boolean = True; const aDoErrorEvent: Boolean = True);
        overload;
    // Отослать буфер aData
    procedure Send(aData: TiaBuf; const aDoSuccessEvent: Boolean = True; const
        aManageErrorEvent: Boolean = True; const aDoErrorEvent: Boolean = True);
        overload;
    // Настройки
//    function GetCOMProp: TiaCOMProperties;
    function GetActive: Boolean;
    // Открыт порт или нет
    property Active: Boolean read GetActive;
//    property Properties: TiaCOMProperties read GetCOMProp;
  end;

//Основной класс
  TiaRS232 = class(TInterfacedObject, IiaRS232, IiaRS232_Compact)
  private
    fCOMProp: TiaCOMProperties;
    fCSPortActive: TCriticalSection;
    fDCB: TDCB;
    FLastRSError: RiaLastError;
    FOnRSError: TiaOnRSError;
    FOnRSReceived: TiaOnRSReceived;
    FOnRSSended: TiaOnRSSended;
    fPortActive: Boolean;
    fComListner: TiaComListner;
    fUseSynchronizeReceive: Boolean;
    //Выполнение события. aRaiseException - возбуждать или нет исключение,
    //управляется внутри обработчика события
    procedure DoOnError(const aData: TiaBuf; const aCount: Cardinal;
      const aMSG: string; const aUIN: Cardinal; var aRaiseException: Boolean);
    procedure SetDCBStruct(const aCOMProp: TiaCOMProperties; const aManageErrorEvent: Boolean = True;
        const aDoErrorEvent: Boolean = True);
    procedure SetPortActive(Value: Boolean);
  protected
    fhPort: THandle;
    function GetCOMProp: TiaCOMProperties;
    function GetActive: Boolean;
    procedure DoOnReceived(const aData: TiaBuf; aCount: Cardinal);
    procedure DoOnSended(const aData: TiaBuf; aCount: Cardinal);
    procedure ListnerSynchroRead;
    //Обработка исключения,
    // aData - данные при которых произошла ошибка
    // aCount - поличество принфтых байт
    // aMSG - Сообщение об ошибке
    // aUIN - Код ошибки
    // aManageErrorEvent - Если true, то в событии вызываемом процедурой можно
    //   управлять возбуждением исключения, если false - то возбуждается исключение
    // aDoErrorEvent - Если true выполнять событие при ошибке
    // Result - false если не было исключения, иначе, неопределен!!!
    function RaiseEiaRS232Error(const aData: TiaBuf; const aCount: Cardinal; const
        aMsg: string; const aUIN: Cardinal; const aManageErrorEvent: Boolean =
        True; const aDoErrorEvent: Boolean = True): Boolean;
    procedure ToggleDTR;
    procedure ToggleRTS;
  public
    constructor Create; virtual;
    destructor Destroy; override;
    // Закрыть порт, прекратить прием/перадачу, остановить прослушку, очистить буферы
    // aManageErrorEvent - Если true, то в событии вызываемом процедурой можно
    //   управлять возбуждением исключения
    // aDoErrorEvent - Если true выполнять событие при ошибке
    procedure Close(const aManageErrorEvent: Boolean = True; const aDoErrorEvent:
        Boolean = True);
    // Открыть порт, очистить буферы
    procedure Open(const aManageErrorEvent: Boolean = True; const aDoErrorEvent:
        Boolean = True);
    function Purge(const aTX_Abort: Boolean = True; const aRX_Abort: Boolean =
        True; const aTX_Clear: Boolean = True; const aRX_Clear: Boolean = True):
        Boolean;
    // Получить aCount байт в aData буфер
    // aReadedCount - Количество фактически прочитанных байт
    // aDoSuccessEvent - Если true выполнять событи при успешном выполнении
    // aManageErrorEvent - Если true, то в событии вызываемом процедурой можно
    //   управлять возбуждением исключения
    // aDoErrorEvent - Если true выполнять событие при ошибке
    procedure Receive(var aData: TiaBuf; aCount: Cardinal; var aReadedCount:
        Cardinal; const aDoSuccessEvent: Boolean = True; const aManageErrorEvent:
        Boolean = True; const aDoErrorEvent: Boolean = True); overload; virtual;
    procedure Receive(var aStr: String; aCount: Cardinal; var aReadedCount:
        Cardinal; const aDoSuccessEvent: Boolean = True; const aManageErrorEvent:
        Boolean = True; const aDoErrorEvent: Boolean = True); overload; virtual;
    // Отослать буфер aData
    procedure Send(aData: TiaBuf; const aDoSuccessEvent: Boolean = True; const
        aManageErrorEvent: Boolean = True; const aDoErrorEvent: Boolean = True); overload; virtual;
    procedure Send(aStr: String; const aDoSuccessEvent: Boolean = True; const
        aManageErrorEvent: Boolean = True; const aDoErrorEvent: Boolean = True); overload; virtual;
    // Установить сигнал DTR в высокий
    procedure SetDTRHi(aDTR: Boolean);
    procedure SetRTSHi(aRTS: Boolean);
    // Запустить прослушку порта
    procedure StartListner;
    // Остановить прослушку порта
    procedure StopListner;

    // Последняя произошедшая ошибка
    property LastRSError: RiaLastError read FLastRSError;
    // Открыт порт или нет
    property Active: Boolean read GetActive;
    // Настройки
    property Properties: TiaCOMProperties read GetCOMProp;
    // События по приему/передаче
    property OnRSError: TiaOnRSError read FOnRSError write FOnRSError;
    property OnRSReceived: TiaOnRSReceived read FOnRSReceived write FOnRSReceived;
    property OnRSSended: TiaOnRSSended read FOnRSSended write FOnRSSended;
    //Если True, то событие из потока на OnReceive вызывается через Synchronize
    property UseSynchronizeReceive: Boolean read fUseSynchronizeReceive write fUseSynchronizeReceive;
  end;

implementation

constructor TiaRS232.Create;
begin
  inherited;
  fCOMProp := TiaCOMProperties.Create;
  fCSPortActive := TCriticalSection.Create;

  //Значения по умолчанию
  With fCOMProp do
    begin
      PortNum := DEF_PORTNUM;
      BaudRate :=  DEF_BAUDRATE;
      Parity := DEF_PARITY;
      StopBits := DEF_STOPBITS;
      ByteSize := DEF_BYTESIZE;
      RTSControl := DEF_RTS_CONTROL;
      DTRControl := DEF_DTR_CONTROL;
      ReadIntervalTimeout := DEF_READINTERVALTIMEOUT;
      ReadTotalTimeoutMultiplier := DEF_READTOTALTIMEOUTMULTIPLIER;
      ReadTotalTimeoutConstant := DEF_READTOTALTIMEOUTCONSTANT;
      WriteTotalTimeoutMultiplier := DEF_WRITETOTALTIMEOUTMULTIPLIER;
      WriteTotalTimeoutConstant := DEF_WRITETOTALTIMEOUTCONSTANT;
      RTS := DEF_RTS;
      DTR := DEF_DTR;
    end;
  fUseSynchronizeReceive := DEF_USE_SYNCHRONIZ_RECEIVE;

  //Создания потока для прослушки порта
  fComListner := TiaComListner.Create(True);
  fComListner.RSObject := Self;
  fComListner.Priority := tpLowest;
  fComListner.FreeOnTerminate := True;
  fComListner.Resume;
end;

destructor TiaRS232.Destroy;
begin
  fComListner.Terminate;
  StartListner;

  if Active then
    Close(false, false);

  FreeAndNil(fCSPortActive);
  FreeAndNil(fCOMProp);
  inherited;
end;

procedure TiaRS232.Close(const aManageErrorEvent: Boolean = True; const
    aDoErrorEvent: Boolean = True);
begin
  if not GetActive then
    if not RaiseEiaRS232Error(nil, 0, MSG_RS_PORTALREDYCLOSE,
        MSG_RS_PORTALREDYCLOSE_UIN, aManageErrorEvent, aDoErrorEvent) then
      Exit;

  StopListner;      

  PurgeComm(fhPort,
    PURGE_RXABORT or PURGE_RXCLEAR or PURGE_TXABORT or PURGE_TXCLEAR);
  CloseHandle(fhPort);
  FLastRSError.msg := ERR_RS_NOERROR;
  FLastRSError.uin := ERR_RS_NOERROR_UIN;
  SetPortActive(False);
end;

procedure TiaRS232.DoOnError(const aData: TiaBuf; const aCount: Cardinal;
  const aMSG: string; const aUIN: Cardinal; var aRaiseException: Boolean);
Var
  vData: TiaBuf;
begin
  vData := Copy(aData, 0, aCount);
  If Assigned(FOnRSError) then
    FOnRSError(Self, vData, aCount, aMSG, aUIN, aRaiseException);
end;

procedure TiaRS232.DoOnReceived(const aData: TiaBuf; aCount: Cardinal);
Var
  vData: TiaBuf;
begin
  FLastRSError.msg := ERR_RS_NOERROR;
  FLastRSError.uin := ERR_RS_NOERROR_UIN;
  vData := Copy(aData);
  If Assigned(FOnRSReceived) then
    FOnRSReceived(Self, vData, aCount);
end;

procedure TiaRS232.DoOnSended(const aData: TiaBuf; aCount: Cardinal);
Var
  vData: TiaBuf;
begin
  FLastRSError.msg := ERR_RS_NOERROR;
  FLastRSError.uin := ERR_RS_NOERROR_UIN;
  vData := Copy(aData);
  If Assigned(FOnRSSended) then
    FOnRSSended(Self, vData, aCount);
end;

function TiaRS232.GetCOMProp: TiaCOMProperties;
begin
  Result := nil;
  If fPortActive then
    if not RaiseEiaRS232Error(nil, 0, MSG_RS_PORTACTIVE, MSG_RS_PORTACTIVE_UIN,
        False, False) then
      Exit;
  Result := fCOMProp;
end;

function TiaRS232.GetActive: Boolean;
begin
  fCSPortActive.Enter;
  Result := fPortActive;
  fCSPortActive.Leave;
end;

procedure TiaRS232.Open(const aManageErrorEvent: Boolean = True; const
    aDoErrorEvent: Boolean = True);
begin
  If GetActive then
    if not RaiseEiaRS232Error(nil, 0, MSG_RS_PORTALREDYOPEN,
        MSG_RS_PORTALREDYOPEN_UIN, aManageErrorEvent, aDoErrorEvent) then
      Exit;

{$WARNINGS OFF}
  fhPort := CreateFile(PChar('\\.\COM'+IntToStr(fCOMProp.PortNum)),
    GENERIC_READ or GENERIC_WRITE,
    0,
    nil,
    OPEN_EXISTING,
    0,
    0);

  if fhPort = INVALID_HANDLE_VALUE then
    Begin
      if not RaiseEiaRS232Error(nil, 0, ERR_RS_PORTOPENERROR,
          ERR_RS_PORTOPENERROR_UIN, aManageErrorEvent, aDoErrorEvent) then
        Exit;
      end
  else
    begin
      SetDCBStruct(fCOMProp, aManageErrorEvent, aDoErrorEvent);
      if not SetCommState(fhPort, fDCB)
        or not SetCommTimeouts(fhPort, fComProp.Timeouts)
        or not SetCommMask(fhPort, EV_RXCHAR or EV_RXFLAG) then
          begin
            if fPortActive then
              Close;
            if not RaiseEiaRS232Error(nil, 0, ERR_RS_SETPORTPROPERROR,
                ERR_RS_SETPORTPROPERROR_UIN, aManageErrorEvent, aDoErrorEvent) then
              Exit;
          end
      else
        begin
          //очистить входной и выходной буферы, прекратить передачу/прием
          Purge;
          SetRTSHi(fCOMProp.RTS);
          SetDTRHi(fCOMProp.DTR);
          FLastRSError.msg := ERR_RS_NOERROR;
          FLastRSError.uin := ERR_RS_NOERROR_UIN;
          SetPortActive(True);
        end;
    end;
{$WARNINGS ON}
end;

function TiaRS232.Purge(const aTX_Abort: Boolean = True; const aRX_Abort:
    Boolean = True; const aTX_Clear: Boolean = True; const aRX_Clear: Boolean =
    True): Boolean;
Var
  vFlags: Cardinal;
begin
  vFlags := 0;
  If aTX_Abort then
    vFlags := vFlags or PURGE_TXABORT;
  If aRX_Abort then
    vFlags := vFlags or PURGE_RXABORT;
  If aTX_Clear then
    vFlags := vFlags or PURGE_TXCLEAR;
  If aRX_Clear then
    vFlags := vFlags or PURGE_RXCLEAR;

  Result := PurgeComm(fhPort, vFlags);
end;

function TiaRS232.RaiseEiaRS232Error(const aData: TiaBuf; const aCount:
    Cardinal; const aMsg: string; const aUIN: Cardinal; const
    aManageErrorEvent: Boolean = True; const aDoErrorEvent: Boolean = True):
    Boolean;
Var
  E: EiaRS232_Error;
  vRaiseException: Boolean;
begin
//  vRaiseException := True;
//  FLastRSError.msg := aMSG;
//  FLastRSError.uin := aUIN;
//  If aDoErrorEvent then
//    DoOnError(aData, aCount, aMsg, aUIN, vRaiseException);
//  If vRaiseException or not aManageErrorEvent then
//    Begin
//      E := EiaRS232_Error.Create(aMsg);
//      E.ErrorCodeRS232 := aUIN;
//      Raise E;
//    end
//  Else
    Result := False;
end;

procedure TiaRS232.Receive(var aData: TiaBuf; aCount: Cardinal; var
    aReadedCount: Cardinal; const aDoSuccessEvent: Boolean = True; const
    aManageErrorEvent: Boolean = True; const aDoErrorEvent: Boolean = True);
begin
  If Not fPortActive then
    if not RaiseEiaRS232Error(nil, 0, MSG_RS_PORTINACTIVE,
        MSG_RS_PORTINACTIVE_UIN, aManageErrorEvent, aDoErrorEvent) then
      Exit;

  SetLength(aData, aCount);

  ReadFile(fhPort, aData[0], aCount, aReadedCount, nil); //читаем ответ
  //Проверка на правильность принятия
  If aCount <> aReadedCount then
    If Not RaiseEiaRS232Error(aData, aReadedCount, ERR_RS_READANSWERERROR,
        ERR_RS_READANSWERERROR_UIN, aManageErrorEvent, aDoErrorEvent) then
      Exit;
  If aDoSuccessEvent then
    DoOnReceived(aData, aCount);
end;

procedure TiaRS232.Send(aData: TiaBuf; const aDoSuccessEvent: Boolean =
    True; const aManageErrorEvent: Boolean = True; const aDoErrorEvent: Boolean
    = True);
Var
  vOutBufLen: Cardinal;
  vWritedCount: Cardinal;
begin
  If Not fPortActive then
    If not RaiseEiaRS232Error(Nil, 0, MSG_RS_PORTINACTIVE,
        MSG_RS_PORTINACTIVE_UIN, aManageErrorEvent, aDoErrorEvent) then
      Exit;

  vOutBufLen := Length(aData);

  //Пишем данные
  WriteFile(fhPort, aData[0], vOutBufLen, vWritedCount, nil);
  //Проверка на правильность отправки
  If vOutBufLen <> vWritedCount then
    Begin
      If Not RaiseEiaRS232Error(aData, vWritedCount, ERR_RS_WRITEDATAERROR,
          ERR_RS_WRITEDATAERROR_UIN, aManageErrorEvent, aDoErrorEvent) then
        Exit;
    end;
  If aDoSuccessEvent then
    DoOnSended(aData, vOutBufLen);
end;

procedure TiaRS232.SetDCBStruct(const aCOMProp: TiaCOMProperties; const
    aManageErrorEvent: Boolean = True; const aDoErrorEvent: Boolean = True);
begin
  if not GetCommState(fhPort, fDCB) then
    if not RaiseEiaRS232Error(nil, 0, ERR_RS_SETPORTPROPERROR,
      ERR_RS_SETPORTPROPERROR_UIN, aManageErrorEvent, aDoErrorEvent) then
        Exit;

  fDCB.BaudRate := aCOMProp.BaudRate;
  fDCB.Parity := aCOMProp.Parity;
  fDCB.StopBits := aCOMProp.StopBits;
  fDCB.ByteSize := aCOMProp.ByteSize;

  fDCB.Flags := dcb_Binary;

  Case aCOMProp.DTRControl of
    DTR_CONTROL_DISABLE:    fdcb.Flags := fdcb.Flags or dcb_DtrControlDisable;
    DTR_CONTROL_ENABLE:     fdcb.Flags := fdcb.Flags or dcb_DtrControlEnable;
    DTR_CONTROL_HANDSHAKE:  fdcb.Flags := fdcb.Flags or dcb_DtrControlHandshake;
  end;

  Case aCOMProp.RTSControl of
    RTS_CONTROL_DISABLE:    fdcb.Flags := fdcb.Flags or dcb_RtsControlDisable;
    RTS_CONTROL_ENABLE:     fdcb.Flags := fdcb.Flags or dcb_RtsControlEnable;
    RTS_CONTROL_HANDSHAKE:  fdcb.Flags := fdcb.Flags or dcb_RtsControlHandshake;
    RTS_CONTROL_TOGGLE:     fdcb.Flags := fdcb.Flags or dcb_RtsControlToggle;
  end;
end;

procedure TiaRS232.SetDTRHi(aDTR: Boolean);
begin
  if aDTR then
    EscapeCommFunction(fhPort, SETDTR)
  else
    EscapeCommFunction(fhPort, CLRDTR);
  fCOMProp.DTR := aDTR;
end;

procedure TiaRS232.SetPortActive(Value: Boolean);
begin
  fCSPortActive.Enter;
  fPortActive := Value;
  fCSPortActive.Leave;
end;

procedure TiaRS232.SetRTSHi(aRTS: Boolean);
begin
  if aRTS then
    EscapeCommFunction(fhPort, SETRTS)
  else
    EscapeCommFunction(fhPort, CLRRTS);
  fCOMProp.RTS := aRTS;
end;

procedure TiaRS232.StartListner;
begin
  If (Not fPortActive) and (Not fComListner.Terminated) then
    If not RaiseEiaRS232Error(Nil, 0, MSG_RS_PORTINACTIVE,
        MSG_RS_PORTINACTIVE_UIN, False, False) then
      Exit;

  fComListner.Start;
end;

procedure TiaRS232.ToggleDTR;
begin
  SetDTRHi(not fCOMProp.DTR);
end;

procedure TiaRS232.ToggleRTS;
begin
  SetRTSHi(not fCOMProp.RTS);
end;

{ TiaComListner }

constructor TiaComListner.Create;
begin
  inherited Create(aSuspended);
  fComListnerEvent := TSimpleEvent.Create;
  fComListnerEvent.ResetEvent;
end;

destructor TiaComListner.Destroy;
begin
  FreeAndNil(fComListnerEvent);
  inherited;
end;

procedure TiaComListner.Execute;
begin
  SetName;
  while not Terminated do
    Begin
      try
        fComListnerEvent.WaitFor(INFINITE);
        If Terminated then
          Continue;
        if RSObject.UseSynchronizeReceive then
          Synchronize(RSObject.ListnerSynchroRead)
        else
          RSObject.ListnerSynchroRead;
        Sleep(25);
      except
        on E: Exception do
          ShowException(E, ExceptAddr);
      end;
    end;
end;

procedure TiaComListner.SetName;
var
  ThreadNameInfo: TThreadNameInfo;
begin
  {$WARNINGS OFF}
  ThreadNameInfo.FType := $1000;
  ThreadNameInfo.FName := 'iaCOMListner';
  ThreadNameInfo.FThreadID := $FFFFFFFF;
  ThreadNameInfo.FFlags := 0;
  try
    RaiseException( $406D1388, 0, sizeof(ThreadNameInfo) div sizeof(LongWord), @ThreadNameInfo );
  except
  end;
  {$WARNINGS ON}
end;

procedure TiaRS232.ListnerSynchroRead;
var
  vData: TiaBuf;
  vReadedCount: Cardinal;
  vComErrors: Cardinal;
  vCS: COMSTAT;
begin
  {$WARNINGS OFF}
  ClearCommError(fhPort, vComErrors, @vCS);
  {$WARNINGS ON}
    If vCS.cbInQue <> 0 then
      Receive(vData, vCS.cbInQue, vReadedCount);
end;

procedure TiaRS232.Send(aStr: String; const aDoSuccessEvent,
    aManageErrorEvent, aDoErrorEvent: Boolean);
Var
  Buf: TiaBuf;
begin
  SetLength(Buf, Length(aStr));
  {$WARNINGS OFF}
  CopyMemory(@Buf[0], @aStr[1], Length(aStr));
  {$WARNINGS ON}
  Send(Buf, aDoSuccessEvent, aManageErrorEvent);
end;

procedure TiaRS232.StopListner;
begin
  fComListner.Stop
end;

procedure TiaComListner.Start;
begin
  fComListnerEvent.SetEvent;
end;

procedure TiaComListner.Stop;
begin
  fComListnerEvent.ResetEvent;
end;

procedure TiaRS232.Receive(var aStr: String; aCount: Cardinal;
  var aReadedCount: Cardinal; const aDoSuccessEvent, aManageErrorEvent,
  aDoErrorEvent: Boolean);
Var
  vData: TiaBuf;   
begin
  Try
    Receive(vData, aCount, aReadedCount, aDoSuccessEvent, aManageErrorEvent, aDoErrorEvent);
  finally
    SetLength(aStr, aReadedCount);
    {$WARNINGS OFF}
    CopyMemory(@aStr[1], @vData[0], Length(aStr));
    {$WARNINGS ON}
  end;
end;

end.
