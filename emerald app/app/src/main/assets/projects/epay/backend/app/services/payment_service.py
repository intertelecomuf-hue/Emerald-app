from decimal import Decimal
from typing import List, Optional
import stripe
from sqlalchemy.ext.asyncio import AsyncSession
from sqlalchemy import select

from ..config import STRIPE_SECRET_KEY
from ..models.payment import Payment
from ..models.user import User
from ..schemas.payment import PaymentCreate

stripe.api_key = STRIPE_SECRET_KEY


async def create_payment(
    db: AsyncSession, user: User, payment_data: PaymentCreate
) -> Payment:
    """Create a payment record and a Stripe PaymentIntent."""
    payment = Payment(
        user_id=user.id,
        amount=payment_data.amount,
        currency=payment_data.currency.lower(),
        description=payment_data.description,
        status="pending",
    )
    db.add(payment)
    await db.commit()
    await db.refresh(payment)

    # Create Stripe PaymentIntent
    if STRIPE_SECRET_KEY:
        try:
            intent = stripe.PaymentIntent.create(
                amount=int(payment_data.amount * 100),  # cents
                currency=payment_data.currency.lower(),
                metadata={
                    "payment_id": payment.id,
                    "user_id": user.id,
                },
                description=payment_data.description or "",
            )
            payment.stripe_payment_intent_id = intent.id
            payment.stripe_payment_method = intent.payment_method_types[0] if intent.payment_method_types else None
            await db.commit()
            await db.refresh(payment)
        except stripe.error.StripeError as e:
            payment.status = "failed"
            await db.commit()
            raise Exception(f"Stripe error: {str(e)}")
    else:
        # For development without Stripe - auto-complete
        payment.status = "completed"
        await db.commit()
        await db.refresh(payment)

    return payment


async def get_user_payments(
    db: AsyncSession, user: User, skip: int = 0, limit: int = 50
) -> List[Payment]:
    result = await db.execute(
        select(Payment)
        .where(Payment.user_id == user.id)
        .order_by(Payment.created_at.desc())
        .offset(skip)
        .limit(limit)
    )
    return list(result.scalars().all())


async def get_payment_by_id(
    db: AsyncSession, payment_id: str, user: Optional[User] = None
) -> Optional[Payment]:
    query = select(Payment).where(Payment.id == payment_id)
    if user:
        query = query.where(Payment.user_id == user.id)
    result = await db.execute(query)
    return result.scalar_one_or_none()


async def get_all_payments(
    db: AsyncSession, skip: int = 0, limit: int = 50
) -> List[Payment]:
    result = await db.execute(
        select(Payment).order_by(Payment.created_at.desc()).offset(skip).limit(limit)
    )
    return list(result.scalars().all())


async def update_payment_status(
    db: AsyncSession, payment_id: str, status: str
) -> Optional[Payment]:
    payment = await get_payment_by_id(db, payment_id)
    if payment:
        payment.status = status
        await db.commit()
        await db.refresh(payment)
    return payment
