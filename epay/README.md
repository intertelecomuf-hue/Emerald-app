# 💎 Emerald Pay (ePay)

Сучасна платіжна система з веб-інтерфейсом.

## 🚀 Технології

- **Backend**: Python + FastAPI + SQLAlchemy (async)
- **Frontend**: HTML + CSS + JavaScript (vanilla)
- **Database**: SQLite (легко мігрується на PostgreSQL)
- **Auth**: JWT (python-jose)
- **Payments**: Stripe API (опціонально)

## 📋 Функціонал

- ✅ Реєстрація та авторизація користувачів
- ✅ Створення платежів
- ✅ Історія транзакцій
- ✅ Адмін-панель (управління платежами)
- ✅ Статистика на дашборді
- ✅ Інтеграція з Stripe (за наявності ключів)

## 🏃 Запуск

### 1. Встановлення залежностей

```bash
cd backend
pip3 install fastapi uvicorn sqlalchemy aiosqlite python-jose[cryptography] passlib[bcrypt] python-multipart stripe pydantic greenlet "bcrypt==4.0.1"
```

### 2. Ініціалізація бази даних

```bash
cd backend
python3 seed.py
```

### 3. Запуск сервера

```bash
cd backend
python3 run.py
```

Сервер буде доступний на `http://localhost:8000`

### 4. Відкрити фронтенд

Відкрийте `frontend/index.html` у браузері.

## 🔑 Тестові користувачі

| Email | Password | Role |
|-------|----------|------|
| admin@epay.com | admin123 | Admin |
| user@epay.com | user123 | User |

## 📡 API Endpoints

### Authentication
- `POST /api/auth/register` - Реєстрація
- `POST /api/auth/login` - Вхід
- `GET /api/auth/me` - Поточний користувач

### Payments
- `POST /api/payments/` - Створити платіж
- `GET /api/payments/` - Список платежів
- `GET /api/payments/{id}` - Деталі платежу

### Admin
- `GET /api/admin/payments` - Всі платежі
- `GET /api/admin/payments/{id}` - Деталі платежу
- `POST /api/admin/payments/{id}/status` - Змінити статус
- `GET /api/admin/users` - Список користувачів

### Health
- `GET /api/health` - Перевірка статусу

## 🐳 Docker (опціонально)

```bash
# Для PostgreSQL замість SQLite:
docker run -d --name epay-db -e POSTGRES_PASSWORD=epay -e POSTGRES_DB=epay -p 5432:5432 postgres:16
```

## 🔧 Stripe Integration

Для використання Stripe встановіть змінні оточення:
```bash
export STRIPE_SECRET_KEY=sk_test_...
export STRIPE_PUBLISHABLE_KEY=pk_test_...
```

Без ключів Stripe платежі автоматично завершуються (режим розробки).
