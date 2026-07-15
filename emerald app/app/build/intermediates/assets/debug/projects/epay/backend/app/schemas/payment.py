from datetime import datetime
from decimal import Decimal
from typing import Optional
from pydantic import BaseModel


class PaymentCreate(BaseModel):
    amount: Decimal
    currency: str = "USD"
    description: Optional[str] = None


class PaymentResponse(BaseModel):
    id: str
    user_id: str
    amount: Decimal
    currency: str
    status: str
    description: Optional[str] = None
    stripe_payment_intent_id: Optional[str] = None
    stripe_payment_method: Optional[str] = None
    created_at: datetime
    updated_at: datetime

    class Config:
        from_attributes = True


class PaymentIntentResponse(BaseModel):
    client_secret: str
    payment_id: str
    amount: Decimal
    currency: str
