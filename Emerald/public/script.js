const assistantName = 'Emerald';
const chatWindow = document.getElementById('chatWindow');
const userInput = document.getElementById('userInput');
const sendButton = document.getElementById('sendButton');
const voiceButton = document.getElementById('voiceButton');

const voiceSelect = document.getElementById('voiceSelect');
const languageSelect = document.getElementById('languageSelect');
let selectedVoice = null;
let voiceLoadAttempts = 0;

const avatarElement = document.querySelector('.assistant-avatar');
const avatarEmojiSelect = document.getElementById('avatarEmojiSelect');
const customEmojiInput = document.getElementById('customEmojiInput');
const backgroundSelect = document.getElementById('backgroundSelect');
const backgroundColorInput = document.getElementById('backgroundColorInput');
const cardColorInput = document.getElementById('cardColorInput');

function appendMessage(text, role) {
    const messageElement = document.createElement('div');
    messageElement.className = 'message ' + role;
    messageElement.textContent = text;
    chatWindow.appendChild(messageElement);
    chatWindow.scrollTop = chatWindow.scrollHeight;
}

function populateVoiceList() {
    const voices = window.speechSynthesis.getVoices();
    if (!voices.length && voiceLoadAttempts < 10) {
        voiceLoadAttempts += 1;
        return setTimeout(populateVoiceList, 200);
    }

    voiceSelect.innerHTML = '';
    if (!voices.length) {
        const option = document.createElement('option');
        option.textContent = 'Голоси не знайдені';
        voiceSelect.appendChild(option);
        voiceSelect.disabled = true;
        selectedVoice = null;
        return;
    }

    const placeholder = document.createElement('option');
    placeholder.value = '';
    placeholder.textContent = 'Виберіть голос Emerald';
    placeholder.disabled = true;
    placeholder.selected = true;
    voiceSelect.appendChild(placeholder);

    voices.forEach((voice, index) => {
        const option = document.createElement('option');
        option.value = index;
        const isUkrainian = /uk(-|_)ua|\buk\b/i.test(voice.lang) || /укр/i.test(voice.name);
        option.textContent = `${voice.name} (${voice.lang})${isUkrainian ? ' — український' : ''}`;
        if (voice.default) {
            option.textContent += ' — default';
        }
        voiceSelect.appendChild(option);
    });

    const ukrainianIndex = voices.findIndex(v => /uk(-|_)ua|\buk\b/i.test(v.lang) || /укр/i.test(v.name));
    const preferredIndex = ukrainianIndex >= 0 ? ukrainianIndex : voices.findIndex(v => /en-US|en_GB/i.test(v.lang));
    const chosenIndex = preferredIndex >= 0 ? preferredIndex : 0;
    if (voices[chosenIndex]) {
        selectedVoice = voices[chosenIndex];
        voiceSelect.value = chosenIndex;
    }
    voiceSelect.disabled = false;
}

if (window.speechSynthesis) {
    populateVoiceList();
    window.speechSynthesis.addEventListener('voiceschanged', populateVoiceList);
}

voiceSelect.addEventListener('change', () => {
    const voices = window.speechSynthesis.getVoices();
    selectedVoice = voices[voiceSelect.value] || selectedVoice;
});

avatarEmojiSelect.addEventListener('change', () => {
    avatarElement.textContent = avatarEmojiSelect.value;
    customEmojiInput.value = '';
    saveSettings();
});

customEmojiInput.addEventListener('input', () => {
    const customValue = customEmojiInput.value.trim();
    if (customValue) {
        avatarElement.textContent = customValue;
        avatarEmojiSelect.value = '';
    }
    saveSettings();
});

backgroundSelect.addEventListener('change', () => {
    if (backgroundSelect.value !== 'custom') {
        backgroundColorInput.style.display = 'none';
    } else {
        backgroundColorInput.style.display = 'block';
    }
    updateBackground();
    saveSettings();
});

backgroundColorInput.addEventListener('input', () => {
    if (backgroundSelect.value === 'custom') {
        updateBackground();
        saveSettings();
    }
});

cardColorInput.addEventListener('input', () => {
    updateCardColor();
    saveSettings();
});

