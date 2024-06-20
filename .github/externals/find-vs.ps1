$f = gci ${env:ProgramFiles(x86)}/"Microsoft Visual Studio/20*/*"

$v_path = "";
foreach ($vs in $f)  {
    if ($vs.Name -eq "Enterprise") {
        $v_path = $vs;
        break;
    }
    elseif ($vs.Name -eq "Community") {
        $v_path = $vs;
        break;
    }
    else {
        Write-Host "VS20xx not installed"
    }
}

$vs_path = ${v_path}.FullName
Write-Host ${v_path}.FullName
exit ${v_path}.FullName
