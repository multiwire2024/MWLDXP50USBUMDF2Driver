param (
    [string] $config = "Debug",
    [string] $arch = "x64",
    [string] $solution = "./MWL DXP50 USB UMDF 2 Driver.sln",
    [string] $artifact_dir = "."
)
Write-Host "Artifact Directory = $artifact_dir"

$signing_cert="$artifact_dir/../MWLDXP50USBUMDF2Driver.cer"

$Preferred_VS_Version=2019

try {
    $f = gci ${env:ProgramFiles}/"Microsoft Visual Studio/$Preferred_VS_Version/*" -ErrorAction Stop
}
catch [System.Management.Automation.ItemNotFoundException]   {
    try {
        $f = gci ${env:ProgramFiles(x86)}/"Microsoft Visual Studio/$Preferred_VS_Version/*"
    }
    catch {
        Write-Output "no VS$Preferred_VS_Version installed"
    }
}

$v_path = "";
foreach ($vs in $f)  {
    if ($vs.Name -eq "Enterprise") {
        $v_path = $vs;
        break;
    }
    elseif ($vs.Name -eq "Professional") {
        $v_path = $vs;
        break;
    }
    elseif ($vs.Name -eq "Community") {
        $v_path = $vs;
        break;
    }
    else {
        Write-Host "VS$Preferred_VS_Version not installed"
    }
}

$vs_root = ${v_path}.FullName
Write-Output "$vs_root"

