#!/usr/bin/env python3
"""HTTP-сервер для перегляду всіх проєктів Emerald Inc."""
import http.server
import socketserver
import os
import sys
import json
import re
import hashlib
import sqlite3
from urllib.parse import quote

PORT = 8080
PROJECTS_DIR = "/Users/fedir/Projects"
DB_PATH = os.path.join(os.path.dirname(os.path.abspath(__file__)), "users.db")


def _init_db():
    """Створює таблицю users, якщо її немає."""
    conn = sqlite3.connect(DB_PATH)
    cur = conn.cursor()
    cur.execute("""
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT UNIQUE NOT NULL,
            password_hash TEXT NOT NULL
        )
    """)
    conn.commit()
    conn.close()


def _hash_password(password):
    return hashlib.sha256(password.encode("utf-8")).hexdigest()


class Handler(http.server.SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=PROJECTS_DIR, **kwargs)

    def do_GET(self):
        if self.path == "/api/projects":
            self._send_json(200, self._collect_projects())
            return
        super().do_GET()

    def do_POST(self):
        content_len = int(self.headers.get("Content-Length", 0))
        body = self.rfile.read(content_len).decode("utf-8") if content_len else "{}"
        try:
            data = json.loads(body)
        except json.JSONDecodeError:
            self._send_json(400, {"error": "Invalid JSON"})
            return

        if self.path == "/api/register":
            self._handle_register(data)
        elif self.path == "/api/login":
            self._handle_login(data)
        else:
            self._send_json(404, {"error": "Not found"})

    def _handle_register(self, data):
        username = data.get("username", "").strip()
        password = data.get("password", "")

        if not username or not password:
            self._send_json(400, {"error": "Ім'я та пароль обов'язкові"})
            return

        if len(username) < 2:
            self._send_json(400, {"error": "Ім'я занадто коротке (мінімум 2 символи)"})
            return

        if len(password) < 3:
            self._send_json(400, {"error": "Пароль занадто короткий (мінімум 3 символи)"})
            return

        # Перевірка на небажані символи (пробіли)
        if re.search(r"\s", username):
            self._send_json(400, {"error": "Ім'я не може містити пробіли"})
            return

        conn = sqlite3.connect(DB_PATH)
        cur = conn.cursor()
        try:
            cur.execute("INSERT INTO users (username, password_hash) VALUES (?, ?)",
                        (username, _hash_password(password)))
            conn.commit()
            self._send_json(200, {"ok": True, "message": f"Користувача {username}@emerald.com створено!"})
        except sqlite3.IntegrityError:
            self._send_json(409, {"error": f"Користувач {username}@emerald.com вже існує"})
        finally:
            conn.close()

    def _handle_login(self, data):
        username = data.get("username", "").strip()
        password = data.get("password", "")

        if not username or not password:
            self._send_json(400, {"error": "Ім'я та пароль обов'язкові"})
            return

        conn = sqlite3.connect(DB_PATH)
        cur = conn.cursor()
        cur.execute("SELECT password_hash FROM users WHERE username = ?", (username,))
        row = cur.fetchone()
        conn.close()

        if row is None:
            self._send_json(401, {"error": "Користувача не існує"})
            return

        if row[0] != _hash_password(password):
            self._send_json(401, {"error": "Неправильний пароль"})
            return

        self._send_json(200, {"ok": True, "message": f"Ласкаво просимо, {username}@emerald.com!"})

    def do_OPTIONS(self):
        """CORS preflight"""
        self.send_response(200)
        self.send_header("Access-Control-Allow-Origin", "*")
        self.send_header("Access-Control-Allow-Methods", "POST, OPTIONS")
        self.send_header("Access-Control-Allow-Headers", "Content-Type")
        self.end_headers()

    def _collect_projects(self):
        projects = []
        exclude = {"EMERALD.inc", "безіменна папка", "безіменна папка 2"}
        with os.scandir(PROJECTS_DIR) as entries:
            for entry in sorted(entries, key=lambda item: item.name.lower()):
                if not entry.is_dir() or entry.name.startswith("."):
                    continue
                if entry.name in exclude:
                    continue
                projects.append(self._build_project_entry(entry.path))
        return projects

    def _build_project_entry(self, project_dir):
        name = os.path.basename(project_dir)
        summary = ""
        description = ""
        tags = []
        readme_path = os.path.join(project_dir, "README.md")

        if os.path.exists(readme_path):
            text = self._read_text(readme_path)
            summary = self._extract_summary(text)
            description = self._extract_description(text)
            tags = self._extract_tags(text)

        if not summary:
            summary = self._extract_html_title(project_dir) or f"Проєкт {name}"
        if not description:
            description = summary
        if not tags:
            tags = self._guess_tags(project_dir)

        return {
            "name": name,
            "icon": self._guess_icon(name, summary),
            "summary": summary,
            "description": description,
            "subtitle": "Проєкт зі складу Projects",
            "tags": tags,
            "openPath": self._resolve_open_path(project_dir),
        }

    def _read_text(self, path):
        try:
            with open(path, "r", encoding="utf-8", errors="ignore") as handle:
                return handle.read()
        except FileNotFoundError:
            return ""

    def _extract_summary(self, text):
        lines = []
        for raw_line in text.splitlines():
            line = raw_line.strip()
            if not line or line.startswith("#") or line.startswith("---") or line.startswith("!"):
                continue
            line = re.sub(r"!\[[^\]]*\]\([^)]+\)", "", line)
            line = line.replace("|", " ")
            if line.startswith("```"):
                continue
            if line.startswith("- ") or line.startswith("* "):
                continue
            if line.startswith("|"):
                continue
            if line.startswith(">"):
                continue
            if line.startswith("##"):
                continue
            if not line:
                continue
            lines.append(re.sub(r"\[(.*?)\]\([^)]+\)", r"\1", line))
            if len(lines) >= 2:
                break
        if lines:
            summary = re.sub(r"\*\*(.*?)\*\*", r"\1", lines[0]).strip()
            return summary
        return ""

    def _extract_description(self, text):
        summary = self._extract_summary(text)
        if summary:
            return summary
        return "Проєкт доступний для перегляду з папки Projects."

    def _extract_tags(self, text):
        tags = []
        for raw_line in text.splitlines():
            line = raw_line.strip()
            if not line.startswith(("- ", "* ")):
                continue
            cleaned = re.sub(r"^[-*]\s+", "", line)
            cleaned = re.sub(r"\*\*(.*?)\*\*", r"\1", cleaned)
            cleaned = re.sub(r"`([^`]+)`", r"\1", cleaned)
            cleaned = cleaned.replace("✅", "").strip()
            if cleaned:
                tags.append(cleaned)
            if len(tags) >= 4:
                break
        return tags

    def _guess_tags(self, project_dir):
        name = os.path.basename(project_dir).lower()
        tags = []
        if "game" in name or "play" in name or "гру" in name or "sus" in name:
            tags.append("гра")
        if "web" in name or "site" in name or "inc" in name or "emerald" in name:
            tags.append("веб")
        if not tags:
            tags.append("проєкт")
        return tags

    def _guess_icon(self, name, summary):
        lowered = name.lower()
        if "game" in lowered or "гра" in lowered or "sus" in lowered:
            return "🎮"
        if "emerald" in lowered:
            return "💎"
        if "pay" in lowered or "epay" in lowered:
            return "💳"
        if "ephone" in lowered or "phone" in lowered:
            return "📱"
        if "yellow" in lowered or "display" in lowered:
            return "📺"
        if "house" in lowered or "flopper" in lowered:
            return "🏚️"
        if "board" in lowered:
            return "🖥️"
        if "web" in lowered or "site" in lowered:
            return "🌐"
        if "mobile" in lowered or "phone" in lowered:
            return "📱"
        return "📦"

    def _extract_html_title(self, project_dir):
        html_candidates = ["index.html", "public/index.html", "frontend/index.html", "jesus.html"]
        for rel_path in html_candidates:
            full_path = os.path.join(project_dir, rel_path)
            if not os.path.exists(full_path):
                continue
            try:
                with open(full_path, "r", encoding="utf-8", errors="ignore") as handle:
                    content = handle.read(20000)
                match = re.search(r"<title[^>]*>(.*?)</title>", content, flags=re.IGNORECASE | re.DOTALL)
                if match:
                    title = re.sub(r"<[^>]+>", "", match.group(1)).strip()
                    return title
            except Exception:
                continue
        return ""

    def _resolve_open_path(self, project_dir):
        name = os.path.basename(project_dir)
        candidates = ["public/index.html", "frontend/index.html", "index.html", "jesus.html", "ePhone.html", "epay.html"]
        for rel_path in candidates:
            full_path = os.path.join(project_dir, rel_path)
            if os.path.exists(full_path):
                return "/" + quote(name, safe="") + "/" + quote(rel_path.replace("\\", "/"), safe="/")
        # Якщо в папці проєкту немає HTML, шукаємо {name}.html в папці EMERALD.inc
        emerald_inc_dir = os.path.join(PROJECTS_DIR, "EMERALD.inc")
        alt_html = name + ".html"
        alt_path = os.path.join(emerald_inc_dir, alt_html)
        if os.path.exists(alt_path):
            return "/EMERALD.inc/" + quote(alt_html, safe="")
        return "/" + quote(name, safe="") + "/"

    def _send_json(self, status, data):
        self.send_response(status)
        self.send_header("Content-Type", "application/json")
        self.send_header("Access-Control-Allow-Origin", "*")
        self.end_headers()
        self.wfile.write(json.dumps(data, ensure_ascii=False).encode("utf-8"))

    def log_message(self, format, *args):
        if len(args) >= 3:
            print(f"[SERVER] {args[0]} {args[1]} {args[2]}")
        else:
            print(f"[SERVER] {' '.join(str(a) for a in args)}")


if __name__ == "__main__":
    _init_db()
    os.chdir(PROJECTS_DIR)
    print(f"🌐 Emerald Inc. — сервер запущено!")
    print(f"   Сайт:        http://localhost:{PORT}/EMERALD.inc/index.html")
    print(f"   E-suslya:    http://localhost:{PORT}/e-suslya/index.html")
    print(f"   Emerald:     http://localhost:{PORT}/Emerald/public/index.html")
    print(f"   Натисніть Ctrl+C для зупинки\n")

    socketserver.TCPServer.allow_reuse_address = True
    with socketserver.TCPServer(("", PORT), Handler) as httpd:
        try:
            httpd.serve_forever()
        except KeyboardInterrupt:
            print("\n👋 Сервер зупинено.")
            httpd.server_close()
            sys.exit(0)
