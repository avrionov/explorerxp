;NSIS Script For ExplorerXP

Function .onInit
  InitPluginsDir
  File /oname=$PLUGINSDIR\splash.bmp "C:\Prj\ExplorerXP\res\LogoXPQ.bmp"
  splash::show 1000 $PLUGINSDIR\splash
  Pop $0
FunctionEnd

Icon "C:\Prj\ExplorerXP\res\nsis1-install.ico"

XPStyle on

;Title Of Your Application
Name "ExplorerXP"

;Do A CRC Check
CRCCheck On

;Output File Name
OutFile "explorerxpsetup.exe"

;The Default Installation Directory
InstallDir "$PROGRAMFILES\ExplorerXP"

LicenseText "Please read the following License Agreement. Use the scrollbar or press the Page Down key to view the rest of the agreement."
LicenseData "C:\Prj\ExplorerXP\Setup\EvenSimplier.txt"

;The text to prompt the user to enter a directory
DirText "Please select the folder below"

Section "Install"
  ;Install Files
  SetOutPath $INSTDIR
  SetCompress Auto
  SetOverwrite IfNewer
  File "C:\Prj\bin\ExplorerXP.exe"
  File "C:\Prj\bin\keyhelp.html"

  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ExplorerXP" "DisplayName" "ExplorerXP (remove only)"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ExplorerXP" "UninstallString" "$INSTDIR\Uninst.exe"
WriteUninstaller "Uninst.exe"
SectionEnd

Section "Shortcuts"
  ;Add Shortcuts
  Delete "$SMPROGRAMS\ExplorerXP\*.*"
  RmDir "$SMPROGRAMS\ExplorerXP"
  CreateDirectory "$SMPROGRAMS\ExplorerXP"
  CreateShortCut "$SMPROGRAMS\ExplorerXP\ExplorerXP.lnk" "$INSTDIR\ExplorerXP.exe" "" "$INSTDIR\ExplorerXP.exe" 0
  CreateShortCut "$SMPROGRAMS\ExplorerXP\ExplorerXP Home.lnk" "http:\\www.explorerxp.com"
  CreateShortCut "$SMPROGRAMS\ExplorerXP\ExplorerXP Help.lnk" "$INSTDIR\keyhelp.html"
  CreateShortCut "$SMPROGRAMS\ExplorerXP\Uninstall.lnk" "$INSTDIR\Uninst.exe" "" "$INSTDIR\Uninst.exe"
  CreateShortCut "$DESKTOP\ExplorerXP.lnk" "$INSTDIR\ExplorerXP.exe" "" "$INSTDIR\ExplorerXP.exe" 0
  CreateShortCut "$QUICKLAUNCH\ExplorerXP.lnk" "$INSTDIR\ExplorerXP.exe" "" "$INSTDIR\ExplorerXP.exe" 0
SectionEnd

UninstallText "This will uninstall ExplorerXP from your system"

Section Uninstall
  ;Delete Files
  Delete "$INSTDIR\ExplorerXP.exe"
  Delete "$DESKTOP\ExplorerXP.lnk"

  ;Delete Start Menu Shortcuts
  Delete "$SMPROGRAMS\ExplorerXP\*.*"
  RmDir "$SMPROGRAMS\ExplorerXP"

  ;Delete Uninstaller And Unistall Registry Entries
  Delete "$INSTDIR\Uninst.exe"
  DeleteRegKey HKEY_LOCAL_MACHINE "SOFTWARE\ExplorerXP"
  DeleteRegKey HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\ExplorerXP"
  Delete "$DESKTOP\ExplorerXP.lnk"
  Delete "$QUICKLAUNCH\ExplorerXP.lnk"
  RMDir "$INSTDIR"
SectionEnd