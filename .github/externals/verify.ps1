param (
    [string] $config = "Debug",
    [string] $arch = "x64",
    [string] $artifact_dir = "."
)
Write-Host "Artifact Directory = $artifact_dir"

#
# First, install the driver
#
install_driver() {

   # Find the artifact
   gci -r 
  
   # unzip the artifact
}

run_test(test_script) {
}

install_driver()
run_test("./test.py")
