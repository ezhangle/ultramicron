{******************************************************************************}
{   Класс хранения, загурзки и сохранения настроек COM-порта                   }
{   Используется классом для передачи данных TiaRS232                          }
{                                                                              }
{                                                                              }
{   Автор: Избяков А.М.                                                        }
{   e-mail: mr_tigra@mail.ru                                                   }
{                                                                   19.09.2005 }
{******************************************************************************}

unit iaCOMProperties;

interface

uses
  Windows, IniFiles, SysUtils, SyncObjs;

const
  DEF_CFG_FILE                = 'No File';

  ERR_LOADERROR               = 'Ошибка при загрузке настроек порта';
  ERR_LOADERROR_UIN           = 951;
  ERR_SAVEERROR               = 'Ошибка при записи настроек порта';
  ERR_SAVEERROR_UIN           = 952;
  ERR_NOVALIDFILENAME         = 'Нет правильного имени файла';
  ERR_NOVALIDFILENAME_UIN     = 953;
  ERR_NOFILENAME              = 'Нет файла с настройками COM-порта';
  ERR_NOFILENAME_UIN          = 954;
  ERR_WRONGBAUDRATE           = 'Неправильная скорость порта';
  ERR_WRONGBAUDRATE_UIN       = 955;

type
  EiaCOMPropError  = class(Exception)
  private
    fErrCode: Longint;
  public
    constructor Create(aMsg: String; aCode: LongInt);
    property ErrCode: LongInt read fErrCode write fErrCode;
  end;

  EiaCOMPropLoadError = class(EiaCOMPropError)
  end;

  EiaCOMPropSaveError = class(EiaCOMPropError)
  end;

type
  TiaCOMProperties = class(TObject)
  private
    fBaudRate: Integer;
    fByteSize: Integer;
    fDTR: Boolean;
    fDTRControl: LongInt;
    fParity: Integer;
    fPortNum: Byte;
    fRTS: Boolean;
    fRTSControl: LongInt;
    fStopBits: Integer;

    fTimeouts: TCOMMTimeouts;

    fCSRTS: TCriticalSection;
    fCSDTR: TCriticalSection;


    //Процедуры для установки/чтения таймаутов
    function GetReadIntervalTimeout: DWORD;
    function GetReadTotalTimeoutConstant: DWORD;
    function GetReadTotalTimeoutMultiplier: DWORD;
    function GetWriteTotalTimeoutConstant: DWORD;
    function GetWriteTotalTimeoutMultiplier: DWORD;
    procedure SetReadIntervalTimeout(const Value: Cardinal);
    procedure SetReadTotalTimeoutConstant(const Value: Cardinal);
    procedure SetReadTotalTimeoutMultiplier(const Value: Cardinal);
    procedure SetWriteTotalTimeoutConstant(const Value: Cardinal);
    procedure SetWriteTotalTimeoutMultiplier(const Value: Cardinal);
    function GetRTS: Boolean;
    procedure SetRTS(const Value: Boolean);
    function GetDTR: Boolean;
    procedure SetDTR(const Value: Boolean);
  public
    constructor Create;
    destructor Destroy; override;
    // Загрузка настроек из файла
    procedure LoadProperties(aINI_FileName: TFileName = DEF_CFG_FILE);
    // Сохранение настроек в файл
    procedure SaveProperties(aINI_FileName: TFileName = DEF_CFG_FILE);
    //Свойства
    property BaudRate: Integer read fBaudRate write fBaudRate;
    property ByteSize: Integer read fByteSize write fByteSize;
    // Файл для хранения/чтения настроек
    property DTR: Boolean read GetDTR write SetDTR;
    property DTRControl: LongInt read fDTRControl write fDTRControl;
    property Parity: Integer read fParity write fParity;
    property PortNum: Byte read fPortNum write fPortNum;
    property RTS: Boolean read GetRTS write SetRTS;
    property RTSControl: LongInt read fRTSControl write fRTSControl;
    property StopBits: Integer read fStopBits write fStopBits;

    property ReadIntervalTimeout: Cardinal read GetReadIntervalTimeout write SetReadIntervalTimeout;
    property ReadTotalTimeoutMultiplier: Cardinal read GetReadTotalTimeoutMultiplier write SetReadTotalTimeoutMultiplier;
    property ReadTotalTimeoutConstant: Cardinal read GetReadTotalTimeoutConstant write SetReadTotalTimeoutConstant;
    property WriteTotalTimeoutMultiplier: Cardinal read GetWriteTotalTimeoutMultiplier write SetWriteTotalTimeoutMultiplier;
    property WriteTotalTimeoutConstant: Cardinal read GetWriteTotalTimeoutConstant write SetWriteTotalTimeoutConstant;

    property Timeouts: TCommTimeouts read fTimeouts;
  end;

