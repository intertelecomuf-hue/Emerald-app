from decimal import Decimal
from typing import List, Optional
from sqlalchemy import select
from sqlalchemy.ext.asyncio import AsyncSession

from ..models.user import User
from ..models.card import Card
from ..models.transaction import Transaction


async def deposit_to_user(
    db: AsyncSession,
    user: User,
    amount: Decimal,
    description: Optional[str] = None,
) -> Transaction:
    """Deposit money to user's main balance."""
    user.balance += amount

    transaction = Transaction(
        user_id=user.id,
        card_id=None,
        type="deposit",
        amount=amount,
        currency="USD",
        description=description or f"Deposit of ${amount:.2f}",
        status="completed",
    )
    db.add(transaction)
    await db.commit()
    await db.refresh(transaction)
    return transaction


async def credit_salary(
    db: AsyncSession,
    user: User,
    amount: Decimal,
    description: Optional[str] = None,
) -> Transaction:
    """Credit salary to user's main balance."""
    user.balance += amount

    transaction = Transaction(
        user_id=user.id,
        card_id=None,
        type="salary",
        amount=amount,
        currency="USD",
        description=description or f"Salary of ${amount:.2f}",
        status="completed",
    )
    db.add(transaction)
    await db.commit()
    await db.refresh(transaction)
    return transaction


async def transfer_to_card(
    db: AsyncSession,
    user: User,
    card: Card,
    amount: Decimal,
) -> Transaction:
    """Transfer money from user's main balance to a card."""
    if user.balance < amount:
        raise ValueError("Insufficient balance")

    if not card.is_active:
        raise ValueError("Card is not active")

    if card.is_frozen:
        raise ValueError("Card is frozen")

    # Deduct from user balance
    user.balance -= amount

    # Add to card balance
    card.balance += amount

    transaction = Transaction(
        user_id=user.id,
        card_id=card.id,
        type="transfer_to_card",
        amount=amount,
        currency=card.currency,
        description=f"Transfer ${amount:.2f} to card {card.card_number[-4:]}",
        status="completed",
    )
    db.add(transaction)
    await db.commit()
    await db.refresh(transaction)
    return transaction


async def get_user_transactions(
    db: AsyncSession,
    user_id: str,
    limit: int = 50,
    offset: int = 0,
) -> List[Transaction]:
    """Get all transactions for a user."""
    stmt = (
        select(Transaction)
        .where(Transaction.user_id == user_id)
        .order_by(Transaction.created_at.desc())
        .limit(limit)
        .offset(offset)
    )
    result = await db.execute(stmt)
    return list(result.scalars().all())
