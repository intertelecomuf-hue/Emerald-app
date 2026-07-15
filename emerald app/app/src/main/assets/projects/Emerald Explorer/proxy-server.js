// ============================================================
// Emerald Explorer — Proxy Server
// Запуск: node proxy-server.js
// Проксі для обходу X-Frame-Options та CSP
// ============================================================

const http = require('http');
const https = require('https');
const url = require('url');

const PORT = 3456;

const server = http.createServer((req, res) => {
    // CORS заголовки для доступу з HTML сторінки
    res.setHeader('Access-Control-Allow-Origin', '*');
    res.setHeader('Access-Control-Allow-Methods', 'GET, OPTIONS');
    res.setHeader('Access-Control-Allow-Headers', '*');

    if (req.method === 'OPTIONS') {
        res.writeHead(200);
        res.end();
        return;
    }

    // Отримуємо цільовий URL з параметрів
    const query = url.parse(req.url, true).query;
    const targetUrl = query.url;

    if (!targetUrl) {
        res.writeHead(400, { 'Content-Type': 'text/plain; charset=utf-8' });
        res.end('Помилка: не вказано URL. Використовуйте ?url=https://example.com');
        return;
    }

    console.log(`[Proxy] Запит: ${targetUrl}`);

    // Визначаємо протокол
    const isHttps = targetUrl.startsWith('https://');
    const client = isHttps ? https : http;

    try {
        const parsedUrl = new URL(targetUrl);
        
        const options = {
            hostname: parsedUrl.hostname,
            port: parsedUrl.port || (isHttps ? 443 : 80),
            path: parsedUrl.pathname + parsedUrl.search,
            method: 'GET',
            headers: {
                'User-Agent': 'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36',
                'Accept': 'text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8',
                'Accept-Language': 'uk,en-US;q=0.9,en;q=0.8',
            },
            timeout: 15000
        };

        const proxyReq = client.request(options, (proxyRes) => {
            // Видаляємо заголовки, що блокують iframe
            const headers = { ...proxyRes.headers };
            delete headers['x-frame-options'];
            delete headers['X-Frame-Options'];
            delete headers['content-security-policy'];
            delete headers['Content-Security-Policy'];
            delete headers['content-security-policy-report-only'];
            delete headers['Content-Security-Policy-Report-Only'];

            // Додаємо заголовки для iframe
            headers['Access-Control-Allow-Origin'] = '*';
            headers['Cross-Origin-Embedder-Policy'] = 'unsafe-none';
            headers['Cross-Origin-Opener-Policy'] = 'same-origin-allow-popups';

            res.writeHead(proxyRes.statusCode || 200, headers);

            // Передаємо дані
            proxyRes.pipe(res, { end: true });

            proxyRes.on('error', (err) => {
                console.error(`[Proxy] Помилка читання відповіді: ${err.message}`);
                if (!res.headersSent) {
                    res.writeHead(502, { 'Content-Type': 'text/plain; charset=utf-8' });
                    res.end(`Помилка проксі: ${err.message}`);
                }
            });
        });

        proxyReq.on('error', (err) => {
            console.error(`[Proxy] Помилка з'єднання: ${err.message}`);
            if (!res.headersSent) {
                res.writeHead(502, { 'Content-Type': 'text/plain; charset=utf-8' });
                res.end(`Помилка з'єднання: ${err.message}`);
            }
        });

        proxyReq.on('timeout', () => {
            proxyReq.destroy();
            if (!res.headersSent) {
                res.writeHead(504, { 'Content-Type': 'text/plain; charset=utf-8' });
                res.end('Таймаут проксі');
            }
        });

        proxyReq.end();

    } catch (err) {
        console.error(`[Proxy] Помилка парсингу URL: ${err.message}`);
        res.writeHead(400, { 'Content-Type': 'text/plain; charset=utf-8' });
        res.end(`Невірний URL: ${err.message}`);
    }
});

server.listen(PORT, () => {
    console.log(`\n  🌐 Emerald Explorer Proxy Server`);
    console.log(`  ─────────────────────────────`);
    console.log(`  Сервер запущено на http://localhost:${PORT}`);
    console.log(`  Використовуйте http://localhost:${PORT}/proxy?url=https://example.com`);
    console.log(`  Натисніть Ctrl+C для зупинки\n`);
});
