import random
from decimal import Decimal
from typing import List, Optional

from sqlalchemy.ext.asyncio import AsyncSession
from sqlalchemy import select

from ..models.card import Card
from ..models.user import User
from ..schemas.card import CardCreate


def _generate_card_number(brand: str) -> str:
    """Generate a random card number based on brand."""
    if brand.lower() == "mastercard":
        prefix = "5" + str(random.randint(1, 5))
    else:  # Visa
        prefix = "4"

    # Generate remaining 15 digits (or 14 for some Visa)
    remaining = "".join([str(random.randint(0, 9)) for _ in range(15)])
    number = prefix + remaining

    # Format as XXXX-XXXX-XXXX-XXXX
    formatted = "-".join([number[i:i+4] for i in range(0, 16, 4)])
    return formatted


def _generate_cvv() -> str:
    return f"{random.randint(0, 999):03d}"


def _generate_expiry() -> tuple:
    """Generate expiry date ~2 years from now."""
    import datetime
    now = datetime.datetime.now()
    # Add 2-4 years
    year = now.year + random.randint(2, 4)
    month = random.randint(1, 12)
    return month, year


async def create_card(
    db: AsyncSession, user: User, card_data: CardCreate
) -> Card:
    """Create a new virtual card for the user."""
    month, year = _generate_expiry()
    card = Card(
        user_id=user.id,
        card_number=_generate_card_number(card_data.brand),
        cardholder_name=user.full_name or user.email,
        expiry_month=month,
        expiry_year=year,
        cvv=_generate_cvv(),
        brand=card_data.brand,
        type="virtual",
        balance=Decimal("0.00"),
        currency=card_data.currency,
    )
    db.add(card)
    await db.commit()
    await db.refresh(card)
    return card


async def get_user_cards(
    db: AsyncSession, user: User
) -> List[Card]:
    """Get all cards for a user."""
    result = await db.execute(
        select(Card)
        .where(Card.user_id == user.id)
        .order_by(Card.created_at.desc())
    )
    return list(result.scalars().all())


async def get_card_by_id(
    db: AsyncSession, card_id: str, user: Optional[User] = None
) -> Optional[Card]:
    """Get a card by ID, optionally filtered by user."""
    query = select(Card).where(Card.id == card_id)
    if user:
        query = query.where(Card.user_id == user.id)
    result = await db.execute(query)
    return result.scalar_one_or_none()


async def top_up_card(
    db: AsyncSession, card: Card, amount: Decimal
) -> Card:
    """Add funds to a card."""
    card.balance += amount
    await db.commit()
    await db.refresh(card)
    return card


async def toggle_freeze_card(
    db: AsyncSession, card: Card
) -> Card:
    """Toggle freeze status of a card."""
    card.is_frozen = not card.is_frozen
    await db.commit()
    await db.refresh(card)
    return card


async def deactivate_card(
    db: AsyncSession, card: Card
) -> Card:
    """Deactivate a card."""
    card.is_active = False
    await db.commit()
    await db.refresh(card)
    return card
