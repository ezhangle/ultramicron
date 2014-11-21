program Geiger;

uses
  Forms,
  main in 'main.pas' {mainFrm},
  About_f in 'About_f.pas' {About},
  iaCOMProperties in 'iaCOMProperties.pas',
  iaRS232 in 'iaRS232.PAS',
  Unit1 in 'Unit1.pas' {Form1};

{$R *.res}

begin
  Application.Initialize;
  Application.Title := 'USB Geiger';
  Application.CreateForm(TmainFrm, mainFrm);
  Application.CreateForm(TAbout, About);
  Application.CreateForm(TForm1, Form1);
  Application.Run;
end.
