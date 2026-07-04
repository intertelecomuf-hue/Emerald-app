# run-server.ps1
# Запускає установку залежностей і старт сервера у папці проекту.
# Виконайте цей скрипт у папці c:\Users\Cibermag\Desktop\Emerald або вкажіть шлях.

param(
  [string]$ProjectPath = "$(Split-Path -Parent $MyInvocation.MyCommand.Definition)"
)

Set-Location -Path $ProjectPath

Write-Host "Перевірка Node.js..."
$node = Get-Command node -ErrorAction SilentlyContinue
if (-not $node) {
  Write-Warning "Node.js не знайдено. Запустіть install-node.ps1 або встановіть Node.js вручну: https://nodejs.org"
  exit 1
}

Write-Host "Запускаю npm install..."
npm install
if ($LASTEXITCODE -ne 0) {
  Write-Warning "npm install завершився з помилкою ($LASTEXITCODE). Перевірте помилки вище." 
  exit $LASTEXITCODE
}

Write-Host "Запускаю сервер... (npm start)"
Start-Process -NoNewWindow -FilePath npm -ArgumentList 'start' -WorkingDirectory $ProjectPath
Write-Host "Сервер має працювати. Відкрийте http://localhost:3000" 
