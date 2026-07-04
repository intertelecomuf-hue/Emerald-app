import express from 'express';
import cors from 'cors';
import helmet from 'helmet';
import path from 'path';
import { fileURLToPath } from 'url';
import fs from 'fs';
import { getReply, getSettings, saveSettings } from './botController.js';

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);
const publicPath = path.join(__dirname, 'public');

const app = express();
const port = process.env.PORT || 3000;

// ─── In-memory email store (persisted to a JSON file) ──────────
const EMAIL_DB_PATH = path.join(__dirname, 'emails.json');
let emails = [];

function loadEmails() {
  try {
    if (fs.existsSync(EMAIL_DB_PATH)) {
      const data = fs.readFileSync(EMAIL_DB_PATH, 'utf-8');
      emails = JSON.parse(data);
    }
  } catch (e) {
    emails = [];
  }
}

function saveEmailsToDisk() {
  try {
    fs.writeFileSync(EMAIL_DB_PATH, JSON.stringify(emails, null, 2), 'utf-8');
  } catch (e) {
    console.error('Failed to save emails:', e);
  }
}

loadEmails();
// ───────────────────────────────────────────────────────────────

app.use(helmet());
app.use(cors());
app.use(express.json());
app.use(express.static(publicPath));

app.post('/api/chat', (req, res) => {
  const { message } = req.body;
  if (typeof message !== 'string' || !message.trim()) {
    return res.status(400).json({ error: 'Message is required.' });
  }

  const reply = getReply(message);
  res.json({ reply });
});

app.get('/api/settings', (req, res) => {
  res.json(getSettings());
});

app.post('/api/settings', (req, res) => {
  const settings = req.body;
  const saved = saveSettings(settings);
  res.json(saved);
});

// ─── Email API ────────────────────────────────────────────────
app.get('/api/emails', (req, res) => {
  res.json(emails);
});

app.post('/api/emails', (req, res) => {
  const { to, subject, body } = req.body;
  if (!to || typeof to !== 'string' || !to.trim()) {
    return res.status(400).json({ success: false, error: 'Recipient is required.' });
  }

  const newEmail = {
    id: Date.now().toString(36) + Math.random().toString(36).slice(2, 6),
    from: 'Ви',
    to: to.trim(),
    subject: (subject || '').trim() || '(без теми)',
    body: (body || '').trim() || '(без тексту)',
    timestamp: new Date().toISOString()
  };

  emails.unshift(newEmail);
  saveEmailsToDisk();

  res.json({ success: true, email: newEmail });
});
// ───────────────────────────────────────────────────────────────

app.get('*', (req, res) => {
  if (req.path.startsWith('/api/')) {
    return res.status(404).json({ error: 'Not found' });
  }
  res.sendFile(path.join(publicPath, 'index.html'));
});

app.listen(port, () => {
  console.log(`Emerald backend running at http://localhost:${port}`);
});
