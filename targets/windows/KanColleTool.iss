; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "KanColleTool"
#define MyAppVersion "0.8.0"
#define MyAppPublisher "MacaroniCode"
#define MyAppURL "http://kancolletool.github.io/"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{E4E165A9-C85C-4531-B1DC-B6F696298DA8}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\{#MyAppName}
DefaultGroupName={#MyAppName}
AllowNoIcons=yes
OutputBaseFilename=KanColleTool-Setup
Compression=lzma
SolidCompression=yes
SourceDir=..\..\dist\KanColleTool
OutputDir=..

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Files]
Source: "*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\KanColleTool"; Filename: "{app}\KanColleTool.exe"
Name: "{group}\KCT Viewer"; Filename: "{app}\KCTViewer.exe"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"

