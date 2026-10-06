; ============================================================================
;  简单计算器 (Qt 6 / QML) —— Windows 安装程序脚本
;
;  用法：
;    makensis -DNSISDIR=... calculator_qml.nsi
;  或直接用 ../build-installer.sh 一键构建。
;
;  安装内容：calculator_qml_win64/ 目录下的全部文件
;            （exe + Qt 运行库 + QML 模块 + 插件），目标机无需安装 Qt。
; ============================================================================

Unicode true

!include "MUI2.nsh"
!include "LogicLib.nsh"
!include "x64.nsh"
!include "FileFunc.nsh"

; ---------------------------------------------------------------- 基本信息 --
!define APP_NAME      "简单计算器"
!define APP_NAME_EN   "QML Calculator"
!define APP_VERSION   "1.0.0"
!define APP_PUBLISHER "learn_cplusplus"
!define APP_EXE       "calculator_qml.exe"
!define APP_DIRNAME   "CalculatorQml"
!define APP_REGKEY    "Software\${APP_DIRNAME}"
!define APP_UNINSTKEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_DIRNAME}"

; 待打包的程序目录（可由命令行 -DSRCDIR=... 覆盖）
!ifndef SRCDIR
  !define SRCDIR "..\calculator_qml_win64"
!endif
!ifndef OUTFILE
  !define OUTFILE "..\calculator_qml-setup-${APP_VERSION}.exe"
!endif

Name "${APP_NAME} ${APP_VERSION}"
OutFile "${OUTFILE}"
InstallDir "$PROGRAMFILES64\${APP_DIRNAME}"
InstallDirRegKey HKLM "${APP_REGKEY}" "InstallDir"

RequestExecutionLevel admin

SetCompressor /SOLID lzma
SetCompressorDictSize 64

ShowInstDetails show
ShowUninstDetails show
BrandingText "${APP_NAME} ${APP_VERSION}"

VIProductVersion "1.0.0.0"
VIAddVersionKey /LANG=2052 "ProductName"     "${APP_NAME}"
VIAddVersionKey /LANG=2052 "FileDescription" "${APP_NAME} 安装程序"
VIAddVersionKey /LANG=2052 "FileVersion"     "${APP_VERSION}"
VIAddVersionKey /LANG=2052 "ProductVersion"  "${APP_VERSION}"
VIAddVersionKey /LANG=2052 "CompanyName"     "${APP_PUBLISHER}"
VIAddVersionKey /LANG=2052 "LegalCopyright"  "${APP_PUBLISHER}"

; ---------------------------------------------------------------- 界面 ------
!define MUI_ABORTWARNING
!define MUI_WELCOMEPAGE_TITLE "欢迎安装 ${APP_NAME}"
!define MUI_WELCOMEPAGE_TEXT "安装程序将把 ${APP_NAME} ${APP_VERSION} 安装到您的电脑。$\r$\n$\r$\n程序自带了全部 Qt 运行库，安装后即可直接使用，不需要另外安装 Qt 或其他运行环境。$\r$\n$\r$\n需要 64 位 Windows 10 / 11。"

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES

!define MUI_FINISHPAGE_RUN "$INSTDIR\${APP_EXE}"
!define MUI_FINISHPAGE_RUN_TEXT "立即运行 ${APP_NAME}"
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

; 程序界面本身只有中文，安装程序就固定用简体中文。
; （若同时插入 English，NSIS 会按系统语言自动选择，英文系统会显示英文界面。）
!insertmacro MUI_LANGUAGE "SimpChinese"

; ---------------------------------------------------------------- 初始化 ----
Function .onInit
    SetRegView 64
    SetShellVarContext all

    ; 本程序由 MinGW-w64 交叉编译为 64 位，必须在 64 位 Windows 上运行
    ${IfNot} ${RunningX64}
        MessageBox MB_ICONSTOP "本程序是 64 位版本，只能安装在 64 位 Windows 10 / 11 上。"
        Abort
    ${EndIf}

    ; 已安装过就先卸载旧版本，避免残留
    Call CheckOldInstall
FunctionEnd

; 覆盖安装时先静默卸载旧版本；交互模式下先问一句
Function CheckOldInstall
    ReadRegStr $0 HKLM "${APP_REGKEY}" "InstallDir"
    StrCmp $0 "" done
    IfFileExists "$0\uninstall.exe" 0 done

    IfSilent silent_uninstall
    MessageBox MB_OKCANCEL|MB_ICONQUESTION "检测到已安装的 ${APP_NAME}：$\r$\n$0$\r$\n$\r$\n是否先卸载旧版本？" IDOK uninstall_old
    Abort

    uninstall_old:
    silent_uninstall:
    ExecWait '"$0\uninstall.exe" /S _?=$0'

    done:
