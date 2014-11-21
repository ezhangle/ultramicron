program prjMain;

uses
  Forms,
  untMain in 'untMain.pas' {Form1},
  iaPostCommands in '..\Prog\classes\iaPostCommands.pas',
  iaPostCMDTypes in '..\Prog\classes\iaPostCMDTypes.pas',
  iaPostCommonTypes in '..\Prog\classes\iaPostCommonTypes.pas';

{$R *.res}

begin
  Application.Initialize;
  Application.CreateForm(TForm1, Form1);
  Application.Run;
end.