languageSelect.addEventListener('change', () => {
    saveSettings();
});

function updateBackground() {
    const theme = backgroundSelect.value;
    document.body.style.transition = 'background 0.4s ease';
    if (theme === 'emerald') {
        document.body.style.backgroundColor = '#081d13';
        document.body.style.backgroundImage = "url(\"data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 900 900'%3E%3Cdefs%3E%3ClinearGradient id='g1' x1='0%25' y1='0%25' x2='100%25' y2='100%25'%3E%3Cstop offset='0%25' stop-color='%23d4ffd7'/%3E%3Cstop offset='40%25' stop-color='%2300b06f'/%3E%3Cstop offset='100%25' stop-color='%2301160a'/%3E%3C/linearGradient%3E%3ClinearGradient id='g2' x1='0%25' y1='0%25' x2='100%25' y2='100%25'%3E%3Cstop offset='0%25' stop-color='%23ffffff' stop-opacity='0.6'/%3E%3Cstop offset='100%25' stop-color='%23ffffff' stop-opacity='0'/%3E%3C/linearGradient%3E%3Cfilter id='glow' x='-50%25' y='-50%25' width='200%25' height='200%25'%3E%3CfeGaussianBlur stdDeviation='16' /%3E%3C/filter%3E%3C/defs%3E%3Crect width='900' height='900' fill='%23031008'/%3E%3Cpath d='M200 220 L240 300 L210 370 L150 430 L140 520 L220 580 L320 560 L350 470 L340 360 Z' fill='url(%23g1)' filter='url(%23glow)' stroke='%23011108' stroke-width='8'/%3E%3Cpath d='M310 140 L360 210 L340 300 L300 350 L260 320 L240 260 L260 190 Z' fill='url(%23g1)' filter='url(%23glow)' stroke='%23020e08' stroke-width='8'/%3E%3Cpath d='M540 120 L620 200 L640 310 L600 420 L520 430 L460 360 L470 250 Z' fill='url(%23g1)' filter='url(%23glow)' stroke='%23020e08' stroke-width='8'/%3E%3Cpath d='M260 560 L320 650 L400 700 L480 680 L540 610 L520 520 L430 500 L340 520 Z' fill='url(%23g1)' filter='url(%23glow)' stroke='%23020e08' stroke-width='8'/%3E%3Cpath d='M580 520 L650 620 L700 710 L760 700 L790 620 L760 540 L680 500 L620 520 Z' fill='url(%23g1)' filter='url(%23glow)' stroke='%23020e08' stroke-width='8'/%3E%3Cpath d='M170 370 L210 440 L200 520 L140 560 L110 520 L120 450 Z' fill='url(%23g2)' opacity='0.35'/%3E%3Cpath d='M530 150 L570 230 L560 320 L520 360 L480 330 L470 240 Z' fill='url(%23g2)' opacity='0.35'/%3E%3Cpath d='M290 600 L340 700 L410 720 L470 680 L500 620 L480 560 Z' fill='url(%23g2)' opacity='0.35'/%3E%3Cpath d='M620 560 L680 660 L720 700 L760 660 L760 600 L720 560 Z' fill='url(%23g2)' opacity='0.35'/%3E%3C/svg%3E\")";
    } else if (theme === 'dark') {
        document.body.style.backgroundColor = '#06140f';
        document.body.style.backgroundImage = 'linear-gradient(135deg, rgba(10, 33, 22, 0.95), rgba(2, 8, 4, 0.97))';
    } else if (theme === 'green-gradient') {
        document.body.style.backgroundColor = '#05220f';
        document.body.style.backgroundImage = 'linear-gradient(135deg, rgba(0, 176, 111, 0.85), rgba(4, 81, 45, 0.95))';
    } else if (theme === 'purple') {
        document.body.style.backgroundColor = '#140f29';
        document.body.style.backgroundImage = 'linear-gradient(135deg, rgba(98, 47, 145, 0.92), rgba(10, 5, 22, 0.98))';
    } else if (theme === 'custom') {
        document.body.style.backgroundColor = backgroundColorInput.value;
        document.body.style.backgroundImage = 'none';
    }
}

