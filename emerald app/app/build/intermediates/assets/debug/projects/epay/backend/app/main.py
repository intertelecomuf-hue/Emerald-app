from contextlib import asynccontextmanager
from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware

from .database import init_db
from .routers import auth, payments, admin, cards, transactions


@asynccontextmanager
async def lifespan(app: FastAPI):
    # Startup
    await init_db()
    yield
    # Shutdown


app = FastAPI(
    title="Emerald Pay (ePay) API",
    description="Payment processing system",
    version="1.0.0",
    lifespan=lifespan,
)

# CORS
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],  # In production, restrict this
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# Routers
app.include_router(auth.router)
app.include_router(payments.router)
app.include_router(cards.router)
app.include_router(transactions.router)
app.include_router(admin.router)


@app.get("/api/health")
async def health_check():
    return {"status": "ok", "service": "Emerald Pay"}
