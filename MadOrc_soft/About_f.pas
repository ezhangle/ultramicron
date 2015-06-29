unit About_f;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, pngimage, ExtCtrls, Registry;

type
  TAbout = class(TForm)
    Label1: TLabel;
    Image1: TImage;
    Button1: TButton;
    Label2: TLabel;
    Label3: TLabel;
    Label4: TLabel;
    Label5: TLabel;
    Edit1: TEdit;
    Label6: TLabel;
    procedure Button1Click(Sender: TObject);
    procedure FormCreate(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  About: TAbout;
implementation

{$R *.dfm}

procedure TAbout.Button1Click(Sender: TObject);
var
  reg: TRegistry;
begin
  reg := TRegistry.Create;                               // Открываем реестр
  reg.RootKey := HKEY_CURRENT_USER;                      // Для текущего пользователя
  reg.OpenKey('Software\USB_Geiger\USB_Geiger', true); // Открываем раздел
  reg.WriteString('Reg_key', Edit1.Text);
  reg.CloseKey;                                          // Закрываем раздел
  reg.Free;
About.Close;
end;

procedure TAbout.FormCreate(Sender: TObject);
var
  reg: TRegistry;
begin

  reg := TRegistry.Create;                              // Открываем реестр
  reg.RootKey := HKEY_CURRENT_USER;
  reg.OpenKey('Software\USB_Geiger\USB_Geiger', false);
  Edit1.Text := reg.ReadString('Reg_key');
  reg.CloseKey;                                          // Закрываем раздел
end;

end.
