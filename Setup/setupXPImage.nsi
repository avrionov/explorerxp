;NSIS Script For ExplorerXP

SetCompressor lzma

Icon "C:\Prj\ExplorerXP\res\nsis1-install.ico"

XPStyle on

;Title Of Your Application
Name "ExplorerXP"

;Do A CRC Check
CRCCheck On

;Output File Name
OutFile "explorerxpsetupImage.exe"


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
	File "C:\Prj\bin\cximagecrt.dll"

  ; assosiate with folders
  
  ;WriteRegStr HKCR "Folder\Shell" "ExplorerXP" "" 
  WriteRegStr HKCR "Folder\Shell\ExplorerXP\Command" "" '"$INSTDIR\ExplorerXP.exe" "%1"'
  WriteRegStr HKCR "Drive\Shell\ExplorerXP\Command" "" '"$INSTDIR\ExplorerXP.exe" "%L"'
  
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
  Delete "$INSTDIR\keyhelp.html"
  Delete "$DESKTOP\ExplorerXP.lnk"

  ;Delete Start Menu Shortcuts
  Delete "$SMPROGRAMS\ExplorerXP\*.*"
  RmDir "$SMPROGRAMS\ExplorerXP"

  ;Delete Uninstaller And Unistall Registry Entries
  Delete "$INSTDIR\Uninst.exe"
  DeleteRegKey HKCU "SOFTWARE\ExplorerXP"
  DeleteRegKey HKLM "SOFTWARE\ExplorerXP"
  DeleteRegKey HKCR "Folder\Shell\ExplorerXP\"
  DeleteRegKey HKCR "Drive\Shell\ExplorerXP\"
  
  DeleteRegKey HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\ExplorerXP"
  Delete "$DESKTOP\ExplorerXP.lnk"
  Delete "$QUICKLAUNCH\ExplorerXP.lnk"
  RMDir "$INSTDIR"
SectionEnd