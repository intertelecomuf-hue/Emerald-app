import uuid
from datetime import datetime
from decimal import Decimal
from sqlalchemy import String, Numeric, DateTime, ForeignKey, Boolean, func
from sqlalchemy.orm import Mapped, mapped_column, relationship
from sqlalchemy.dialects.sqlite import TEXT

from ..database import Base


class Card(Base):
    __tablename__ = "cards"

    id: Mapped[str] = mapped_column(
        TEXT, primary_key=True, default=lambda: str(uuid.uuid4())
    )
    user_id: Mapped[str] = mapped_column(
        TEXT, ForeignKey("users.id"), nullable=False, index=True
    )
    # Emulated card details
    card_number: Mapped[str] = mapped_column(String(19), nullable=False)  # XXXX-XXXX-XXXX-XXXX
    cardholder_name: Mapped[str] = mapped_column(String(255), nullable=False)
    expiry_month: Mapped[int] = mapped_column(nullable=False)
    expiry_year: Mapped[int] = mapped_column(nullable=False)
    cvv: Mapped[str] = mapped_column(String(4), nullable=False)
    # Virtual card info
    brand: Mapped[str] = mapped_column(String(50), default="Visa")  # Visa, Mastercard
    type: Mapped[str] = mapped_column(String(20), default="virtual")  # virtual, physical
    balance: Mapped[Decimal] = mapped_column(Numeric(10, 2), default=Decimal("0.00"))
    currency: Mapped[str] = mapped_column(String(3), default="USD")
    is_active: Mapped[bool] = mapped_column(Boolean, default=True)
    is_frozen: Mapped[bool] = mapped_column(Boolean, default=False)
    created_at: Mapped[datetime] = mapped_column(
        DateTime(timezone=True), server_default=func.now()
    )
    updated_at: Mapped[datetime] = mapped_column(
        DateTime(timezone=True), server_default=func.now(), onupdate=func.now()
    )

    # Relationships
    user = relationship("User", back_populates="cards")
    transactions = relationship("Transaction", back_populates="card", cascade="all, delete-orphan")
