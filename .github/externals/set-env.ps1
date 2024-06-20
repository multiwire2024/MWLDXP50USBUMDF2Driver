#!pwsh
#[CmdletBinding()]
#param([Parameter(Mandatory=$true, Position=0)]
#      [string]$env_file)
#
$env_file = $env:GITHUB_ENV
echo "" >$env_file
(gci -recurse env:) | foreach-object { Write-Host  ([string]::Format('{0}={1}', ($_.name), $_.value)) >>$env_file }  