$env:DevEnvDir="${vs_root}\Common7\IDE\"
$env:ExtensionSdkDir="C:\Program Files (x86)\Microsoft SDKs\Windows Kits\10\ExtensionSDKs"
$env:Framework40Version="v4.0"
$env:FrameworkDir="C:\WINDOWS\Microsoft.NET\Framework\"
$env:FrameworkDIR32="C:\WINDOWS\Microsoft.NET\Framework\"
$env:FrameworkVersion="v4.0.30319"
$env:FrameworkVersion32="v4.0.30319"
$env:IFCPATH="${vs_root}\VC\Tools\MSVC\14.29.30133\ifc\x86"
$env:INCLUDE="${vs_root}\VC\Tools\MSVC\14.29.30133\ATLMFC\include;${vs_root}\VC\Tools\MSVC\14.29.30133\include;C:\Program Files (x86)\Windows Kits\NETFXSDK\4.8\include\um;C:\Program Files (x86)\Windows Kits\10\include\10.0.19041.0\ucrt;C:\Program Files (x86)\Windows Kits\10\include\10.0.19041.0\shared;C:\Program Files (x86)\Windows Kits\10\include\10.0.19041.0\km;C:\Program Files (x86)\Windows Kits\10\include\10.0.19041.0\km;C:\Program Files (x86)\Windows Kits\10\include\10.0.19041.0\winrt;C:\Program Files (x86)\Windows Kits\10\include\10.0.19041.0\cppwinrt"
$env:LIB="${vs_root}\VC\Tools\MSVC\14.29.30133\ATLMFC\lib\x64;${vs_root}\VC\Tools\MSVC\14.29.30133\lib\x64;C:\Program Files (x86)\Windows Kits\NETFXSDK\4.8\lib\um\x64;C:\Program Files (x86)\Windows Kits\10\lib\10.0.19041.0\ucrt\x64;C:\Program Files (x86)\Windows Kits\10\lib\10.0.19041.0\um\x64"
$env:LIBPATH="${vs_root}\VC\Tools\MSVC\14.29.30133\ATLMFC\lib\x64;${vs_root}\VC\Tools\MSVC\14.29.30133\lib\x64;${vs_root}\VC\Tools\MSVC\14.29.30133\lib\x64\store\references;C:\Program Files (x86)\Windows Kits\10\UnionMetadata\10.0.19041.0;C:\Program Files (x86)\Windows Kits\10\References\10.0.19041.0;C:\WINDOWS\Microsoft.NET\Framework\v4.0.30319"
$env:MSBuildLoadMicrosoftTargetsReadOnly="true"
$env:NETFXSDKDir="C:\Program Files (x86)\Windows Kits\NETFXSDK\4.8\"
$env:PkgDefApplicationConfigFile="$env:HOME\AppData\Local\Microsoft\VisualStudio\16.0_c2056b1af\devenv.exe.config"
$env:UCRTVersion="10.0.19041.0"
$env:UniversalCRTSdkDir="C:\Program Files (x86)\Windows Kits\10\"
$env:VCIDEInstallDir="${vs_root}\Common7\IDE\VC\"
$env:VCINSTALLDIR="${vs_root}\VC\"
$env:VCToolsInstallDir="${vs_root}\VC\Tools\MSVC\14.29.30133\"
$env:VCToolsRedistDir="${vs_root}\VC\Redist\MSVC\14.29.30133\"
$env:VCToolsVersion="14.29.30133"
$env:VisualStudioDir="$env:HOME\OneDrive\Documents\Visual Studio 2019"
$env:VisualStudioEdition="Microsoft Visual Studio Enterprise 2019"
$env:VisualStudioVersion="16.0"
$env:VS140COMNTOOLS="C:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\Tools\"
$env:VS160COMNTOOLS="${vs_root}\Common7\Tools\"
$env:VSAPPIDDIR="${vs_root}\Common7\IDE\"
$env:VSAPPIDNAME="devenv.exe"
$env:VSCMD_ARG_app_plat="Desktop"
$env:VSCMD_ARG_HOST_ARCH="x64"
$env:VSCMD_ARG_TGT_ARCH="x64"
$env:VSCMD_VER="16.11.34"
$env:VSINSTALLDIR="${vs_root}\"
$env:VSLANG=1033
$env:VSSKUEDITION="Enterprise"
$env:WindowsLibPath="C:\Program Files (x86)\Windows Kits\10\UnionMetadata\10.0.19041.0;C:\Program Files (x86)\Windows Kits\10\References\10.0.19041.0"
$env:WindowsSdkBinPath="C:\Program Files (x86)\Windows Kits\10\bin\"
$env:WindowsSdkDir="C:\Program Files (x86)\Windows Kits\10\"
$env:WindowsSDKLibVersion="10.0.19041.0\"
$env:WindowsSdkVerBinPath="C:\Program Files (x86)\Windows Kits\10\bin\10.0.19041.0\"
$env:WindowsSDKVersion="10.0.19041.0\"
$env:WindowsSDK_ExecutablePath_x64="C:\Program Files (x86)\Microsoft SDKs\Windows\v10.0A\bin\NETFX 4.8 Tools\x64\"
$env:WindowsSDK_ExecutablePath_x86="C:\Program Files (x86)\Microsoft SDKs\Windows\v10.0A\bin\NETFX 4.8 Tools\"
$env:__devinit_path="${vs_root}\Common7\Tools\devinit\devinit.exe"
$env:__DOTNET_ADD_32BIT=1
$env:__DOTNET_PREFERRED_BITNESS=32
$env:__VSCMD_PREINIT_PATH="C:\WINDOWS\system32;C:\WINDOWS;C:\WINDOWS\System32\Wbem;C:\WINDOWS\System32\WindowsPowerShell\v1.0\;C:\Program Files\Microsoft SQL Server\130\Tools\Binn\;C:\Program Files\Git\usr\bin;C:\Program Files\Microsoft SQL Server\150\Tools\Binn\;C:\Program Files\dotnet\;C:\Program Files (x86)\Windows Kits\10\Windows Performance Toolkit\;C:\Users\user\AppData\Local\Microsoft\WindowsApps;C:\Users\user\.dotnet\tools"
$env:__VSCMD_script_err_count="0"



