; Start
 
  !define MUI_PRODUCT "ExplorerXP 1.10"
  !define MUI_FILE "explorerxp"
  !define MUI_VERSION ""
  !define MUI_BRANDINGTEXT "ExplorerXP 1.10"
  CRCCheck On
 
  ; We should test if we must use an absolute path 
  !include "${NSISDIR}\Contrib\Modern UI\System.nsh"
 
 !define SOURCE_PATH "C:\dev\ExplorerXP\Setup"
 !define BIN_PATH  "C:\dev\bin2"
 
;---------------------------------
;General
 
  OutFile "explorerxpsetup.exe"
  ShowInstDetails "nevershow"
  ShowUninstDetails "nevershow"
  ;SetCompressor "bzip2"
 
  !define MUI_ICON "C:\dev\ExplorerXP\ExplorerXP\res\ExplorerXP.ico"
  !define MUI_UNICON "C:\dev\ExplorerXP\ExplorerXP\res\ExplorerXP.ico"
  !define MUI_SPECIALBITMAP "C:\dev\ExplorerXP\ExplorerXP\res\LogoXPQ.bmp"
 
 
;--------------------------------
;Folder selection page
 
;  InstallDir "$PROGRAMFILES\${MUI_PRODUCT}"
 InstallDir "$PROGRAMFILES\ExplorerXP"

 
;--------------------------------
;Modern UI Configuration
 
  !define MUI_WELCOMEPAGE  
  !define MUI_LICENSEPAGE
  !define MUI_DIRECTORYPAGE
  !define MUI_ABORTWARNING
  !define MUI_UNINSTALLER
  !define MUI_UNCONFIRMPAGE
  !define MUI_FINISHPAGE  
 
 
;--------------------------------
;Language
 
  !insertmacro MUI_LANGUAGE "English"
 
 
;-------------------------------- 
;Modern UI System
 
  !insertmacro MUI_SYSTEM 
 
 
;--------------------------------
;Data
 
;  LicenseData "Read_me.txt"
LicenseData "${SOURCE_PATH}\EvenSimplier.txt"
 
 
;-------------------------------- 
;Installer Sections     
Section "install" Installation info
 
;Add files
  SetOutPath "$INSTDIR" 
  File "${MUI_FILE}.exe"    
    
  File "${BIN_PATH}\ExplorerXP.exe"
  File "${BIN_PATH}\DupFind.exe"
  File "${BIN_PATH}\keyhelp.html"

  ; assosiate with folders
  
  ;WriteRegStr HKCR "Folder\Shell" "ExplorerXP" "" 
  WriteRegStr HKCR "Folder\Shell\ExplorerXP\Command" "" '"$INSTDIR\ExplorerXP.exe" "%1"'
  WriteRegStr HKCR "Drive\Shell\ExplorerXP\Command" "" '"$INSTDIR\ExplorerXP.exe" "%L"'
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ExplorerXP" "DisplayName" "ExplorerXP (remove only)"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ExplorerXP" "UninstallString" "$INSTDIR\Uninst.exe"
  
  WriteUninstaller "Uninst.exe"
	
  ;Add Shortcuts
  Delete "$SMPROGRAMS\ExplorerXP\*.*"
  RmDir "$SMPROGRAMS\ExplorerXP"
	
	
  CreateDirectory "$SMPROGRAMS\ExplorerXP"	
	
  CreateShortCut "$SMPROGRAMS\ExplorerXP\ExplorerXP.lnk" "$INSTDIR\ExplorerXP.exe" "" "$INSTDIR\ExplorerXP.exe" 0
  CreateShortCut "$SMPROGRAMS\ExplorerXP\Duplicate Find.lnk" "$INSTDIR\DupFind.exe" "" "$INSTDIR\DupFind.exe" 0
  CreateShortCut "$SMPROGRAMS\ExplorerXP\ExplorerXP Home.lnk" "http://www.explorerxp.com"
  CreateShortCut "$SMPROGRAMS\ExplorerXP\ExplorerXP Support Forums.lnk" "http://www.explorerxp.com/phpBB2/"
  CreateShortCut "$SMPROGRAMS\ExplorerXP\ExplorerXP Help.lnk" "$INSTDIR\keyhelp.html"
  CreateShortCut "$SMPROGRAMS\ExplorerXP\Uninstall.lnk" "$INSTDIR\Uninst.exe" "" "$INSTDIR\Uninst.exe"
 
SectionEnd
 
 
;--------------------------------    
;Uninstaller Section  
Section "Uninstall"
 
;Delete Files 
  RMDir /r "$INSTDIR\*.*"    
 
;Remove the installation directory
  RMDir "$INSTDIR"
 
;Delete Start Menu Shortcuts
  Delete "$DESKTOP\${MUI_PRODUCT}.lnk"
  Delete "$SMPROGRAMS\${MUI_PRODUCT}\*.*"
  RmDir  "$SMPROGRAMS\${MUI_PRODUCT}"
 
;Delete Uninstaller And Unistall Registry Entries
  DeleteRegKey HKEY_LOCAL_MACHINE "SOFTWARE\${MUI_PRODUCT}"
  DeleteRegKey HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${MUI_PRODUCT}"  
 
SectionEnd
 
 
;--------------------------------    
;MessageBox Section
 
 
;Function that calls a messagebox when installation finished correctly
Function .onInstSuccess
  MessageBox MB_OK "You have successfully installed ${MUI_PRODUCT}. Use the desktop icon to start the program."
FunctionEnd
 
Function un.onUninstSuccess
  MessageBox MB_OK "You have successfully uninstalled ${MUI_PRODUCT}."
FunctionEnd
 
 
;eof