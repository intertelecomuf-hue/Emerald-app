#!/bin/bash
# Emerald Explorer — Start Script
# Запускає сервер + проксі і відкриває браузер

cd "$(dirname "$0")"

echo "🌐 Запуск Emerald Explorer..."
echo ""

# Запускаємо сервер в фоновому режимі
python3 proxy-server.py &
PROXY_PID=$!

# Чекаємо поки сервер запуститься
sleep 2

# Відкриваємо браузер на localhost:3456
open http://localhost:3456

echo ""
echo "✅ Emerald Explorer запущено!"
echo "   Відкрито: http://localhost:3456"
echo "   Натисніть Ctrl+C для зупинки"

# Чекаємо натискання Ctrl+C
trap "kill $PROXY_PID 2>/dev/null; echo 'Зупинено.'; exit 0" INT
wait $PROXY_PID
