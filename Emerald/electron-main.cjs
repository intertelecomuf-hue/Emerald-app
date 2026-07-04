const { app, BrowserWindow, Menu } = require('electron');
const path = require('path');
const fs = require('fs');
const net = require('net');

const { getReply, getSettings, saveSettings } = require('./botController.cjs');

let mainWindow;
let httpServer;
let actualPort = 3000;

// ─── Find a free port ─────────────────────────────────────────
function getFreePort(startPort) {
    return new Promise((resolve, reject) => {
        const server = net.createServer();
        server.listen(startPort, '127.0.0.1', () => {
            const port = server.address().port;
            server.close(() => resolve(port));
        });
        server.on('error', (err) => {
            if (err.code === 'EADDRINUSE') {
                resolve(getFreePort(startPort + 1));
            } else {
                reject(err);
            }
        });
    });
}
// ───────────────────────────────────────────────────────────────

async function startServer() {
    const express = require('express');
    const cors = require('cors');

    const publicPath = path.join(__dirname, 'public');
    const userDataPath = app.getPath('userData');
    const EMAIL_DB_PATH = path.join(userDataPath, 'emails.json');
    let emails = [];

    function loadEmails() {
        try {
            if (fs.existsSync(EMAIL_DB_PATH)) {
                emails = JSON.parse(fs.readFileSync(EMAIL_DB_PATH, 'utf-8'));
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

    const server = express();
    server.use(cors());
    server.use(express.json());
    server.use(express.static(publicPath));

    server.post('/api/chat', (req, res) => {
        const { message } = req.body;
        if (typeof message !== 'string' || !message.trim()) {
            return res.status(400).json({ error: 'Message is required.' });
        }
        const reply = getReply(message);
        res.json({ reply });
    });

    server.get('/api/settings', (req, res) => {
        res.json(getSettings());
    });

    server.post('/api/settings', (req, res) => {
        const settings = req.body;
        res.json(saveSettings(settings));
    });

    server.get('/api/emails', (req, res) => {
        res.json(emails);
    });

    server.post('/api/emails', (req, res) => {
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

    server.get('*', (req, res) => {
        if (req.path.startsWith('/api/')) {
            return res.status(404).json({ error: 'Not found' });
        }
        res.sendFile(path.join(publicPath, 'index.html'));
    });

    return new Promise((resolve, reject) => {
        httpServer = server.listen(actualPort, () => {
            console.log(`Emerald backend running at http://localhost:${actualPort}`);
            resolve();
        });
        httpServer.on('error', (err) => {
            reject(err);
        });
    });
}

function createWindow() {
    mainWindow = new BrowserWindow({
        width: 1200,
        height: 800,
        title: 'Emerald',
        icon: path.join(__dirname, 'public', 'favicon.ico'),
        webPreferences: {
            nodeIntegration: false,
            contextIsolation: true
        }
    });

    Menu.setApplicationMenu(null);
    mainWindow.setMenuBarVisibility(false);

    mainWindow.loadURL(`http://localhost:${actualPort}`);

    mainWindow.on('closed', () => {
        mainWindow = null;
    });
}

app.whenReady().then(async () => {
    actualPort = await getFreePort(parseInt(process.env.PORT, 10) || 3000);
    await startServer();
    createWindow();
});

app.on('window-all-closed', () => {
    if (httpServer) {
        httpServer.close();
    }
    app.quit();
});

app.on('activate', () => {
    if (mainWindow === null) {
        createWindow();
    }
});
