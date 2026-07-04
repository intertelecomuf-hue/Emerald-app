import uuid
from datetime import datetime
from decimal import Decimal
from sqlalchemy import String, Numeric, DateTime, ForeignKey, Text, func
from sqlalchemy.orm import Mapped, mapped_column, relationship
from sqlalchemy.dialects.sqlite import TEXT

from ..database import Base


class Transaction(Base):
    __tablename__ = "transactions"

    id: Mapped[str] = mapped_column(
        TEXT, primary_key=True, default=lambda: str(uuid.uuid4())
    )
    user_id: Mapped[str] = mapped_column(
        TEXT, ForeignKey("users.id"), nullable=False, index=True
    )
    card_id: Mapped[str] = mapped_column(
        TEXT, ForeignKey("cards.id"), nullable=True, index=True
    )
    type: Mapped[str] = mapped_column(
        String(30), nullable=False, index=True
    )  # deposit, salary, transfer_to_card, transfer_from_card, payment
    amount: Mapped[Decimal] = mapped_column(Numeric(10, 2), nullable=False)
    currency: Mapped[str] = mapped_column(String(3), default="USD")
    description: Mapped[str] = mapped_column(Text, nullable=True)
    status: Mapped[str] = mapped_column(
        String(20), default="completed"
    )  # completed, pending, failed
    created_at: Mapped[datetime] = mapped_column(
        DateTime(timezone=True), server_default=func.now()
    )

    # Relationships
    user = relationship("User", back_populates="transactions")
    card = relationship("Card", back_populates="transactions")
