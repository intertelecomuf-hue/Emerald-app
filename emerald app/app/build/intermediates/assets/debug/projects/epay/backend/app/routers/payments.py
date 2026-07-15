from typing import List
from fastapi import APIRouter, Depends, HTTPException, status
from sqlalchemy.ext.asyncio import AsyncSession

from ..database import get_db
from ..models.user import User
from ..schemas.payment import PaymentCreate, PaymentResponse, PaymentIntentResponse
from ..services.auth import get_current_user
from ..services import payment_service

router = APIRouter(prefix="/api/payments", tags=["Payments"])


@router.post("/", response_model=PaymentResponse, status_code=status.HTTP_201_CREATED)
async def create_payment(
    payment_data: PaymentCreate,
    current_user: User = Depends(get_current_user),
    db: AsyncSession = Depends(get_db),
):
    payment = await payment_service.create_payment(db, current_user, payment_data)
    return PaymentResponse.model_validate(payment)


@router.get("/", response_model=List[PaymentResponse])
async def list_payments(
    skip: int = 0,
    limit: int = 50,
    current_user: User = Depends(get_current_user),
    db: AsyncSession = Depends(get_db),
):
    payments = await payment_service.get_user_payments(db, current_user, skip, limit)
    return [PaymentResponse.model_validate(p) for p in payments]


@router.get("/{payment_id}", response_model=PaymentResponse)
async def get_payment(
    payment_id: str,
    current_user: User = Depends(get_current_user),
    db: AsyncSession = Depends(get_db),
):
    payment = await payment_service.get_payment_by_id(db, payment_id, current_user)
    if not payment:
        raise HTTPException(
            status_code=status.HTTP_404_NOT_FOUND,
            detail="Payment not found",
        )
    return PaymentResponse.model_validate(payment)


@router.post("/{payment_id}/pay", response_model=PaymentIntentResponse)
async def pay_payment(
    payment_id: str,
    current_user: User = Depends(get_current_user),
    db: AsyncSession = Depends(get_db),
):
    """Get Stripe PaymentIntent client secret for frontend to complete payment."""
    payment = await payment_service.get_payment_by_id(db, payment_id, current_user)
    if not payment:
        raise HTTPException(
            status_code=status.HTTP_404_NOT_FOUND,
            detail="Payment not found",
        )

    if payment.status != "pending":
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=f"Payment is already {payment.status}",
        )

    if not payment.stripe_payment_intent_id:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail="No Stripe PaymentIntent created. Create payment first.",
        )

    import stripe
    from ..config import STRIPE_SECRET_KEY
    stripe.api_key = STRIPE_SECRET_KEY

    try:
        intent = stripe.PaymentIntent.retrieve(payment.stripe_payment_intent_id)
        return PaymentIntentResponse(
            client_secret=intent.client_secret,
            payment_id=payment.id,
            amount=payment.amount,
            currency=payment.currency,
        )
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Stripe error: {str(e)}",
        )