function updateCardColor() {
    const assistantCard = document.querySelector('.assistant-card');
    assistantCard.style.background = cardColorInput.value;
}

function speakText(text) {
    if (!window.speechSynthesis) {
        return;
    }

    const utterance = new SpeechSynthesisUtterance(text);
    const chosenLang = languageSelect.value || 'uk-UA';
    if (selectedVoice && selectedVoice.lang && selectedVoice.lang.toLowerCase().startsWith(chosenLang.toLowerCase())) {
        utterance.voice = selectedVoice;
        utterance.lang = selectedVoice.lang;
    } else {
        utterance.lang = chosenLang;
        const voices = window.speechSynthesis.getVoices();
        const matchingVoice = voices.find(v => v.lang.toLowerCase().startsWith(chosenLang.toLowerCase()));
        if (matchingVoice) {
            utterance.voice = matchingVoice;
        }
    }
    utterance.pitch = 1;
    utterance.rate = 0.95;
    window.speechSynthesis.speak(utterance);
}

async function sendMessage() {
    const text = userInput.value.trim();
    if (!text) {
        return;
    }
    appendMessage(text, 'user');
    userInput.value = '';

    try {
        const response = await fetch('/api/chat', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ message: text })
        });

        const data = await response.json();
        const reply = data.reply || 'Сталася помилка, спробуйте ще раз.';
        appendMessage(reply, 'assistant');
        speakText(reply);
    } catch (error) {
        appendMessage('Не вдалося з’єднатися з сервером.', 'assistant');
    }
}

sendButton.addEventListener('click', sendMessage);
userInput.addEventListener('keydown', event => {
    if (event.key === 'Enter') {
        sendMessage();
    }
});

const SpeechRecognition = window.SpeechRecognition || window.webkitSpeechRecognition;
const recognition = SpeechRecognition ? new SpeechRecognition() : null;
if (recognition) {
    recognition.lang = 'uk-UA';
    recognition.interimResults = false;
    recognition.maxAlternatives = 1;
}

voiceButton.addEventListener('click', () => {
    if (!recognition) {
        alert('Голосовий чат не підтримується у цьому браузері.');
        return;
    }
    if (voiceButton.classList.contains('active')) {
        recognition.stop();
    } else {
        voiceButton.classList.add('active');
        voiceButton.textContent = '■';
        recognition.start();
    }
});

if (recognition) {
    recognition.addEventListener('result', event => {
        const transcript = event.results[0][0].transcript;
        userInput.value = transcript;
        sendMessage();
    });

    recognition.addEventListener('end', () => {
        voiceButton.classList.remove('active');
        voiceButton.textContent = '🎙️';
    });

    recognition.addEventListener('error', () => {
        voiceButton.classList.remove('active');
        voiceButton.textContent = '🎙️';
    });
} else {
    voiceButton.style.opacity = '0.5';
    voiceButton.title = 'Голосовий чат не підтримується';
}

async function loadSettings() {
    try {
        const response = await fetch('/api/settings');
        const settings = await response.json();

        cardColorInput.value = settings.cardColor || '#ffffff';
        avatarElement.textContent = settings.avatarEmoji || '😊';
        if (Object.values(avatarEmojiSelect.options).map(o => o.value).includes(settings.avatarEmoji)) {
            avatarEmojiSelect.value = settings.avatarEmoji;
            customEmojiInput.value = '';
        } else {
            avatarEmojiSelect.value = '';
            customEmojiInput.value = settings.avatarEmoji || '';
        }

        languageSelect.value = settings.language || 'uk-UA';
        backgroundSelect.value = settings.backgroundTheme || 'emerald';
        backgroundColorInput.value = settings.backgroundColor || '#081d13';
        backgroundColorInput.style.display = backgroundSelect.value === 'custom' ? 'block' : 'none';

        updateBackground();
        updateCardColor();
    } catch (error) {
        console.warn('Не вдалося завантажити налаштування:', error);
    }
}

async function saveSettings() {
    const payload = {
        cardColor: cardColorInput.value,
        avatarEmoji: customEmojiInput.value.trim() || avatarEmojiSelect.value || '😊',
        language: languageSelect.value,
        backgroundTheme: backgroundSelect.value,
        backgroundColor: backgroundColorInput.value
    };

    try {
        await fetch('/api/settings', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(payload)
        });
    } catch (error) {
        console.warn('Не вдалося зберегти налаштування:', error);
    }
}