FunctionEnd

Function un.onInit
    ; 卸载前确认程序没有在运行
    IfFileExists "$INSTDIR\${APP_EXE}" 0 done
    ClearErrors
    FileOpen $0 "$INSTDIR\${APP_EXE}" a
    ${If} ${Errors}
        MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION "${APP_NAME} 似乎正在运行。$\r$\n请先关闭程序，然后点击「确定」继续卸载。" IDOK done
        Abort
    ${Else}
        FileClose $0
    ${EndIf}
    done:
FunctionEnd

; ---------------------------------------------------------------- 安装 ------
Section "主程序（必需）" SecMain
    SectionIn RO
    SetRegView 64
    SetOutPath "$INSTDIR"
    SetOverwrite on

    File /r "${SRCDIR}\*.*"

    ; 卸载信息，显示在「设置 → 应用 / 程序和功能」中
    WriteRegStr HKLM "${APP_REGKEY}" "InstallDir" "$INSTDIR"
    WriteRegStr HKLM "${APP_REGKEY}" "Version"    "${APP_VERSION}"

    WriteRegStr HKLM "${APP_UNINSTKEY}" "DisplayName"     "${APP_NAME}"
    WriteRegStr HKLM "${APP_UNINSTKEY}" "DisplayVersion"  "${APP_VERSION}"
    WriteRegStr HKLM "${APP_UNINSTKEY}" "Publisher"       "${APP_PUBLISHER}"
    WriteRegStr HKLM "${APP_UNINSTKEY}" "DisplayIcon"     "$INSTDIR\${APP_EXE}"
    WriteRegStr HKLM "${APP_UNINSTKEY}" "InstallLocation" "$INSTDIR"
    WriteRegStr HKLM "${APP_UNINSTKEY}" "UninstallString" '"$INSTDIR\uninstall.exe"'
    WriteRegStr HKLM "${APP_UNINSTKEY}" "QuietUninstallString" '"$INSTDIR\uninstall.exe" /S'
    WriteRegDWORD HKLM "${APP_UNINSTKEY}" "NoModify" 1
    WriteRegDWORD HKLM "${APP_UNINSTKEY}" "NoRepair" 1

    ${GetSize} "$INSTDIR" "/S=0K" $0 $1 $2
    WriteRegDWORD HKLM "${APP_UNINSTKEY}" "EstimatedSize" "$0"

    WriteUninstaller "$INSTDIR\uninstall.exe"
SectionEnd

Section "开始菜单快捷方式" SecStartMenu
    CreateDirectory "$SMPROGRAMS\${APP_NAME}"
    CreateShortCut "$SMPROGRAMS\${APP_NAME}\${APP_NAME}.lnk" "$INSTDIR\${APP_EXE}" "" "$INSTDIR\${APP_EXE}" 0
    CreateShortCut "$SMPROGRAMS\${APP_NAME}\卸载 ${APP_NAME}.lnk" "$INSTDIR\uninstall.exe"
SectionEnd

Section "桌面快捷方式" SecDesktop
    CreateShortCut "$DESKTOP\${APP_NAME}.lnk" "$INSTDIR\${APP_EXE}" "" "$INSTDIR\${APP_EXE}" 0
SectionEnd

; ---------------------------------------------------------------- 说明 ------
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${SecMain}      "程序主体：${APP_EXE} 及它需要的 Qt 运行库、QML 模块和插件。"
    !insertmacro MUI_DESCRIPTION_TEXT ${SecStartMenu} "在开始菜单中创建 ${APP_NAME} 的快捷方式。"
    !insertmacro MUI_DESCRIPTION_TEXT ${SecDesktop}   "在桌面上创建 ${APP_NAME} 的快捷方式。"
!insertmacro MUI_FUNCTION_DESCRIPTION_END

; ---------------------------------------------------------------- 卸载 ------
Section "Uninstall"
    SetRegView 64
    SetShellVarContext all

    Delete "$DESKTOP\${APP_NAME}.lnk"
    Delete "$SMPROGRAMS\${APP_NAME}\${APP_NAME}.lnk"
    Delete "$SMPROGRAMS\${APP_NAME}\卸载 ${APP_NAME}.lnk"
    RMDir  "$SMPROGRAMS\${APP_NAME}"

    RMDir /r "$INSTDIR\qml"
    RMDir /r "$INSTDIR\plugins"
    Delete   "$INSTDIR\*.dll"
    Delete   "$INSTDIR\${APP_EXE}"
    Delete   "$INSTDIR\uninstall.exe"
    RMDir    "$INSTDIR"

    DeleteRegKey HKLM "${APP_UNINSTKEY}"
    DeleteRegKey HKLM "${APP_REGKEY}"
SectionEnd
