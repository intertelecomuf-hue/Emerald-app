// ============================================================
// EMERALD EXPLORER — Browser
// ============================================================

// ─── Boot Animation (скопійовано з EMERALD.inc) ──────────────
(function bootAnimation() {
    const canvas = document.getElementById('bootCanvas');
    const ctx = canvas.getContext('2d');
    const lines = document.querySelectorAll('#bootTerminal .line');
    const progressWrap = document.getElementById('bootProgress');
    const progressFill = document.getElementById('bootProgressFill');
    const progressLabel = document.getElementById('bootProgressLabel');

    let progress = 0;
    let lineIndex = 0;
    let animFrame;

    // Малюємо кристал Emerald на canvas
    function drawCrystal(angle) {
        const w = canvas.width;
        const h = canvas.height;
        ctx.clearRect(0, 0, w, h);

        const cx = w / 2;
        const cy = h / 2;

        ctx.save();
        ctx.translate(cx, cy);
        ctx.rotate(angle);

        // Зовнішнє сяйво
        const glow = ctx.createRadialGradient(0, 0, 10, 0, 0, 80);
        glow.addColorStop(0, 'rgba(0, 230, 118, 0.15)');
        glow.addColorStop(1, 'rgba(0, 230, 118, 0)');
        ctx.fillStyle = glow;
        ctx.beginPath();
        ctx.arc(0, 0, 80, 0, Math.PI * 2);
        ctx.fill();

        // Кристал (ромб)
        ctx.beginPath();
        ctx.moveTo(0, -40);
        ctx.lineTo(30, 0);
        ctx.lineTo(0, 40);
        ctx.lineTo(-30, 0);
        ctx.closePath();

        const grad = ctx.createLinearGradient(-30, -40, 30, 40);
        grad.addColorStop(0, '#00e676');
        grad.addColorStop(0.5, '#00c853');
        grad.addColorStop(1, '#009624');
        ctx.fillStyle = grad;
        ctx.fill();

        ctx.strokeStyle = 'rgba(0, 230, 118, 0.6)';
        ctx.lineWidth = 2;
        ctx.stroke();

        // Внутрішній блиск
        ctx.beginPath();
        ctx.moveTo(0, -25);
        ctx.lineTo(18, 0);
        ctx.lineTo(0, 25);
        ctx.lineTo(-18, 0);
        ctx.closePath();
        ctx.fillStyle = 'rgba(255, 255, 255, 0.15)';
        ctx.fill();

        ctx.restore();
    }

    let angle = 0;

    function animateCrystal() {
        angle += 0.02;
        drawCrystal(angle);
        animFrame = requestAnimationFrame(animateCrystal);
    }

    animateCrystal();

    // Показуємо рядки терміналу по черзі
    function showNextLine() {
        if (lineIndex < lines.length) {
            const line = lines[lineIndex];
            line.classList.add('visible');
            lineIndex++;

            // Оновлюємо прогрес
            const lineProgress = Math.min((lineIndex / lines.length) * 100, 95);
            progress = lineProgress;
            progressFill.style.width = progress + '%';
            progressLabel.textContent = Math.round(progress) + '%';

            setTimeout(showNextLine, 400 + Math.random() * 300);
        }
    }

    // Показуємо прогрес-бар
    setTimeout(() => {
        progressWrap.classList.add('visible');
    }, 600);

    // Запускаємо рядки
    setTimeout(showNextLine, 300);

    // Дозавантаження до 100%
    setTimeout(() => {
        progress = 100;
        progressFill.style.width = '100%';
        progressLabel.textContent = '100%';
    }, 3500);

    // Завершення boot-анімації
    setTimeout(() => {
        cancelAnimationFrame(animFrame);
        drawCrystal(0);
        document.getElementById('bootOverlay').classList.add('hidden');
        document.getElementById('browserWindow').classList.remove('hidden');
        initBrowser();
    }, 4200);
})();

