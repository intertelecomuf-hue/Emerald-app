from datetime import datetime
from decimal import Decimal
from typing import Optional
from pydantic import BaseModel


class CardCreate(BaseModel):
    currency: str = "USD"
    brand: str = "Visa"  # Visa, Mastercard


class CardResponse(BaseModel):
    id: str
    user_id: str
    card_number: str
    cardholder_name: str
    expiry_month: int
    expiry_year: int
    cvv: str
    brand: str
    type: str
    balance: Decimal
    currency: str
    is_active: bool
    is_frozen: bool
    created_at: datetime

    class Config:
        from_attributes = True


class CardTopUp(BaseModel):
    amount: Decimal
