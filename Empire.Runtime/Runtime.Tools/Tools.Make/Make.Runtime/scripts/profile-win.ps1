# PowerShell parameter completion for xmake
Register-ArgumentCompleter -Native -CommandName xmake -ScriptBlock {
    param($commandName, $wordToComplete, $cursorPosition)
    $complete = "$wordToComplete"
    if (-not $commandName) {
        $complete = $complete + " "
    }
    $oldenv = $env:XMAKE_SKIP_HISTORY
    $oldcolor = $env:XMAKE_COLORTERM
    $env:XMAKE_SKIP_HISTORY = 1
    $env:XMAKE_COLORTERM = "nocolor"
    $results = xmake lua "private.utils.complete" $cursorPosition "nospace-json" "$complete" 2>$null | ConvertFrom-Json | Sort-Object -Property value
    $results | ForEach-Object {
        $hasdesc = [bool] $_.psobject.Properties['description']
        if ($hasdesc) {
            $desc = " - $($_.description)"
        } else {
            $desc = ""
        }
        [System.Management.Automation.CompletionResult]::new($_.value, "$($_.value)$desc", 'ParameterValue', $_.value)
    }
    $env:XMAKE_SKIP_HISTORY = $oldenv
    $env:XMAKE_COLORTERM = $oldcolor
}

# PowerShell parameter completion for xrepo
Register-ArgumentCompleter -Native -CommandName xrepo -ScriptBlock {
    param($commandName, $wordToComplete, $cursorPosition)
    $complete = "$wordToComplete"
    if (-not $commandName) {
        $complete = $complete + " "
    }
    $oldenv = $env:XMAKE_SKIP_HISTORY
    $oldcolor = $env:XMAKE_COLORTERM
    $env:XMAKE_SKIP_HISTORY = 1
    $env:XMAKE_COLORTERM = "nocolor"
    $results = xmake lua "private.xrepo.complete" $cursorPosition "nospace-json" "$complete" 2>$null | ConvertFrom-Json | Sort-Object -Property value
    $results | ForEach-Object {
        $hasdesc = [bool] $_.psobject.Properties['description']
        if ($hasdesc) {
            $desc = " - $($_.description)"
        } else {
            $desc = ""
        }
        [System.Management.Automation.CompletionResult]::new($_.value, "$($_.value)$desc", 'ParameterValue', $_.value)
    }
    $env:XMAKE_SKIP_HISTORY = $oldenv
    $env:XMAKE_COLORTERM = $oldcolor
}