// ─── Loading Animation (для завантаження сайтів) ─────────────
function showLoadingAnimation() {
    return new Promise((resolve) => {
        const overlay = document.getElementById('loadingOverlay');
        const canvas = document.getElementById('loadingCanvas');
        const ctx = canvas.getContext('2d');
        const lines = document.querySelectorAll('#loadingTerminal .line');
        const progressFill = document.getElementById('loadingProgressFill');
        const progressLabel = document.getElementById('loadingProgressLabel');

        // Скидаємо
        overlay.classList.remove('hidden');
        lines.forEach(l => l.classList.remove('visible'));
        progressFill.style.width = '0%';
        progressLabel.textContent = '0%';

        let angle = 0;
        let animFrame;
        let lineIndex = 0;
        let progress = 0;

        function drawCrystal(a) {
            const w = canvas.width;
            const h = canvas.height;
            ctx.clearRect(0, 0, w, h);

            const cx = w / 2;
            const cy = h / 2;

            ctx.save();
            ctx.translate(cx, cy);
            ctx.rotate(a);

            const glow = ctx.createRadialGradient(0, 0, 10, 0, 0, 80);
            glow.addColorStop(0, 'rgba(0, 230, 118, 0.15)');
            glow.addColorStop(1, 'rgba(0, 230, 118, 0)');
            ctx.fillStyle = glow;
            ctx.beginPath();
            ctx.arc(0, 0, 80, 0, Math.PI * 2);
            ctx.fill();

            ctx.beginPath();
            ctx.moveTo(0, -40);
            ctx.lineTo(30, 0);
            ctx.lineTo(0, 40);
            ctx.lineTo(-30, 0);
            ctx.closePath();

            const grad = ctx.createLinearGradient(-30, -40, 30, 40);
            grad.addColorStop(0, '#00e676');
            grad.addColorStop(0.5, '#00c853');
            grad.addColorStop(1, '#009624');
            ctx.fillStyle = grad;
            ctx.fill();

            ctx.strokeStyle = 'rgba(0, 230, 118, 0.6)';
            ctx.lineWidth = 2;
            ctx.stroke();

            ctx.beginPath();
            ctx.moveTo(0, -25);
            ctx.lineTo(18, 0);
            ctx.lineTo(0, 25);
            ctx.lineTo(-18, 0);
            ctx.closePath();
            ctx.fillStyle = 'rgba(255, 255, 255, 0.15)';
            ctx.fill();

            ctx.restore();
        }

        function animate() {
            angle += 0.03;
            drawCrystal(angle);
            animFrame = requestAnimationFrame(animate);
        }

        animate();

        function showNextLine() {
            if (lineIndex < lines.length) {
                const line = lines[lineIndex];
                line.classList.add('visible');
                lineIndex++;

                const lineProgress = Math.min((lineIndex / lines.length) * 100, 90);
                progress = lineProgress;
                progressFill.style.width = progress + '%';
                progressLabel.textContent = Math.round(progress) + '%';

                setTimeout(showNextLine, 300 + Math.random() * 200);
            }
        }

        setTimeout(showNextLine, 200);

        // Зберігаємо resolve, щоб викликати пізніше
        window._loadingResolve = () => {
            progress = 100;
            progressFill.style.width = '100%';
            progressLabel.textContent = '100%';

            setTimeout(() => {
                cancelAnimationFrame(animFrame);
                overlay.classList.add('hidden');
                resolve();
            }, 400);
        };
    });
}

function finishLoading() {
    if (window._loadingResolve) {
        window._loadingResolve();
        window._loadingResolve = null;
    }
}

// Escape HTML
function escapeHtml(text) {
    const div = document.createElement('div');
    div.textContent = text;
    return div.innerHTML;
}

