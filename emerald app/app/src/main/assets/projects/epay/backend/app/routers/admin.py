from typing import List
from fastapi import APIRouter, Depends, HTTPException, status
from sqlalchemy.ext.asyncio import AsyncSession
from sqlalchemy import select

from ..database import get_db
from ..models.user import User
from ..schemas.payment import PaymentResponse
from ..schemas.user import UserResponse
from ..services.auth import require_admin
from ..services import payment_service
from ..models.user import User as UserModel

router = APIRouter(prefix="/api/admin", tags=["Admin"])


@router.get("/payments", response_model=List[PaymentResponse])
async def admin_list_payments(
    skip: int = 0,
    limit: int = 50,
    admin: User = Depends(require_admin),
    db: AsyncSession = Depends(get_db),
):
    payments = await payment_service.get_all_payments(db, skip, limit)
    return [PaymentResponse.model_validate(p) for p in payments]


@router.get("/payments/{payment_id}", response_model=PaymentResponse)
async def admin_get_payment(
    payment_id: str,
    admin: User = Depends(require_admin),
    db: AsyncSession = Depends(get_db),
):
    payment = await payment_service.get_payment_by_id(db, payment_id)
    if not payment:
        raise HTTPException(
            status_code=status.HTTP_404_NOT_FOUND,
            detail="Payment not found",
        )
    return PaymentResponse.model_validate(payment)


@router.post("/payments/{payment_id}/status")
async def admin_update_payment_status(
    payment_id: str,
    status: str,
    admin: User = Depends(require_admin),
    db: AsyncSession = Depends(get_db),
):
    valid_statuses = {"pending", "completed", "failed", "refunded"}
    if status not in valid_statuses:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=f"Invalid status. Must be one of: {valid_statuses}",
        )

    payment = await payment_service.update_payment_status(db, payment_id, status)
    if not payment:
        raise HTTPException(
            status_code=status.HTTP_404_NOT_FOUND,
            detail="Payment not found",
        )
    return {"message": f"Payment status updated to {status}"}


@router.get("/users", response_model=List[UserResponse])
async def admin_list_users(
    skip: int = 0,
    limit: int = 50,
    admin: User = Depends(require_admin),
    db: AsyncSession = Depends(get_db),
):
    result = await db.execute(
        select(UserModel).offset(skip).limit(limit)
    )
    users = result.scalars().all()
    return [UserResponse.model_validate(u) for u in users]
