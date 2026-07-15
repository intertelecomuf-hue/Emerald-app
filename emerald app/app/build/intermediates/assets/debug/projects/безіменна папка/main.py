from dotenv import load_dotenv
import os
import random
import logging
from datetime import datetime

from aiogram import Bot, Dispatcher, types
from aiogram.filters import Command
from aiogram.enums import ParseMode
from aiogram.client.default import DefaultBotProperties
from aiogram.types import ReactionTypeEmoji

log_file = f"bot_log_{datetime.now().strftime('%Y%m%d_%H%M%S')}.txt"
logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s %(levelname)s %(message)s",
    handlers=[
        logging.FileHandler(log_file, encoding="utf-8"),
        logging.StreamHandler(),
    ],
)
logger = logging.getLogger(__name__)

load_dotenv()
BOT_TOKEN = os.getenv("BOT_TOKEN")

if not BOT_TOKEN:
    logger.error("Не задано BOT_TOKEN у змінних середовища")
    raise RuntimeError("Не задано BOT_TOKEN у змінних середовища")

logger.info("Завантажено токен: %s", BOT_TOKEN[:10] + "...")

bot = Bot(
    token=BOT_TOKEN,
    default=DefaultBotProperties(parse_mode=ParseMode.HTML),
)
dp = Dispatcher()

SITE_LINK = "https://fanciful-belekoy-867894.netlify.app/"
user_message_counts = {}
user_next_reaction = {}


async def maybe_react(message: types.Message):
    user_id = message.from_user.id
    count = user_message_counts.get(user_id, 0) + 1
    user_message_counts[user_id] = count
    next_at = user_next_reaction.get(user_id)

    if next_at is None:
        next_at = random.randint(2, 7)
        user_next_reaction[user_id] = next_at

    if count >= next_at:
        try:
            await message.react([ReactionTypeEmoji(emoji="👍")])
            logger.info("Поставив реакцію 👍 для %s на повідомленні %d", user_id, count)
        except Exception as e:
            logger.warning("Не вдалося поставити реакцію для %s: %s", user_id, e)
        user_message_counts[user_id] = 0
        user_next_reaction[user_id] = random.randint(2, 7)


@dp.message(Command("start", "старт"))
async def cmd_start(message: types.Message):
    logger.info("Отримано /start від %s", message.from_user.id)
    await message.answer(
        "Вітаю! Я бот підтримки Emerald.\n\n"
        "Що я вмію:\n"
        "- Відповідати на ваші запитання\n"
        "- Ділитися інформацією про проєкти\n"
        "- Направляти вас на офіційний сайт\n\n"
        "Просто пишіть, що вас цікавить, і я допоможу!"
    )
    logger.info("Відправлено відповідь на /start")
    await maybe_react(message)


@dp.message(lambda m: any(word in (m.text or "").lower() for word in ["сайт", "сторінка", "веб", "website", "site", "emerald.inc", "emerald inc"]))
async def handle_site(message: types.Message):
    logger.info("Запит сайту від %s", message.from_user.id)
    await message.answer(
        "Ось посилання на офіційний сайт:\n"
        f"<a href=\"{SITE_LINK}\">Відкрити Emerald.inc</a>"
    )
    logger.info("Відправлено посилання на сайт")
    await maybe_react(message)


@dp.message(lambda m: any(word in (m.text or "").lower() for word in ["проєкт", "проект", "що робите", "що це", "що таке", "про що"]))
async def handle_projects(message: types.Message):
    logger.info("Запит про проєкти від %s", message.from_user.id)
    await message.answer(
        "Emerald — це цифровий простір, де створюються різноманітні проєкти: "
        "від вебзастосунків, ігор та фінансових сервісів до IoT-рішень. "
        "Ми поєднуємо креатив, технології та практичність у кожному проєкті."
    )
    logger.info("Відправлено інформацію про проєкти")
    await maybe_react(message)


@dp.message(lambda m: any(word in (m.text or "").lower() for word in ["ігра", "ігри", "гра", "game", "house flopper", "suslya", "груш", "чіча"]))
async def handle_games(message: types.Message):
    logger.info("Запит про ігри від %s", message.from_user.id)
    await message.answer(
        "У нас є кілька проєктів-ігор:\n\n"
        "🎮 e-suslya — 3D хоррор\n"
        "Імерсивна 3D-гра жахів у браузері на Three.js + PeerJS. "
        "Кооператив для двох гравців через P2P, 5 рівнів + безкінечний режим.\n"
        "Відкрити: https://dynamic-frangipane-bf83da.netlify.app/\n\n"
        "🏚️ House Flopper — Реставрація будинків\n"
        "Гра про реставрацію будинків.\n"
        "Відкрити: https://friendly-beijinho-831384.netlify.app/\n\n"
        "🎮 грушик — Ремонт квартири\n"
        "Клікер-гра про ремонт квартири.\n"
        "Відкрити: https://meek-florentine-6de021.netlify.app/\n\n"
        "🏡 чіча — Домик на дереві\n"
        "Гра про побутову дереву.\n"
        "Відкрити: https://statuesque-scone-d7a2bb.netlify.app/\n\n"
        "Більше деталей на офіційному сайті."
    )
    logger.info("Відправлено інформацію про ігри")
    await maybe_react(message)


@dp.message(lambda m: any(word in (m.text or "").lower() for word in ["epay", "платіж", "платеж", "фінанс", "pay", "оплата", "кошти"]))
async def handle_epay(message: types.Message):
    logger.info("Запит про epay від %s", message.from_user.id)
    await message.answer(
        "Epay — це фінансовий/платіжний сервіс всередині екосистеми Emerald. "
        "Його деталі можна переглянути на офіційному сайті."
    )
    logger.info("Відправлено інформацію про epay")
    await maybe_react(message)


@dp.message(lambda m: any(word in (m.text or "").lower() for word in ["телеграм", "бот", "telegram", "бот"]))
async def handle_bot(message: types.Message):
    logger.info("Запит про бота від %s", message.from_user.id)
    await message.answer(
        "Я — цей самий бот підтримки Emerald 😊\n"
        "Можу розповісти про проєкти, показати сайт або відповісти на інші запитання."
    )
    logger.info("Відправлено інформацію про бота")
    await maybe_react(message)


@dp.message(lambda m: any(word in (m.text or "").lower() for word in ["привіт", "хай", "добрий", "доброго", "вітаю", "hello", "hi"]))
async def handle_greeting(message: types.Message):
    logger.info("Привітання від %s", message.from_user.id)
    await message.answer(
        "Привіт! Я бот підтримки Emerald.\n"
        "Пиши, що тебе цікавить — про проєкти, ігри, сайт чи щось інше."
    )
    logger.info("Відправлено привітання")
    await maybe_react(message)


@dp.message()
async def fallback(message: types.Message):
    logger.info("Нерозпізнане повідомлення від %s: %s", message.from_user.id, message.text)
    await message.answer(
        "Я не впевнений, що зрозумів запитання.\n"
        "Спробуй перефразувати або напиши:\n"
        "- «сайт» — покажу посилання\n"
        "- «проєкти» — розповім про Emerald\n"
        "- «ігри» — розповім про ігрові проєкти"
    )
    logger.info("Відправлено fallback")
    await maybe_react(message)


async def main():
    logger.info("Запускаю polling...")
    await dp.start_polling(bot)


if __name__ == "__main__":
    import asyncio
    logger.info("Старт бота")
    asyncio.run(main())
