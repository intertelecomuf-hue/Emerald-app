from typing import List
from fastapi import APIRouter, Depends, HTTPException, status
from sqlalchemy.ext.asyncio import AsyncSession

from ..database import get_db
from ..models.user import User
from ..schemas.card import CardCreate, CardResponse, CardTopUp
from ..services.auth import get_current_user
from ..services import card_service

router = APIRouter(prefix="/api/cards", tags=["Cards"])


@router.post("/", response_model=CardResponse, status_code=status.HTTP_201_CREATED)
async def create_card(
    card_data: CardCreate,
    current_user: User = Depends(get_current_user),
    db: AsyncSession = Depends(get_db),
):
    card = await card_service.create_card(db, current_user, card_data)
    return CardResponse.model_validate(card)


@router.get("/", response_model=List[CardResponse])
async def list_cards(
    current_user: User = Depends(get_current_user),
    db: AsyncSession = Depends(get_db),
):
    cards = await card_service.get_user_cards(db, current_user)
    return [CardResponse.model_validate(c) for c in cards]


@router.get("/{card_id}", response_model=CardResponse)
async def get_card(
    card_id: str,
    current_user: User = Depends(get_current_user),
    db: AsyncSession = Depends(get_db),
):
    card = await card_service.get_card_by_id(db, card_id, current_user)
    if not card:
        raise HTTPException(
            status_code=status.HTTP_404_NOT_FOUND,
            detail="Card not found",
        )
    return CardResponse.model_validate(card)


@router.post("/{card_id}/topup", response_model=CardResponse)
async def top_up_card(
    card_id: str,
    topup_data: CardTopUp,
    current_user: User = Depends(get_current_user),
    db: AsyncSession = Depends(get_db),
):
    card = await card_service.get_card_by_id(db, card_id, current_user)
    if not card:
        raise HTTPException(
            status_code=status.HTTP_404_NOT_FOUND,
            detail="Card not found",
        )
    if not card.is_active:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail="Card is deactivated",
        )
    if card.is_frozen:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail="Card is frozen",
        )
    if topup_data.amount <= 0:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail="Amount must be positive",
        )

    card = await card_service.top_up_card(db, card, topup_data.amount)
    return CardResponse.model_validate(card)


@router.post("/{card_id}/freeze", response_model=CardResponse)
async def toggle_freeze_card(
    card_id: str,
    current_user: User = Depends(get_current_user),
    db: AsyncSession = Depends(get_db),
):
    card = await card_service.get_card_by_id(db, card_id, current_user)
    if not card:
        raise HTTPException(
            status_code=status.HTTP_404_NOT_FOUND,
            detail="Card not found",
        )
    if not card.is_active:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail="Card is deactivated",
        )

    card = await card_service.toggle_freeze_card(db, card)
    return CardResponse.model_validate(card)


@router.delete("/{card_id}", response_model=CardResponse)
async def deactivate_card(
    card_id: str,
    current_user: User = Depends(get_current_user),
    db: AsyncSession = Depends(get_db),
):
    card = await card_service.get_card_by_id(db, card_id, current_user)
    if not card:
        raise HTTPException(
            status_code=status.HTTP_404_NOT_FOUND,
            detail="Card not found",
        )

    card = await card_service.deactivate_card(db, card)
    return CardResponse.model_validate(card)
