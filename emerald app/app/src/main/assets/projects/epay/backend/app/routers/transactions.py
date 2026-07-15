from decimal import Decimal
from typing import List
from fastapi import APIRouter, Depends, HTTPException, Query
from sqlalchemy import select
from sqlalchemy.ext.asyncio import AsyncSession

from ..database import get_db
from ..models.user import User
from ..models.card import Card
from ..schemas.transaction import (
    TransactionResponse,
    DepositRequest,
    SalaryRequest,
    TransferToCardRequest,
)
from ..services.auth import get_current_user
from ..services import transaction_service

router = APIRouter(prefix="/api/transactions", tags=["transactions"])


@router.get("/", response_model=List[TransactionResponse])
async def list_transactions(
    limit: int = Query(50, ge=1, le=200),
    offset: int = Query(0, ge=0),
    current_user: User = Depends(get_current_user),
    db: AsyncSession = Depends(get_db),
):
    """Get all transactions for the current user."""
    transactions = await transaction_service.get_user_transactions(
        db, current_user.id, limit=limit, offset=offset
    )
    return transactions


@router.post("/deposit", response_model=TransactionResponse)
async def deposit(
    request: DepositRequest,
    current_user: User = Depends(get_current_user),
    db: AsyncSession = Depends(get_db),
):
    """Deposit money to your main balance."""
    transaction = await transaction_service.deposit_to_user(
        db, current_user, request.amount, request.description
    )
    return transaction


@router.post("/salary", response_model=TransactionResponse)
async def salary(
    request: SalaryRequest,
    current_user: User = Depends(get_current_user),
    db: AsyncSession = Depends(get_db),
):
    """Credit salary to your main balance."""
    transaction = await transaction_service.credit_salary(
        db, current_user, request.amount, request.description
    )
    return transaction


@router.post("/transfer-to-card", response_model=TransactionResponse)
async def transfer_to_card(
    request: TransferToCardRequest,
    current_user: User = Depends(get_current_user),
    db: AsyncSession = Depends(get_db),
):
    """Transfer money from main balance to a card."""
    # Verify card exists and belongs to user
    stmt = select(Card).where(
        Card.id == request.card_id,
        Card.user_id == current_user.id,
    )
    result = await db.execute(stmt)
    card = result.scalar_one_or_none()

    if not card:
        raise HTTPException(status_code=404, detail="Card not found")

    try:
        transaction = await transaction_service.transfer_to_card(
            db, current_user, card, request.amount
        )
        return transaction
    except ValueError as e:
        raise HTTPException(status_code=400, detail=str(e))
