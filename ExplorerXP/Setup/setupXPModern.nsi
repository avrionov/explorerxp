; Script generated with the Venis Install Wizard

; Define your application name
!define APPNAME "ExplorerXP"
!define APPNAMEANDVERSION "ExplorerXP 1.05"

; Main Install settings
Name "${APPNAMEANDVERSION}"
InstallDir "$PROGRAMFILES\ExplorerXP"
InstallDirRegKey HKLM "Software\${APPNAME}" ""
OutFile "explorerxpsetupmodern.exe"

; Use compression
SetCompressor LZMA

; Modern interface settings
!include "MUI.nsh"

!define MUI_ABORTWARNING
!define MUI_FINISHPAGE_RUN "$INSTDIR\explorerxp_modern.exe"

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "C:\Prj\ExplorerXP\Setup\EvenSimplier.txt"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

; Set languages (first is default language)
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_RESERVEFILE_LANGDLL

Section "ExplorerXP" Section1

	; Set Section properties
	SetOverwrite on
	SectionIn RO
	; Set Section Files and Shortcuts
	SetOutPath "$INSTDIR\"
	File "C:\Prj\bin\explorerxp.exe"
	File "C:\Prj\bin\keyhelp.html"
	CreateShortCut "$DESKTOP\ExplorerXP.lnk" "$INSTDIR\explorerxp.exe"
	CreateDirectory "$SMPROGRAMS\ExplorerXP"
	CreateShortCut "$SMPROGRAMS\ExplorerXP\ExplorerXP.lnk" "$INSTDIR\explorerxp.exe"
	CreateShortCut "$SMPROGRAMS\ExplorerXP\Uninstall.lnk" "$INSTDIR\uninstall.exe"

SectionEnd

Section -FinishSection

	WriteRegStr HKLM "Software\${APPNAME}" "" "$INSTDIR"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayName" "${APPNAME}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "UninstallString" "$INSTDIR\uninstall.exe"
	WriteUninstaller "$INSTDIR\uninstall.exe"

SectionEnd


Section "Desktop Shortcut" Section2
  CreateShortCut "$DESKTOP\ExplorerXP.lnk" "$INSTDIR\ExplorerXP.exe" "" "$INSTDIR\ExplorerXP.exe" 0  
SectionEnd

Section "Quick Launch Shortcut"  Section3
  CreateShortCut "$QUICKLAUNCH\ExplorerXP.lnk" "$INSTDIR\ExplorerXP.exe" "" "$INSTDIR\ExplorerXP.exe" 0
SectionEnd

; Modern install component descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
	!insertmacro MUI_DESCRIPTION_TEXT ${Section1} ""
	!insertmacro MUI_DESCRIPTION_TEXT ${Section2} ""
	!insertmacro MUI_DESCRIPTION_TEXT ${Section3} ""
	!insertmacro MUI_DESCRIPTION_TEXT ${Section4} ""
!insertmacro MUI_FUNCTION_DESCRIPTION_END

;Uninstall section
Section Uninstall

	;Remove from registry...
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}"
	DeleteRegKey HKLM "SOFTWARE\${APPNAME}"

	; Delete self
	Delete "$INSTDIR\uninstall.exe"

	; Delete Shortcuts
	Delete "$DESKTOP\ExplorerXP.lnk"
	Delete "$SMPROGRAMS\ExplorerXP\ExplorerXP.lnk"
	Delete "$SMPROGRAMS\ExplorerXP\Uninstall.lnk"

	; Clean up ExplorerXP
	Delete "$INSTDIR\explorerxp.exe"
	Delete "$INSTDIR\keyhelp.html"

	; Remove remaining directories
	RMDir "$SMPROGRAMS\ExplorerXP"
	RMDir "$INSTDIR\"

SectionEnd

BrandingText "www.explorerxp.com"

; eof