implementation

constructor TiaCOMProperties.Create;
begin
  inherited;
  fCSRTS := TCriticalSection.Create;
  fCSDTR := TCriticalSection.Create;
end;

destructor TiaCOMProperties.Destroy;
begin
  FreeAndNil(fCSDTR);
  FreeAndNil(fCSRTS);
  inherited;
end;

{ TiaCOMProperties }

function TiaCOMProperties.GetDTR: Boolean;
begin
  fCSDTR.Enter;
  Result := fDTR;
  fCSDTR.Leave;
end;

function TiaCOMProperties.GetReadIntervalTimeout: Cardinal;
begin
  Result := fTimeouts.ReadIntervalTimeout;
end;

function TiaCOMProperties.GetReadTotalTimeoutConstant: Cardinal;
begin
  Result := fTimeouts.ReadTotalTimeoutConstant;
end;

function TiaCOMProperties.GetReadTotalTimeoutMultiplier: Cardinal;
begin
  Result := fTimeouts.ReadTotalTimeoutMultiplier;
end;

function TiaCOMProperties.GetRTS: Boolean;
begin
  fCSRTS.Enter;
  Result := fRTS;
  fCSRTS.Leave;
end;

function TiaCOMProperties.GetWriteTotalTimeoutConstant: Cardinal;
begin
  Result := fTimeouts.WriteTotalTimeoutConstant;
end;

function TiaCOMProperties.GetWriteTotalTimeoutMultiplier: Cardinal;
begin
  Result := fTimeouts.WriteTotalTimeoutMultiplier;
end;

procedure TiaCOMProperties.LoadProperties(aINI_FileName: TFileName =
    DEF_CFG_FILE);
Var
  vIniFile: TIniFile;
begin
  if (not FileExists(aINI_FileName)) or (aINI_FileName = DEF_CFG_FILE) then
    raise EiaCOMPropLoadError.CreateHelp(ERR_NOFILENAME, ERR_NOFILENAME_UIN);

  Try
    vIniFile := TIniFile.Create(aINI_FileName);
    Try
      BaudRate := vIniFile.ReadInteger('COM port proprties', 'BaudRate', fBaudRate);
      ByteSize := vIniFile.ReadInteger('COM port proprties', 'ByteSize', fByteSize);
      Parity := vIniFile.ReadInteger('COM port proprties', 'Parity', fParity);
      PortNum := vIniFile.ReadInteger('COM port proprties', 'PortNum', fPortNum);
      StopBits := vIniFile.ReadInteger('COM port proprties', 'StopBits', fStopBits);
      DTR := vIniFile.ReadBool('COM port proprties', 'DTR', fDTR);
      RTS := vIniFile.ReadBool('COM port proprties', 'RTS', fRTS);
      DTRControl := vIniFile.ReadInteger('COM port proprties', 'DTRControl', fDTRControl);
      RTSControl := vIniFile.ReadInteger('COM port proprties', 'RTSControl', fRTSControl);

      SetReadIntervalTimeout(vIniFile.ReadInteger('COM port proprties', 'ReadIntervalTimeout', GetReadIntervalTimeout));
      SetReadTotalTimeoutConstant(vIniFile.ReadInteger('COM port proprties', 'ReadTotalTimeoutConstant', GetReadTotalTimeoutConstant));
      SetReadTotalTimeoutMultiplier(vIniFile.ReadInteger('COM port proprties', 'ReadTotalTimeoutMultiplier', GetReadTotalTimeoutMultiplier));

      SetWriteTotalTimeoutConstant(vIniFile.ReadInteger('COM port proprties', 'WriteTotalTimeoutConstant', GetWriteTotalTimeoutConstant));
      SetWriteTotalTimeoutMultiplier(vIniFile.ReadInteger('COM port proprties', 'WriteTotalTimeoutMultiplier', GetWriteTotalTimeoutMultiplier));
    Finally
      FreeAndNil(vIniFile);
    end;
  Except
    on e: Exception do
      Raise EiaCOMPropLoadError.CreateHelp(ERR_LOADERROR, ERR_LOADERROR_UIN);
  end;
