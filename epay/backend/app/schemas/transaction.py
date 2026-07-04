from datetime import datetime
from decimal import Decimal
from typing import Optional
from pydantic import BaseModel, Field


class TransactionResponse(BaseModel):
    id: str
    user_id: str
    card_id: Optional[str] = None
    type: str  # deposit, salary, transfer_to_card, transfer_from_card, payment
    amount: Decimal
    currency: str
    description: Optional[str] = None
    status: str  # completed, pending, failed
    created_at: datetime

    model_config = {"from_attributes": True}


class DepositRequest(BaseModel):
    amount: Decimal = Field(..., gt=0, description="Amount to deposit")
    description: Optional[str] = None


class SalaryRequest(BaseModel):
    amount: Decimal = Field(..., gt=0, description="Salary amount")
    description: Optional[str] = "Salary payment"


class TransferToCardRequest(BaseModel):
    card_id: str = Field(..., description="Target card ID")
    amount: Decimal = Field(..., gt=0, description="Amount to transfer")
