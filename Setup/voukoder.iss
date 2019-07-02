#define MyAppName "Voukoder"
#define MyAppPublisher "Daniel Stankewitz"
#define MyAppURL "http://www.voukoder.org"

[Setup]
AppId={{9F919D76-F1AC-4813-8B10-AB22E8F5015D}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={code:GetPrmPath}
DisableProgramGroupPage=yes
DisableDirPage=yes
LicenseFile=C:\Users\Daniel\source\repos\voukoder\LICENSE
OutputBaseFilename=setup
Compression=lzma
SolidCompression=yes
WizardSmallImageFile=logo.bmp
WizardStyle=modern

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "brazilianportuguese"; MessagesFile: "compiler:Languages\BrazilianPortuguese.isl"
Name: "catalan"; MessagesFile: "compiler:Languages\Catalan.isl"
Name: "corsican"; MessagesFile: "compiler:Languages\Corsican.isl"
Name: "czech"; MessagesFile: "compiler:Languages\Czech.isl"
Name: "danish"; MessagesFile: "compiler:Languages\Danish.isl"
Name: "dutch"; MessagesFile: "compiler:Languages\Dutch.isl"
Name: "finnish"; MessagesFile: "compiler:Languages\Finnish.isl"
Name: "french"; MessagesFile: "compiler:Languages\French.isl"
Name: "german"; MessagesFile: "compiler:Languages\German.isl"
Name: "hebrew"; MessagesFile: "compiler:Languages\Hebrew.isl"
Name: "italian"; MessagesFile: "compiler:Languages\Italian.isl"
Name: "japanese"; MessagesFile: "compiler:Languages\Japanese.isl"
Name: "norwegian"; MessagesFile: "compiler:Languages\Norwegian.isl"
Name: "polish"; MessagesFile: "compiler:Languages\Polish.isl"
Name: "portuguese"; MessagesFile: "compiler:Languages\Portuguese.isl"
Name: "russian"; MessagesFile: "compiler:Languages\Russian.isl"
Name: "slovenian"; MessagesFile: "compiler:Languages\Slovenian.isl"
Name: "spanish"; MessagesFile: "compiler:Languages\Spanish.isl"
Name: "turkish"; MessagesFile: "compiler:Languages\Turkish.isl"
Name: "ukrainian"; MessagesFile: "compiler:Languages\Ukrainian.isl"

[Components]
Name: "prm";      Description: "Voukoder for Premiere / Media Encoder";  Types: full compact;
Name: "aex";      Description: "Voukoder for After Effects";             Types: full;

[Files]
Source: "..\PremierePlugin\x64\Release\VoukoderR2.prm"; DestDir: "{code:GetDir|0}"; Flags: ignoreversion; Components: prm;
Source: "..\AfterEffectsPlugin\x64\Release\VoukoderR2.aex"; DestDir: "{code:GetDir|1}"; Flags: ignoreversion; Components: aex;

[Run]
Filename: "https://www.patreon.com/voukoder"; Flags: shellexec runasoriginaluser postinstall unchecked; Description: "Donate with Patreon"
Filename: "https://www.paypal.me/voukoder"; Flags: shellexec runasoriginaluser postinstall unchecked; Description: "Donate via PayPal.me"

[Code]
var
  DirPage: TInputDirWizardPage;

function GetPrmPath(Path: String): String;
begin
  RegQueryStringValue(HKLM64, 'Software\Adobe\Premiere Pro\CurrentVersion', 'Plug-InsDir', Path);
  Result := Path;
end;

function GetAexPath(): String;
var
  Versions: TArrayOfString;
  I: Integer;
  Version: String;
  Path: String;
begin
  if RegGetSubkeyNames(HKLM64, 'Software\Adobe\After Effects', Versions) then
  begin
    Version := '';
    for I := 0 to GetArrayLength(Versions) - 1 do
      if (Version = '') OR (StrToFloat(Versions[I]) > StrToFloat(Version)) then Version := Versions[I];
    if Version <> '' then
    begin
      if RegQueryStringValue(HKLM64, 'Software\Adobe\After Effects\' + Version, 'CommonPluginInstallPath', Path) then
      begin
        Result := Path;
        Exit;
      end;
    end;
  end;
  Result := '';
end;

function GetDir(Param: String): String;
begin
  Result := DirPage.Values[StrToInt(Param)];
end;

procedure InitializeWizard;
var
  desc: String;
  labl: String;
begin
  desc := SetupMessage(msgSelectDirDesc);
  labl := SetupMessage(msgSelectDirLabel3);
  StringChangeEx(desc, '[name]', '{#SetupSetting("AppVerName")}', True);
  StringChangeEx(labl, '[name]', '{#SetupSetting("AppVerName")}', True);
  DirPage := CreateInputDirPage(wpSelectComponents, SetupMessage(msgWizardSelectDir), desc, labl, False, '');
  DirPage.Add('Adobe Premiere / Media Encoder');
  DirPage.Add('Adobe After Effects');
  DirPage.Values[0] := GetPreviousData('PrmDir', GetPrmPath(''));
  DirPage.Values[1] := GetPreviousData('AexDir', GetAexPath());
end;

procedure RegisterPreviousData(PreviousDataKey: Integer);
begin
  SetPreviousData(PreviousDataKey, 'PrmDir', DirPage.Values[0]);
  SetPreviousData(PreviousDataKey, 'AexDir', DirPage.Values[1]);
end;

procedure CurPageChanged(CurPageID: Integer);
var
  IsPrm: Boolean;
  IsAex: Boolean;
begin
  if CurPageID = 100 then
  begin
    IsPrm := WizardIsComponentSelected('prm');
    DirPage.PromptLabels[0].Enabled := IsPrm;
    DirPage.Edits[0].Enabled := IsPrm;
    DirPage.Buttons[0].Enabled := IsPrm;
    if IsPrm then
      DirPage.Values[0] := GetPreviousData('PrmDir', GetPrmPath(''))
    else
      DirPage.Values[0] := 'C:\';
    IsAex := WizardIsComponentSelected('aex');
    DirPage.PromptLabels[1].Enabled := IsAex;
    DirPage.Edits[1].Enabled := IsAex;
    DirPage.Buttons[1].Enabled := IsAex;
    if IsAex then
      DirPage.Values[1] := GetPreviousData('AexDir', GetAexPath())
    else
      DirPage.Values[1] := 'C:\';
  end;
end;

function NextButtonClick(CurPageID: Integer): Boolean;
begin
  Result := True;
  if CurPageID = wpSelectComponents then
  begin
    if WizardSelectedComponents(False) = '' then
    begin
      MsgBox('No component selected', mbInformation, MB_OK);
      Result := False;
    end;
  end;
end;