$env:ComSpec="C:\WINDOWS\system32\cmd.exe"
$env:DriverData="C:\Windows\System32\Drivers\DriverData"
$env:IC35PATH="$env:HOME\Documents\IC Imaging Control 3.5\"
$env:IGCCSVC_DB="AQAAANCMnd8BFdERjHoAwE/Cl+sBAAAAdjeN8bkaZ0mx1K3z1uksOgQAAAACAAAAAAAQZgAAAAEAACAAAABd3JDZ6lLR5YMUPIbR6gpK5SZFhS5nmqpq3aq90o/1mgAAAAAOgAAAAAIAACAAAAD56XB/x94lyujmSYNPuSPgzZjuG20jMYYlNrbq3JU8CGAAAADUrHu5233VoJBtVErG/cWW+elZCmrkGYY66gjXWTerBpBr7mLtG/qCC+zdSHZscRLn83NSKlifMfsPNkv+RUUgtFNAFJzNvKmGnSmT4uoh/z51DeUXxaSQkCnmUPwiY/RAAAAAMpXC23fyBGZOqWcNRjveKOtE55lUaE9rhdpM7RjIaPLPp2cL2bxPQWNYGAkoIonqjg4a0pBMqHAGsNYBSkZS1w=="
$env:LOGONSERVER="\\MINI2"
$env:NUGETEXETOOLPATH="c:\program files\nuget\nuget.exe"
$env:OS="Windows_NT"
$env:Path="C:\WINDOWS\system32;C:\WINDOWS;C:\WINDOWS\System32\Wbem;C:\WINDOWS\System32\WindowsPowerShell\v1.0\;C:\WINDOWS\System32\OpenSSH\;C:\Program Files\Microsoft SQL Server\150\Tools\Binn\;C:\Program Files\Microsoft SQL Server\Client SDK\ODBC\170\Tools\Binn\;C:\Mingw64\bin;C:\MinGW\bin;C:\Program Files\Git\cmd;C:\Program Files (x86)\Windows Kits\10\Windows Performance Toolkit\;C:\Program Files\Docker\Docker\resources\bin;C:\Program Files\dotnet\;C:\Program Files\PowerShell\7\;$env:HOME/AppData\Local\Microsoft\WindowsApps;C:\Program Files\Vim\vim91;C:\Program Files\Git\usr\bin;C:\Program Files\Git\bin;$env:HOME\Documents\IC Imaging Control 3.5\win32\debug;$env:HOME\.dotnet\tools;c:\Program Files\Nuget;"
$env:PATHEXT=".COM;.EXE;.BAT;.CMD;.VBS;.VBE;.JS;.JSE;.WSF;.WSH;.MSC;.CPL"
$env:POWERSHELL_DISTRIBUTION_CHANNEL="MSI:Windows 10 Pro"
$env:PROCESSOR_ARCHITECTURE="AMD64"
$env:PROCESSOR_IDENTIFIER="Intel64 Family 6 Model 154 Stepping 4, GenuineIntel"
$env:PROCESSOR_LEVEL="6"
$env:PUBLIC="C:\Users\Public"
$env:SystemDrive="C:"
$env:SystemRoot="C:\WINDOWS"
$env:windir="C:\WINDOWS"
$env:WSLENV="WT_SESSION:WT_PROFILE_ID:"
$env:WT_PROFILE_ID="{61c54bbd-c2c6-5271-96e7-009a87ff44bf}"
$env:WT_SESSION="1b6be7de-2669-4c05-87c0-0165adfef6bd"
$env:ZES_ENABLE_SYSMAN=1
$env:PATH="$env:PATH;${vs_root}\msbuild\current\bin\amd64"


(gci -recurse env:) | foreach-object { Write-Host  ([string]::Format('{0}={1}', ($_.name), $_.value)) >>$env_file }

Write-Host "$vs_root $Solution $config $arch"
&"$vs_root\MSBuild\Current\bin\amd64\msbuild.exe" /m -p:Configuration=$config -p:Platform=$arch $solution

Write-Host "Write Certificate to artifact directory "$artifact_dir if exists
if ([System.IO.File]::Exists($signing_cert)) {
    copy $signing_cert "$artifact_dir"
}
else {
    # For release signing we don't sign and don't declare a certificate .It must be done manually"
    Write-Host "signing cert $signing_cert not found, skipping"
}