// ─── Menu / Email ──────────────────────────────────────────────
const menuButton = document.getElementById('menuButton');
const closeMenuButton = document.getElementById('closeMenuButton');
const menuOverlay = document.getElementById('menuOverlay');
const emailList = document.getElementById('emailList');
const emailView = document.getElementById('emailView');
const composeArea = document.getElementById('composeArea');
const composeButton = document.getElementById('composeButton');
const sendEmailButton = document.getElementById('sendEmailButton');
const composeTo = document.getElementById('composeTo');
const composeSubject = document.getElementById('composeSubject');
const composeBody = document.getElementById('composeBody');

function openMenu() {
    menuOverlay.classList.remove('hidden');
    menuOverlay.classList.add('visible');
    fetchEmails();
}

function closeMenu() {
    menuOverlay.classList.remove('visible');
    menuOverlay.classList.add('hidden');
    composeArea.classList.add('hidden');
    emailView.innerHTML = '<div class="email-view-placeholder">Оберіть листа щоб прочитати</div>';
}

menuButton.addEventListener('click', openMenu);
closeMenuButton.addEventListener('click', closeMenu);
menuOverlay.addEventListener('click', (e) => {
    if (e.target === menuOverlay) closeMenu();
});

composeButton.addEventListener('click', () => {
    composeArea.classList.toggle('hidden');
    if (!composeArea.classList.contains('hidden')) {
        composeTo.focus();
    }
});

async function fetchEmails() {
    try {
        const res = await fetch('/api/emails');
        const emails = await res.json();
        renderEmailList(emails);
    } catch (err) {
        emailList.innerHTML = '<div class="email-item" style="color:#8a9a8a;">Не вдалося завантажити листи.</div>';
    }
}

function renderEmailList(emails) {
    emailList.innerHTML = '';
    if (!emails || emails.length === 0) {
        emailList.innerHTML = '<div class="email-item" style="color:#8a9a8a;">Немає листів.</div>';
        return;
    }
    emails.forEach(email => {
        const div = document.createElement('div');
        div.className = 'email-item';
        div.innerHTML = `
            <div class="email-from">${escapeHtml(email.from || 'Невідомо')}</div>
            <div class="email-subject">${escapeHtml(email.subject || '(без теми)')}</div>
            <div class="email-preview">${escapeHtml((email.body || '').slice(0, 80))}</div>
        `;
        div.addEventListener('click', () => viewEmail(email));
        emailList.appendChild(div);
    });
}

function viewEmail(email) {
    composeArea.classList.add('hidden');
    emailView.innerHTML = `
        <div class="email-view-content">
            <div class="email-view-header">
                <div class="ev-from">${escapeHtml(email.from || 'Невідомо')}</div>
                <div class="ev-subject">${escapeHtml(email.subject || '(без теми)')}</div>
            </div>
            <div class="email-view-body">${escapeHtml(email.body || '')}</div>
        </div>
    `;
}

async function sendEmail() {
    const to = composeTo.value.trim();
    const subject = composeSubject.value.trim();
    const body = composeBody.value.trim();

    if (!to) {
        alert('Будь ласка, вкажіть отримувача.');
        return;
    }

    try {
        const res = await fetch('/api/emails', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ to, subject, body })
        });
        const result = await res.json();
        if (result.success) {
            composeTo.value = '';
            composeSubject.value = '';
            composeBody.value = '';
            composeArea.classList.add('hidden');
            fetchEmails();
        } else {
            alert('Помилка надсилання: ' + (result.error || 'Невідома помилка'));
        }
    } catch (err) {
        alert('Не вдалося надіслати листа.');
    }
}

sendEmailButton.addEventListener('click', sendEmail);

function escapeHtml(text) {
    const div = document.createElement('div');
    div.textContent = text;
    return div.innerHTML;
}

// ─── End Email ─────────────────────────────────────────────────

window.addEventListener('load', async () => {
    updateBackground();
    updateCardColor();
    await loadSettings();
});