end;

procedure TiaCOMProperties.SaveProperties(aINI_FileName: TFileName =
    DEF_CFG_FILE);
Var
  vIniFile: TIniFile;
begin
  Try
    if aINI_FileName = DEF_CFG_FILE then
      raise EiaCOMPropSaveError.CreateHelp(ERR_NOFILENAME, ERR_NOFILENAME_UIN);

    vIniFile := TIniFile.Create(aINI_FileName);
    Try
      vIniFile.WriteInteger('COM port proprties', 'BaudRate', fBaudRate);
      vIniFile.WriteInteger('COM port proprties', 'ByteSize', fByteSize);
      vIniFile.WriteInteger('COM port proprties', 'Parity', fParity);
      vIniFile.WriteInteger('COM port proprties', 'PortNum', fPortNum);
      vIniFile.WriteInteger('COM port proprties', 'StopBits', fStopBits);
      vIniFile.WriteBool('COM port proprties', 'DTR', fDTR);
      vIniFile.WriteBool('COM port proprties', 'RTS', fRTS);
      vIniFile.WriteInteger('COM port proprties', 'DTRControl', fDTRControl);
      vIniFile.WriteInteger('COM port proprties', 'RTSControl', fRTSControl);

      vIniFile.WriteInteger('COM port proprties', 'ReadIntervalTimeout', GetReadIntervalTimeout);
      vIniFile.WriteInteger('COM port proprties', 'ReadTotalTimeoutConstant', GetReadTotalTimeoutConstant);
      vIniFile.WriteInteger('COM port proprties', 'ReadTotalTimeoutMultiplier', GetReadTotalTimeoutMultiplier);

      vIniFile.WriteInteger('COM port proprties', 'WriteTotalTimeoutConstant', GetWriteTotalTimeoutConstant);
      vIniFile.WriteInteger('COM port proprties', 'WriteTotalTimeoutMultiplier', GetWriteTotalTimeoutMultiplier);
    Finally
      FreeAndNil(vIniFile);
    end;
  Except
    on e: EiaCOMPropSaveError do
      Raise;
    on e: Exception do
      Raise EiaCOMPropSaveError.CreateHelp(ERR_SAVEERROR, ERR_SAVEERROR_UIN);
  end;
end;

procedure TiaCOMProperties.SetDTR(const Value: Boolean);
begin
  fCSDTR.Enter;
  fDTR := Value;
  fCSDTR.Leave;
end;

procedure TiaCOMProperties.SetReadIntervalTimeout(const Value: Cardinal);
begin
    fTimeouts.ReadIntervalTimeout := Value;
end;

procedure TiaCOMProperties.SetReadTotalTimeoutConstant(const Value: Cardinal);
begin
  If Value > 0 then
    fTimeouts.ReadTotalTimeoutConstant := Value;
end;

procedure TiaCOMProperties.SetReadTotalTimeoutMultiplier(
  const Value: Cardinal);
begin
  fTimeouts.ReadTotalTimeoutMultiplier := Value;
end;

procedure TiaCOMProperties.SetRTS(const Value: Boolean);
begin
  fCSRTS.Enter;
  fRTS := Value;
  fCSRTS.Leave;
end;

procedure TiaCOMProperties.SetWriteTotalTimeoutConstant(
  const Value: Cardinal);
begin
  fTimeouts.WriteTotalTimeoutConstant := Value;
end;

procedure TiaCOMProperties.SetWriteTotalTimeoutMultiplier(
  const Value: Cardinal);
begin
  fTimeouts.WriteTotalTimeoutMultiplier := Value;
end;

{ EiaCOMPropError }

constructor EiaCOMPropError.Create(aMsg: String; aCode: Integer);
begin
  fErrCode := aCode;
  Message := aMsg;
end;

end.

