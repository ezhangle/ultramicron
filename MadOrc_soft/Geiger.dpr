program Geiger;

uses
  Forms,
  main in 'main.pas' {mainFrm},
  About_f in 'About_f.pas' {About},
  iaCOMProperties in 'iaCOMProperties.pas',
  iaRS232 in 'iaRS232.PAS';

{$R *.res}

begin
  Application.Initialize;
  Application.Title := 'USB Geiger';
  Application.CreateForm(TmainFrm, mainFrm);
  Application.CreateForm(TAbout, About);
  Application.Run;
end.
