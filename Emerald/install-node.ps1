# install-node.ps1
# Спробує встановити Node.js через winget (Windows 10/11). Запустіть PowerShell як Адміністратор.
# Якщо winget відсутній, завантажте інсталятор з https://nodejs.org і запустіть вручну.

Write-Host "Перевірка наявності node..."
$node = Get-Command node -ErrorAction SilentlyContinue
if ($node) {
  Write-Host "Node.js вже встановлено:" $node.Source
  exit 0
}

Write-Host "Спроба встановити Node.js (LTS) через winget..."
try {
  winget install --id OpenJS.NodeJS.LTS -e --silent
} catch {
  Write-Warning "winget не вдалося запустити або встановлення не вдалось. Спробуйте встановити Node.js вручну з https://nodejs.org"
  exit 1
}

# Перевірка після встановлення
Start-Sleep -Seconds 3
$node = Get-Command node -ErrorAction SilentlyContinue
if ($node) { Write-Host "Node встановлено успішно:" $node.Source } else { Write-Warning "Node не знайдено після встановлення. Перезапустіть PowerShell і перевірте шлях." }
