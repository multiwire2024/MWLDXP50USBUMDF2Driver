powershell ./find-vs.ps1 >%TMP%\t
set /p vs_root= <%TMP%\t

set sdk_root="

set DevEnvDir="%vs_root%\Common7\IDE\"

rem c:\"Program Files"\"Microsoft Visual Studio"\2022\Enterprise\Common7\Tools\VsDevCmd.bat -arch=%2 -host_arch=amd64 -winsdk=10.0.19041.0
set DevEnvDir="%vs_root%\Common7\IDE\"
set ExtensionSdkDir="C:\Program Files (x86)\Microsoft SDKs\Windows Kits\10\ExtensionSDKs"
set FPS_BROWSER_APP_PROFILE_STRING="Internet Explorer"
set FPS_BROWSER_USER_PROFILE_STRING="Default"
set Framework40Version="v4.0"
set FrameworkDir="C:\WINDOWS\Microsoft.NET\Framework\"
set FrameworkDIR32="C:\WINDOWS\Microsoft.NET\Framework\"
set FrameworkVersion="v4.0.30319"
set FrameworkVersion32="v4.0.30319"
set IFCPATH="%vs_root%\VC\Tools\MSVC\14.29.30133\ifc\x86"
set INCLUDE="%vs_root%\VC\Tools\MSVC\14.29.30133\ATLMFC\include;%vs_root%\VC\Tools\MSVC\14.29.30133\include;C:\Program Files (x86)\Windows Kits\NETFXSDK\4.8\include\um;C:\Program Files (x86)\Windows Kits\10\include\10.0.19041.0\ucrt;C:\Program Files (x86)\Windows Kits\10\include\10.0.19041.0\shared;C:\Program Files (x86)\Windows Kits\10\include\10.0.19041.0\km;C:\Program Files (x86)\Windows Kits\10\include\10.0.22621.0\km;C:\Program Files (x86)\Windows Kits\10\include\10.0.22621.0\winrt;C:\Program Files (x86)\Windows Kits\10\include\10.0.22621.0\cppwinrt"
set LIB="%vs_root%\VC\Tools\MSVC\14.29.30133\ATLMFC\lib\x86;%vs_root%\VC\Tools\MSVC\14.29.30133\lib\x86;C:\Program Files (x86)\Windows Kits\NETFXSDK\4.8\lib\um\x86;C:\Program Files (x86)\Windows Kits\10\lib\10.0.19041.0\ucrt\x86;C:\Program Files (x86)\Windows Kits\10\lib\10.0.19041.0\um\x86"
set LIBPATH="%vs_root%\VC\Tools\MSVC\14.29.30133\ATLMFC\lib\x86;%vs_root%\VC\Tools\MSVC\14.29.30133\lib\x86;%vs_root%\VC\Tools\MSVC\14.29.30133\lib\x86\store\references;C:\Program Files (x86)\Windows Kits\10\UnionMetadata\10.0.19041.0;C:\Program Files (x86)\Windows Kits\10\References\10.0.19041.0;C:\WINDOWS\Microsoft.NET\Framework\v4.0.30319"
set MSBuildLoadMicrosoftTargetsReadOnly="true"
set NETFXSDKDir="C:\Program Files (x86)\Windows Kits\NETFXSDK\4.8\"
set PkgDefApplicationConfigFile="C:\Users\user\AppData\Local\Microsoft\VisualStudio\16.0_c256b1af\devenv.exe.config"
set UCRTVersion="10.0.19041.0"
set UniversalCRTSdkDir="C:\Program Files (x86)\Windows Kits\10\"
set VCIDEInstallDir="%vs_root%\Common7\IDE\VC\"
set VCINSTALLDIR="%vs_root%\VC\"
set VCToolsInstallDir="%vs_root%\VC\Tools\MSVC\14.29.30133\"
set VCToolsRedistDir="%vs_root%\VC\Redist\MSVC\14.29.30133\"
set VCToolsVersion="14.29.30133"
set VisualStudioDir="C:\Users\user\OneDrive\Documents\Visual Studio 2019"
set VisualStudioEdition="Microsoft Visual Studio Enterprise 2019"
set VisualStudioVersion=16.0
set VS140COMNTOOLS="C:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\Tools\"
set VS160COMNTOOLS="%vs_root%\Common7\Tools\"
set VSAPPIDDIR="%vs_root%\Common7\IDE\"
set VSAPPIDNAME="devenv.exe"
set VSCMD_ARG_app_plat="Desktop"
set VSCMD_ARG_HOST_ARCH="x86"
set VSCMD_ARG_TGT_ARCH="x86"
set VSCMD_VER="16.11.34"
set VSINSTALLDIR="%vs_root%\"
set VSLANG=1033
set VSSKUEDITION="Enterprise"
set WindowsLibPath="C:\Program Files (x86)\Windows Kits\10\UnionMetadata\10.0.19041.0;C:\Program Files (x86)\Windows Kits\10\References\10.0.19041.0"
set WindowsSdkBinPath="C:\Program Files (x86)\Windows Kits\10\bin\"
set WindowsSdkDir="C:\Program Files (x86)\Windows Kits\10\"
set WindowsSDKLibVersion="10.0.19041.0\"
set WindowsSdkVerBinPath="C:\Program Files (x86)\Windows Kits\10\bin\10.0.19041.0\"
set WindowsSDKVersion="10.0.22621.0\"
set WindowsSDK_ExecutablePath_x64="C:\Program Files (x86)\Microsoft SDKs\Windows\v10.0A\bin\NETFX 4.8 Tools\x64\"
set WindowsSDK_ExecutablePath_x86="C:\Program Files (x86)\Microsoft SDKs\Windows\v10.0A\bin\NETFX 4.8 Tools\"
set __devinit_path="%vs_root%\Common7\Tools\devinit\devinit.exe"
set __DOTNET_ADD_32BIT=1
set __DOTNET_PREFERRED_BITNESS=32
set __VSCMD_PREINIT_PATH="C:\WINDOWS\system32;C:\WINDOWS;C:\WINDOWS\System32\Wbem;C:\WINDOWS\System32\WindowsPowerShell\v1.0\;C:\Program Files\Microsoft SQL Server\130\Tools\Binn\;C:\Program Files\Git\usr\bin;C:\Program Files\Microsoft SQL Server\150\Tools\Binn\;C:\Program Files\dotnet\;C:\Program Files (x86)\Windows Kits\10\Windows Performance Toolkit\;C:\Users\user\AppData\Local\Microsoft\WindowsApps;C:\Users\user\.dotnet\tools"
set __VSCMD_script_err_count="0"

set IC35PATH=%cd%\%4
rem dir c:\"Program Files (x86)"\"Windows Kits"\10\DesignTime\CommonConfiguration\Neutral\UAP\10.0.19041.0\UAP.props
rem dir c:\"Program Files (x86)"\"Windows Kits"\10\include\10.0.19041.0\include\shared\sdkddkver.h
dir %IC35PATH%\classlib\include
PWD
 msbuild /m /p:Configuration=%1 /p:Machine=%2 %3
