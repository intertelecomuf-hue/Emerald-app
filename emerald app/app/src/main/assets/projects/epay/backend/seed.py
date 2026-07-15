#!/usr/bin/env python3
"""Seed the database with an admin user."""
import asyncio
from app.database import async_session, init_db
from app.models.user import User
from app.services.auth import hash_password
from sqlalchemy import select


async def seed():
    await init_db()
    async with async_session() as db:
        # Check if admin exists
        result = await db.execute(select(User).where(User.email == "admin@epay.com"))
        admin = result.scalar_one_or_none()

        if not admin:
            admin = User(
                email="admin@epay.com",
                hashed_password=hash_password("admin123"),
                full_name="Admin User",
                is_admin=True,
            )
            db.add(admin)
            await db.commit()
            print("✅ Admin user created: admin@epay.com / admin123")
        else:
            print("ℹ️  Admin user already exists")

        # Check if test user exists
        result = await db.execute(select(User).where(User.email == "user@epay.com"))
        user = result.scalar_one_or_none()

        if not user:
            user = User(
                email="user@epay.com",
                hashed_password=hash_password("user123"),
                full_name="Test User",
                is_admin=False,
            )
            db.add(user)
            await db.commit()
            print("✅ Test user created: user@epay.com / user123")
        else:
            print("ℹ️  Test user already exists")


if __name__ == "__main__":
    asyncio.run(seed())
