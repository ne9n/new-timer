$ErrorActionPreference = "Stop"

$compiler = Get-Command g++ -ErrorAction SilentlyContinue
if (-not $compiler) {
    throw "g++ is required to run the host tests. Install MinGW-w64 or another distribution that provides g++."
}

$testDirectory = Split-Path -Parent $MyInvocation.MyCommand.Path
$sketchDirectory = Split-Path -Parent $testDirectory
$stateMachineOutputPath = Join-Path $env:TEMP "new-timer-state-machine-tests-$PID.exe"
$gyroOutputPath = Join-Path $env:TEMP "new-timer-gyro-tests-$PID.exe"
$fakeDirectory = Join-Path $testDirectory "fakes"
$stateMachineTestPath = Join-Path $testDirectory "state_machine_test.cpp"
$gyroTestPath = Join-Path $testDirectory "gyro_test.cpp"
$stateMachinePath = Join-Path $sketchDirectory "state_machine.cpp"
$gyroPath = Join-Path $sketchDirectory "gyro.cpp"

try {
    & $compiler.Source -std=c++11 -Wall -Wextra `
        "-I$fakeDirectory" "-I$sketchDirectory" `
        $stateMachinePath $stateMachineTestPath -o $stateMachineOutputPath
    if ($LASTEXITCODE -ne 0) {
        throw "State-machine test compilation failed with exit code $LASTEXITCODE."
    }

    & $stateMachineOutputPath
    if ($LASTEXITCODE -ne 0) {
        throw "State-machine tests failed with exit code $LASTEXITCODE."
    }

    & $compiler.Source -std=c++11 -Wall -Wextra `
        "-I$fakeDirectory" "-I$sketchDirectory" `
        $gyroPath $gyroTestPath -o $gyroOutputPath
    if ($LASTEXITCODE -ne 0) {
        throw "Gyro test compilation failed with exit code $LASTEXITCODE."
    }

    & $gyroOutputPath
    if ($LASTEXITCODE -ne 0) {
        throw "Gyro tests failed with exit code $LASTEXITCODE."
    }
}
finally {
    foreach ($outputPath in @($stateMachineOutputPath, $gyroOutputPath)) {
        if (Test-Path $outputPath) {
            Remove-Item -LiteralPath $outputPath
        }
    }
}
