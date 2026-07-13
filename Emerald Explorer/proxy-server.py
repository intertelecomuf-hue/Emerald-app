#!/usr/bin/env python3
"""
Emerald Explorer — Server + Proxy
Запуск: python3 proxy-server.py
Відкрити: http://localhost:3456

Роздає статичні файли (index.html, styles.css, script.js)
та проксі для обходу X-Frame-Options та CSP
"""

import http.server
import urllib.request
import urllib.parse
import ssl
import sys
import os
import mimetypes

PORT = 3456
STATIC_DIR = os.path.dirname(os.path.abspath(__file__))

class EmeraldHandler(http.server.BaseHTTPRequestHandler):
    def do_OPTIONS(self):
        self.send_response(200)
        self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header('Access-Control-Allow-Methods', 'GET, HEAD, OPTIONS')
        self.send_header('Access-Control-Allow-Headers', '*')
        self.end_headers()

    def do_HEAD(self):
        self.do_GET(method='HEAD')

    def do_GET(self, method='GET'):
        parsed = urllib.parse.urlparse(self.path)

        # Якщо шлях починається з /proxy?url= — це проксі-запит
        if parsed.path == '/proxy':
            self.handle_proxy(method)
            return

        # Інакше — роздаємо статичні файли
        self.serve_static(method)

    def serve_static(self, method):
        """Роздає статичні файли"""
        path = self.path

        # За замовчуванням — index.html
        if path == '/' or path == '':
            path = '/index.html'

        # Безпека: запобігаємо path traversal
        if '..' in path or path.startswith('~'):
            self.send_response(403)
            self.send_header('Content-Type', 'text/plain; charset=utf-8')
            self.end_headers()
            self.wfile.write(b'Forbidden')
            return

        # Дозволені розширення
        allowed_extensions = {'.html', '.css', '.js', '.ico', '.png', '.svg', '.json', '.txt'}
        ext = os.path.splitext(path)[1].lower()
        if ext and ext not in allowed_extensions:
            self.send_response(403)
            self.send_header('Content-Type', 'text/plain; charset=utf-8')
            self.end_headers()
            self.wfile.write(b'Forbidden')
            return

        file_path = os.path.join(STATIC_DIR, path.lstrip('/'))

        if not os.path.isfile(file_path):
            self.send_response(404)
            self.send_header('Content-Type', 'text/plain; charset=utf-8')
            self.end_headers()
            self.wfile.write(b'Not Found')
            return

        try:
            with open(file_path, 'rb') as f:
                content = f.read()

            content_type, _ = mimetypes.guess_type(file_path)
            if content_type is None:
                content_type = 'application/octet-stream'

            self.send_response(200)
            self.send_header('Content-Type', content_type)
            self.send_header('Content-Length', str(len(content)))
            self.send_header('Cache-Control', 'no-cache')
            self.end_headers()

            if method != 'HEAD':
                self.wfile.write(content)

        except Exception as e:
            self.send_response(500)
            self.send_header('Content-Type', 'text/plain; charset=utf-8')
            self.end_headers()
            self.wfile.write(f'Server error: {str(e)}'.encode('utf-8'))

    def handle_proxy(self, method):
        """Проксі для обходу X-Frame-Options та CSP"""
        parsed = urllib.parse.urlparse(self.path)
        query = urllib.parse.parse_qs(parsed.query)
        target_urls = query.get('url', [])

        if not target_urls:
            self.send_response(400)
            self.send_header('Content-Type', 'text/plain; charset=utf-8')
            self.send_header('Access-Control-Allow-Origin', '*')
            self.send_header('Access-Control-Allow-Methods', 'GET, HEAD, OPTIONS')
            self.end_headers()
            self.wfile.write(b'Error: no URL specified. Use ?url=https://example.com')
            return

        target_url = target_urls[0]
        print(f'[Proxy] {method} {target_url}', file=sys.stderr)

        try:
            req = urllib.request.Request(
                target_url,
                headers={
                    'User-Agent': 'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36',
                    'Accept': 'text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8',
                    'Accept-Language': 'uk,en-US;q=0.9,en;q=0.8',
                },
                method='GET'
            )

            ctx = ssl.create_default_context()
            ctx.check_hostname = False
            ctx.verify_mode = ssl.CERT_NONE

            response = urllib.request.urlopen(req, timeout=15, context=ctx)

            blocked_headers = {
                'x-frame-options', 'X-Frame-Options',
                'content-security-policy', 'Content-Security-Policy',
                'content-security-policy-report-only', 'Content-Security-Policy-Report-Only',
                'transfer-encoding', 'Transfer-Encoding',
            }

            content = response.read()

            self.send_response(response.status)

            for key, value in response.getheaders():
                if key not in blocked_headers:
                    self.send_header(key, value)

            self.send_header('Access-Control-Allow-Origin', '*')
            self.send_header('Access-Control-Allow-Methods', 'GET, HEAD, OPTIONS')
            self.send_header('Cross-Origin-Embedder-Policy', 'unsafe-none')
            self.send_header('Cross-Origin-Opener-Policy', 'same-origin-allow-popups')
            self.send_header('Content-Length', str(len(content)))
            self.end_headers()

            if method != 'HEAD':
                self.wfile.write(content)

        except urllib.error.HTTPError as e:
            self.send_response(e.code)
            self.send_header('Content-Type', 'text/plain; charset=utf-8')
            self.send_header('Access-Control-Allow-Origin', '*')
            self.send_header('Access-Control-Allow-Methods', 'GET, HEAD, OPTIONS')
            self.end_headers()
            self.wfile.write(f'Proxy error: {e.code} {e.reason}'.encode('utf-8'))

        except urllib.error.URLError as e:
            self.send_response(502)
            self.send_header('Content-Type', 'text/plain; charset=utf-8')
            self.send_header('Access-Control-Allow-Origin', '*')
            self.send_header('Access-Control-Allow-Methods', 'GET, HEAD, OPTIONS')
            self.end_headers()
            self.wfile.write(f'Proxy error: {e.reason}'.encode('utf-8'))

        except Exception as e:
            self.send_response(500)
            self.send_header('Content-Type', 'text/plain; charset=utf-8')
            self.send_header('Access-Control-Allow-Origin', '*')
            self.send_header('Access-Control-Allow-Methods', 'GET, HEAD, OPTIONS')
            self.end_headers()
            self.wfile.write(f'Proxy error: {str(e)}'.encode('utf-8'))

    def log_message(self, format, *args):
        if len(args) >= 3:
            print(f'[Server] {args[0]} {args[1]} {args[2]}', file=sys.stderr)
        elif len(args) >= 1:
            print(f'[Server] {" ".join(str(a) for a in args)}', file=sys.stderr)
        else:
            print(f'[Server] {format}', file=sys.stderr)


if __name__ == '__main__':
    server = http.server.HTTPServer(('0.0.0.0', PORT), EmeraldHandler)
    print(f'\n  🌐 Emerald Explorer', file=sys.stderr)
    print(f'  ─────────────────', file=sys.stderr)
    print(f'  Server running at http://localhost:{PORT}', file=sys.stderr)
    print(f'  Open http://localhost:{PORT} in your browser', file=sys.stderr)
    print(f'  Proxy: http://localhost:{PORT}/proxy?url=https://example.com', file=sys.stderr)
    print(f'  Press Ctrl+C to stop\n', file=sys.stderr)
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print('\n  Server stopped.', file=sys.stderr)
        server.server_close()