// ─── Browser Logic ───────────────────────────────────────────
function initBrowser() {
    const addressInput = document.getElementById('addressInput');
    const goBtn = document.getElementById('goBtn');
    const backBtn = document.getElementById('backBtn');
    const forwardBtn = document.getElementById('forwardBtn');
    const reloadBtn = document.getElementById('reloadBtn');
    const contentArea = document.getElementById('contentArea');
    const iframeContainer = document.getElementById('iframeContainer');
    const newTabPage = document.getElementById('newTabPage');
    const tabsBar = document.getElementById('tabsBar');
    const newTabBtn = document.getElementById('newTabBtn');
    const statusText = document.getElementById('statusText');
    const statusUrl = document.getElementById('statusUrl');
    const ntpSearchInput = document.getElementById('ntpSearchInput');
    const ntpSearchBtn = document.getElementById('ntpSearchBtn');

    let tabs = [];
    let activeTabId = 0;
    let tabIdCounter = 1;

    // Створюємо початкову вкладку
    function createTab(url, isNewTab = true) {
        const tabId = tabIdCounter++;
        const tab = {
            id: tabId,
            url: url || '',
            title: isNewTab ? 'Нова вкладка' : url || 'Нова вкладка',
            history: [],
            historyIndex: -1,
            iframe: null
        };

        if (url) {
            tab.history = [url];
            tab.historyIndex = 0;
        }

        tabs.push(tab);
        renderTabs();
        activateTab(tabId);

        if (url) {
            navigateToUrl(tabId, url);
        } else {
            showNewTabPage();
        }

        return tab;
    }

    // Рендеримо вкладки
    function renderTabs() {
        // Видаляємо всі вкладки крім кнопки "+"
        const tabElements = tabsBar.querySelectorAll('.tab');
        tabElements.forEach(el => el.remove());

        tabs.forEach(tab => {
            const tabEl = document.createElement('div');
            tabEl.className = 'tab' + (tab.id === activeTabId ? ' active' : '');
            tabEl.dataset.tabId = tab.id;
            tabEl.innerHTML = `
                <span class="tab-favicon">${tab.url ? '🌐' : '🌐'}</span>
                <span class="tab-title">${escapeHtml(tab.title)}</span>
                <button class="tab-close">✕</button>
            `;

            tabEl.addEventListener('click', (e) => {
                if (e.target.classList.contains('tab-close')) return;
                activateTab(tab.id);
            });

            tabEl.querySelector('.tab-close').addEventListener('click', (e) => {
                e.stopPropagation();
                closeTab(tab.id);
            });

            tabsBar.insertBefore(tabEl, newTabBtn);
        });
    }

    // Активуємо вкладку
    function activateTab(tabId) {
        activeTabId = tabId;
        renderTabs();

        const tab = tabs.find(t => t.id === tabId);
        if (!tab) return;

        // Оновлюємо адресний рядок
        if (tab.url) {
            addressInput.value = tab.url;
        } else {
            addressInput.value = '';
        }

        // Показуємо iframe або нову вкладку
        if (tab.iframe) {
            iframeContainer.innerHTML = '';
            iframeContainer.appendChild(tab.iframe);
            iframeContainer.classList.add('active');
            newTabPage.classList.add('hidden');
        } else {
            iframeContainer.classList.remove('active');
            newTabPage.classList.remove('hidden');
        }

        updateNavButtons(tab);
    }

    // Закриваємо вкладку
    function closeTab(tabId) {
        if (tabs.length <= 1) return;

        const tabIndex = tabs.findIndex(t => t.id === tabId);
        const tab = tabs[tabIndex];
        if (tab.iframe) {
            tab.iframe.src = 'about:blank';
            tab.iframe.remove();
        }
        tabs.splice(tabIndex, 1);

        if (activeTabId === tabId) {
            const newIndex = Math.min(tabIndex, tabs.length - 1);
            activateTab(tabs[newIndex].id);
        } else {
            renderTabs();
        }
    }

    // Оновлюємо кнопки навігації
    function updateNavButtons(tab) {
        backBtn.disabled = tab.historyIndex <= 0;
        forwardBtn.disabled = tab.historyIndex >= tab.history.length - 1;
    }

    // Показуємо сторінку нової вкладки
    function showNewTabPage() {
        iframeContainer.classList.remove('active');
        newTabPage.classList.remove('hidden');
        addressInput.value = '';
        statusText.textContent = 'Готово';
        statusUrl.textContent = '';
    }

    // Проксі URL (відносний, оскільки сервер роздає і статику, і проксі)
    const PROXY_URL = '/proxy?url=';

    // Перевірка чи проксі доступний (кешуємо результат)
    let proxyAvailable = null;

    async function checkProxy() {
        if (proxyAvailable !== null) return proxyAvailable;
        try {
            const controller = new AbortController();
            const timeout = setTimeout(() => controller.abort(), 3000);
            const res = await fetch(PROXY_URL + encodeURIComponent('https://example.com'), { 
                method: 'GET',
                signal: controller.signal
            });
            clearTimeout(timeout);
            proxyAvailable = res.ok;
            return proxyAvailable;
        } catch (e) {
            console.warn('[Emerald] Proxy not available:', e.message);
            proxyAvailable = false;
            return false;
        }
    }

    // Навігація на URL
    async function navigateToUrl(tabId, url) {
        const tab = tabs.find(t => t.id === tabId);
        if (!tab) return;

        // Нормалізуємо URL
        if (!url.startsWith('http://') && !url.startsWith('https://')) {
            // Перевіряємо чи це домен
            if (url.includes('.') && !url.includes(' ')) {
                url = 'https://' + url;
            } else {
                // Це пошуковий запит
                url = 'https://www.google.com/search?q=' + encodeURIComponent(url);
            }
        }

        tab.url = url;
        addressInput.value = url;

        // Додаємо в історію
        if (tab.historyIndex < tab.history.length - 1) {
            tab.history = tab.history.slice(0, tab.historyIndex + 1);
        }
        tab.history.push(url);
        tab.historyIndex = tab.history.length - 1;

        updateNavButtons(tab);

        // Показуємо loading анімацію
        statusText.textContent = 'Завантаження...';
        statusUrl.textContent = url;

        // Видаляємо старий iframe
        if (tab.iframe) {
            tab.iframe.src = 'about:blank';
            tab.iframe.remove();
            tab.iframe = null;
        }

        // Показуємо loading overlay
        const loadingPromise = showLoadingAnimation();

        // Створюємо новий iframe
        const iframe = document.createElement('iframe');
        iframe.sandbox = 'allow-scripts allow-forms allow-same-origin allow-popups';
        iframe.allow = 'camera; microphone; geolocation';

        // Таймаут для випадку, якщо сайт блокує iframe
        let loadTimer = setTimeout(() => {
            finishLoading();
            // Якщо сайт не завантажився за 8 секунд, пропонуємо відкрити в новому вікні
            showIframeBlocked(tab, url);
        }, 8000);

        iframe.addEventListener('load', () => {
            clearTimeout(loadTimer);
            finishLoading();
            try {
                const title = iframe.contentDocument.title || url;
                tab.title = title;
                renderTabs();
            } catch (e) {
                tab.title = url;
                renderTabs();
            }
            statusText.textContent = 'Готово';
            statusUrl.textContent = '';
        });

        iframe.addEventListener('error', () => {
            clearTimeout(loadTimer);
            finishLoading();
            statusText.textContent = 'Помилка завантаження';
            showIframeBlocked(tab, url);
        });

        tab.iframe = iframe;

        // Показуємо iframe
        iframeContainer.innerHTML = '';
        iframeContainer.appendChild(iframe);
        iframeContainer.classList.add('active');
        newTabPage.classList.add('hidden');

        // Завантажуємо URL через проксі (якщо доступний) або напряму
        const useProxy = await checkProxy();
        if (useProxy) {
            iframe.src = PROXY_URL + encodeURIComponent(url);
            statusText.textContent = 'Завантаження через проксі...';
        } else {
            iframe.src = url;
        }

        // Чекаємо завершення анімації
        await loadingPromise;
    }

    // Обробка введення адреси
    function handleAddressSubmit() {
        const url = addressInput.value.trim();
        if (!url) return;
        navigateToUrl(activeTabId, url);
    }

    // Кнопки навігації
    backBtn.addEventListener('click', () => {
        const tab = tabs.find(t => t.id === activeTabId);
        if (!tab || tab.historyIndex <= 0) return;
        tab.historyIndex--;
        const url = tab.history[tab.historyIndex];
        navigateToUrl(activeTabId, url);
    });

    forwardBtn.addEventListener('click', () => {
        const tab = tabs.find(t => t.id === activeTabId);
        if (!tab || tab.historyIndex >= tab.history.length - 1) return;
        tab.historyIndex++;
        const url = tab.history[tab.historyIndex];
        navigateToUrl(activeTabId, url);
    });

    reloadBtn.addEventListener('click', () => {
        const tab = tabs.find(t => t.id === activeTabId);
        if (!tab || !tab.url) return;
        navigateToUrl(activeTabId, tab.url);
    });

    goBtn.addEventListener('click', handleAddressSubmit);
    addressInput.addEventListener('keydown', (e) => {
        if (e.key === 'Enter') handleAddressSubmit();
    });

    // Нова вкладка
    newTabBtn.addEventListener('click', () => {
        createTab('', true);
    });

    // Пошук на новій вкладці
    ntpSearchBtn.addEventListener('click', () => {
        const query = ntpSearchInput.value.trim();
        if (!query) return;
        navigateToUrl(activeTabId, query);
    });

    ntpSearchInput.addEventListener('keydown', (e) => {
        if (e.key === 'Enter') {
            const query = ntpSearchInput.value.trim();
            if (!query) return;
            navigateToUrl(activeTabId, query);
        }
    });

    // Шорткати на новій вкладці
    document.querySelectorAll('.shortcut').forEach(el => {
        el.addEventListener('click', () => {
            const url = el.dataset.url;
            if (url) navigateToUrl(activeTabId, url);
        });
    });

    // Закладки
    document.querySelectorAll('.bookmark').forEach(el => {
        el.addEventListener('click', () => {
            const url = el.dataset.url;
            if (url) navigateToUrl(activeTabId, url);
        });
    });

    // Додати закладку
    document.getElementById('addBookmarkBtn').addEventListener('click', () => {
        const tab = tabs.find(t => t.id === activeTabId);
        if (!tab || !tab.url) return;

        const name = prompt('Назва закладки:', tab.title);
        if (!name) return;

        const bookmarkBar = document.getElementById('bookmarksBar');
        const addBtn = document.getElementById('addBookmarkBtn');

        const bookmark = document.createElement('div');
        bookmark.className = 'bookmark';
        bookmark.dataset.url = tab.url;
        bookmark.textContent = '⭐ ' + name;
        bookmark.addEventListener('click', () => {
            navigateToUrl(activeTabId, tab.url);
        });

        bookmarkBar.insertBefore(bookmark, addBtn);
    });

    // Показуємо повідомлення, якщо сайт блокує iframe
    function showIframeBlocked(tab, url) {
        if (!tab.iframe) return;
        
        // Перевіряємо чи iframe порожній (сайт заблокував)
        try {
            const doc = tab.iframe.contentDocument;
            if (doc && doc.body && doc.body.innerHTML.trim().length > 0) {
                return; // Сайт завантажився
            }
        } catch (e) {
            // Cross-origin — значить сайт заблокував iframe
        }

        // Показуємо повідомлення про блокування
        const blockedDiv = document.createElement('div');
        blockedDiv.style.cssText = `
            position: absolute;
            inset: 0;
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
            background: linear-gradient(135deg, #0a1a0a 0%, #0d2e0d 50%, #0a1a0a 100%);
            color: #e0f0e0;
            padding: 40px 20px;
            text-align: center;
            z-index: 10;
        `;
        blockedDiv.innerHTML = `
            <div style="font-size: 60px; margin-bottom: 16px;">💎</div>
            <h2 style="color: #00e676; margin-bottom: 12px; font-size: 22px;">Сайт блокує відображення в iframe</h2>
            <p style="color: #a0d0a0; margin-bottom: 24px; max-width: 400px; line-height: 1.6;">
                Сайт <strong style="color: #00e676;">${escapeHtml(url)}</strong> не дозволяє відображати себе 
                у вбудованому вікні через політику безпеки.
            </p>
            <div style="display: flex; gap: 12px; flex-wrap: wrap; justify-content: center;">
                <button onclick="window.open('${escapeHtml(url)}', '_blank')" style="
                    padding: 12px 28px;
                    border: none;
                    background: linear-gradient(135deg, #00e676, #00c853);
                    color: #062b08;
                    font-size: 15px;
                    font-weight: 600;
                    border-radius: 28px;
                    cursor: pointer;
                    font-family: inherit;
                    transition: all 0.2s;
                " onmouseover="this.style.opacity='0.9'" onmouseout="this.style.opacity='1'">
                    🌐 Відкрити в новому вікні
                </button>
                <button onclick="this.parentElement.parentElement.remove()" style="
                    padding: 12px 28px;
                    border: 1px solid rgba(0, 200, 80, 0.3);
                    background: transparent;
                    color: #a0d0a0;
                    font-size: 15px;
                    border-radius: 28px;
                    cursor: pointer;
                    font-family: inherit;
                    transition: all 0.2s;
                " onmouseover="this.style.background='rgba(0,200,80,0.1)'" onmouseout="this.style.background='transparent'">
                    ✕ Закрити
                </button>
            </div>
        `;

        // Додаємо поверх iframe
        const container = tab.iframe.parentElement;
        if (container) {
            container.style.position = 'relative';
            container.appendChild(blockedDiv);
        }
    }

    // ─── Логіка входу/профілю ────────────────────────────────
    const loginBtn = document.getElementById('loginBtn');
    const userProfile = document.getElementById('userProfile');
    const profileAvatar = document.getElementById('profileAvatar');
    const profileName = document.getElementById('profileName');
    const profileEmail = document.getElementById('profileEmail');
    const logoutBtn = document.getElementById('logoutBtn');
    const loginModalOverlay = document.getElementById('loginModalOverlay');
    const loginCloseBtn = document.getElementById('loginCloseBtn');
    const loginEmail = document.getElementById('loginEmail');
    const loginPassword = document.getElementById('loginPassword');
    const loginSubmitBtn = document.getElementById('loginSubmitBtn');
    const loginError = document.getElementById('loginError');

    let isLoggedIn = false;

    // Відкрити модальне вікно
    loginBtn.addEventListener('click', () => {
        loginModalOverlay.classList.add('active');
        loginEmail.value = '';
        loginPassword.value = '';
        loginError.classList.remove('visible');
        setTimeout(() => loginEmail.focus(), 300);
    });

    // Закрити модальне вікно
    loginCloseBtn.addEventListener('click', () => {
        loginModalOverlay.classList.remove('active');
    });

    loginModalOverlay.addEventListener('click', (e) => {
        if (e.target === loginModalOverlay) {
            loginModalOverlay.classList.remove('active');
        }
    });

    // Вхід через EMERALD.inc API
    loginSubmitBtn.addEventListener('click', async () => {
        const email = loginEmail.value.trim();
        const password = loginPassword.value.trim();

        if (!email || !password) {
            loginError.textContent = 'Будь ласка, заповніть всі поля';
            loginError.classList.add('visible');
            return;
        }

        if (password.length < 3) {
            loginError.textContent = 'Пароль має бути не менше 3 символів';
            loginError.classList.add('visible');
            return;
        }

        // Витягуємо username з email (підтримуємо username@emerald.com або просто username)
        let username = email;
        if (email.includes('@')) {
            const parts = email.split('@');
            if (parts[1] !== 'emerald.com') {
                loginError.textContent = 'Використовуйте акаунт @emerald.com';
                loginError.classList.add('visible');
                return;
            }
            username = parts[0];
        }

        // Показуємо статус завантаження
        loginSubmitBtn.textContent = 'Вхід...';
        loginSubmitBtn.disabled = true;
        loginError.classList.remove('visible');

        try {
            // Пробуємо підключитися до EMERALD.inc API
            const controller = new AbortController();
            const timeout = setTimeout(() => controller.abort(), 5000);

            const res = await fetch('http://localhost:8080/api/login', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ username, password }),
                signal: controller.signal
            });

            clearTimeout(timeout);
            const data = await res.json();

            if (res.ok && data.ok) {
                // Успішний вхід через EMERALD.inc
                isLoggedIn = true;
                const displayName = username;
                const avatarLetter = displayName.charAt(0).toUpperCase();

                profileName.textContent = displayName;
                profileEmail.textContent = username + '@emerald.com';
                profileAvatar.textContent = avatarLetter;

                loginBtn.classList.add('logged-in');
                userProfile.classList.remove('hidden');
                loginModalOverlay.classList.remove('active');

                statusText.textContent = `Вітаємо, ${displayName}!`;
                setTimeout(() => {
                    statusText.textContent = 'Готово';
                }, 3000);
            } else {
                loginError.textContent = data.error || 'Невірний логін або пароль';
                loginError.classList.add('visible');
            }
        } catch (e) {
            // Якщо сервер EMERALD.inc не доступний — пробуємо локальний вхід
            console.warn('[Emerald] EMERALD.inc API недоступний, використовуємо локальний вхід:', e.message);

            // Локальний вхід (для тестування без сервера)
            if (password.length >= 3) {
                isLoggedIn = true;
                const displayName = username;
                const avatarLetter = displayName.charAt(0).toUpperCase();

                profileName.textContent = displayName;
                profileEmail.textContent = username + '@emerald.com';
                profileAvatar.textContent = avatarLetter;

                loginBtn.classList.add('logged-in');
                userProfile.classList.remove('hidden');
                loginModalOverlay.classList.remove('active');

                statusText.textContent = `Вітаємо, ${displayName}! (локальний вхід)`;
                setTimeout(() => {
                    statusText.textContent = 'Готово';
                }, 3000);
            } else {
                loginError.textContent = 'Помилка з\'єднання з сервером. Спробуйте пізніше.';
                loginError.classList.add('visible');
            }
        } finally {
            loginSubmitBtn.textContent = 'Увійти';
            loginSubmitBtn.disabled = false;
        }
    });

    // Вхід по Enter
    loginPassword.addEventListener('keydown', (e) => {
        if (e.key === 'Enter') loginSubmitBtn.click();
    });
    loginEmail.addEventListener('keydown', (e) => {
        if (e.key === 'Enter') loginPassword.focus();
    });

    // Вихід
    logoutBtn.addEventListener('click', (e) => {
        e.stopPropagation();
        isLoggedIn = false;
        loginBtn.classList.remove('logged-in');
        userProfile.classList.add('hidden');
        statusText.textContent = 'Ви вийшли з облікового запису';
        setTimeout(() => {
            statusText.textContent = 'Готово';
        }, 3000);
    });

    // Клік по профілю — можна додати меню
    userProfile.addEventListener('click', () => {
        statusText.textContent = `Профіль: ${profileName.textContent}`;
        setTimeout(() => {
            statusText.textContent = 'Готово';
        }, 2000);
    });

    // Створюємо початкову вкладку
    createTab('', true);
}
