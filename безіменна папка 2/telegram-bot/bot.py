import os
import logging
from dotenv import load_dotenv
from telegram import Update
from telegram.ext import ApplicationBuilder, ContextTypes, MessageHandler, CommandHandler, filters

load_dotenv()

TELEGRAM_TOKEN = os.getenv("TELEGRAM_BOT_TOKEN")

if not TELEGRAM_TOKEN:
    raise RuntimeError("TELEGRAM_BOT_TOKEN не заданий у .env")

logging.basicConfig(level=logging.INFO)

SYSTEM_PROMPT = (
    "Ти — невеличкий, але розумний асистент у Telegram. "
    "Відповідай коротко, з гумором, іноді жартуй. "
    "Не затягуй з відповіддю."
)

async def start_command(update: Update, context: ContextTypes.DEFAULT_TYPE):
    await update.message.reply_text(
        "Привіт! Я маю невеличкий вбудований розум. Пиши що завгодно — постараюсь відповісти."
    )


async def handle_message(update: Update, context: ContextTypes.DEFAULT_TYPE):
    user_text = update.message.text
    chat_id = update.effective_chat.id
    lower = user_text.lower()

    reply = None
    if "привіт" in lower or "hello" in lower or "hi" in lower:
        reply = "О, привіт! Як справи?"
    elif "як справи" in lower:
        reply = "Справи добре, дякую! А в тебе?"
    elif "хто ти" in lower or "що ти" in lower:
        reply = "Я простий бот з невеличким мозгом. Можу підтримати розмову, але не очікуй філософії 😄"
    elif "смішно" in lower or "жарт" in lower:
        reply = "Чому програміст завжди плутає Хелловін з Різдвом? Бо Oct 31 == Dec 25 😄"
    elif "погода" in lower:
        reply = "Я не знаю твоєї погоди, але сподіваюсь, що сонце світить! ☀️"
    elif "допомога" in lower or "help" in lower:
        reply = "Просто напиши мені щось — я постараюсь відповісти. Можна жартувати, говорити про життя, філософувати (трохи)."
    elif "бувай" in lower or "пака" in lower or "пока" in lower:
        reply = "Бувай! Буду тут, якщо знадоблюсь."
    else:
        import random
        responses = [
            "Цікаво... А що ти ще можешь розповісти?",
            "Хм, зрозумів. А в тебе які плани?",
            "Гм, непередбачувано! 😄",
            "Я трохи подумаю... Так, згоден!",
            "Можливо ти правий. А може й ні? 🤔",
            "Це ж можна обговорювати довго!",
            "Чудове питання! Але я трохи простий для такої глибини 😅",
            "Так, так, я тебе слухаю...",
        ]
        reply = random.choice(responses)

    await update.message.reply_text(reply)


def main():
    application = ApplicationBuilder().token(TELEGRAM_TOKEN).build()
    application.add_handler(CommandHandler("start", start_command))
    application.add_handler(MessageHandler(filters.TEXT & ~filters.COMMAND, handle_message))
    application.run_polling()


if __name__ == "__main__":
    main()
