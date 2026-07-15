// ========== ДАНІ ==========

const CLIENTS = [
    { name: "Олена та Ігор Коваленки", avatar: "👫" },
    { name: "Пані Марія Петрівна", avatar: "👩" },
    { name: "Сергій та Оксана", avatar: "💑" },
    { name: "Молода сім'я Лисенків", avatar: "👨‍👩‍👧‍👦" },
    { name: "Пан Володимир", avatar: "👨" },
    { name: "Анна та Михайло", avatar: "👰" },
    { name: "Дід Панас", avatar: "👴" },
    { name: "Пані Валентина", avatar: "👩" },
    { name: "Олег-архітектор", avatar: "🧑" },
    { name: "Родина Ткаченків", avatar: "👨‍👩‍👧" }
];

const CLIENT_MESSAGES = [
    "Доброго дня! Хотіли б замовити реставрацію будинку. Візьметесь?",
    "Вітаю! Ми шукаємо майстра для ремонту будинку. Допоможете?",
    "Привіт! Нам дуже потрібно відреставрувати будинок. Кажуть, ви найкращий майстер!",
    "Добрий день! Хочемо замовити реставрацію будинку. Скільки коштуватиме?",
    "О! Ви ж реставруєте будинки? Нам дуже потрібна ваша допомога!",
    "Вітаю! Чи можете взятися за реставрацію нашого будинку?"
];

const TOOLS = [
    { id: "hammer", name: "Молоток", price: 150, desc: "Для дрібних робіт", dirt: "цвяхи, що стирчать", dirtIcon: "🔨" },
    { id: "screwdriver", name: "Набір викруток", price: 200, desc: "Для збирання", dirt: "розхитані ручки", dirtIcon: "🪛" },
    { id: "paint", name: "Фарба та пензлі", price: 500, desc: "Для фарбування стін", dirt: "обірвані стіни", dirtIcon: "🎨" },
    { id: "drill", name: "Дриль", price: 800, desc: "Для свердління", dirt: "старі дірки в стінах", dirtIcon: "🔧" },
    { id: "saw", name: "Пила", price: 600, desc: "Для різання", dirt: "старі дошки", dirtIcon: "🪚" },
    { id: "ladder", name: "Драбина", price: 400, desc: "Для роботи на висоті", dirt: "бруд під стелею", dirtIcon: "🪜" },
    { id: "plumbing", name: "Сантехнічний набір", price: 1200, desc: "Для труб", dirt: "протічка труб", dirtIcon: "🔧" },
    { id: "electrical", name: "Електричний набір", price: 1500, desc: "Для проводки", dirt: "оголені дроти", dirtIcon: "⚡" },
    { id: "tiles", name: "Плитка та клей", price: 2000, desc: "Для ванної", dirt: "відбита плитка", dirtIcon: "🔲" },
    { id: "windows", name: "Вікна та двері", price: 3000, desc: "Нові вікна", dirt: "розбиті вікна", dirtIcon: "🪟" },
    { id: "roof", name: "Матеріали для даху", price: 4000, desc: "Для покрівлі", dirt: "діри в даху", dirtIcon: "🏠" },
    { id: "facade", name: "Фасадні матеріали", price: 5000, desc: "Для оздоблення", dirt: "тріщини на стінах", dirtIcon: "🧱" }
];

// Проблеми, які можуть з'явитись після покупки інструментів
// Кожен інструмент дає новий тип проблеми, яку можна вирішити цим інструментом
const TOOL_PROBLEMS = [
    { toolId: "hammer", id: "nails", name: "Цвяхи, що стирчать", icon: "🔨", reward: 30 },
    { toolId: "screwdriver", id: "screws", name: "Розхитані ручки", icon: "🪛", reward: 30 },
    { toolId: "drill", id: "holes", name: "Старі дірки в стінах", icon: "🔧", reward: 40 },
    { toolId: "saw", id: "boards", name: "Старі дошки", icon: "🪚", reward: 40 },
    { toolId: "ladder", id: "ceiling", name: "Бруд під стелею", icon: "🪜", reward: 35 },
    { toolId: "plumbing", id: "leak", name: "Протічка труб", icon: "🔧", reward: 60 },
    { toolId: "electrical", id: "wires", name: "Оголені дроти", icon: "⚡", reward: 70 },
    { toolId: "tiles", id: "tiles", name: "Відбита плитка", icon: "🔲", reward: 80 },
    { toolId: "windows", id: "windows", name: "Розбиті вікна", icon: "🪟", reward: 100 },
    { toolId: "roof", id: "roof", name: "Діри в даху", icon: "🏠", reward: 120 },
    { toolId: "facade", id: "facade", name: "Тріщини на стінах", icon: "🧱", reward: 150 }
];



const START_TOOLS = ["hammer", "screwdriver", "paint"];

const ROOMS = [
    { id: "kitchen", name: "Кухня", icon: "🍳" },
    { id: "living", name: "Вітальня", icon: "🛋️" },
    { id: "bedroom", name: "Спальня", icon: "�️" },
    { id: "bathroom", name: "Ванна", icon: "�" },
    { id: "hall", name: "Коридор", icon: "🚪" },
    { id: "attic", name: "Горище", icon: "�" }
];

// ========== СТАН ==========

let game = {
    money: 5000,
    tools: [],
    projects: [],
    messages: [],
    level: 1,
    projectIdCounter: 0,
    messageIdCounter: 0,
    started: false,
    currentProjectId: null,
    currentRoomIndex: 0,
    selectedTool: null // "trash" або "roller"
};

// ========== ДОПОМІЖНІ ==========

function random(min, max) {
    return Math.floor(Math.random() * (max - min + 1)) + min;
}

function randomItem(arr) {
    return arr[Math.floor(Math.random() * arr.length)];
}

function formatMoney(amount) {
    return amount.toLocaleString('uk-UA') + ' $';
}

// ========== СТАРТ ==========

function startGame() {
    game.started = true;
    game.tools = [...START_TOOLS];
    game.money = 5000;
    
    document.getElementById('start-screen').classList.add('hidden');
    document.getElementById('game-ui').classList.remove('hidden');
    document.getElementById('game').classList.add('started');
    document.getElementById('game').style.display = 'block';
    
    updateUI();
    showNotification("Гра розпочата! Ви отримали 3 інструменти та $5000. Чекайте на замовлення!");
    
    setTimeout(function() {
        generateNewOrder();
    }, 2000);
    
    setInterval(function() {
        if (Math.random() < 0.5) {
            generateNewOrder();
        }
    }, random(15000, 25000));
}

// ========== ГЕНЕРАЦІЯ ЗАМОВЛЕНЬ ==========

function generateNewOrder() {
    var client = randomItem(CLIENTS);
    var reward = random(3000, 8000) + game.level * 500;
    
    // Генеруємо проблеми в кімнатах
    var rooms = {};
    for (var i = 0; i < ROOMS.length; i++) {
        var room = ROOMS[i];
        rooms[room.id] = {
            hasTrash: Math.random() < 0.7,
            hasWallDamage: Math.random() < 0.6,
            trashFixed: false,
            wallFixed: false,
            // Проблеми від інструментів: { problemId: false } — false означає "ще не виправлено"
            toolProblems: {}
        };
    }
    
    // Додаємо проблеми від інструментів, які є у гравця
    // Кожен інструмент може створити проблему у випадковій кімнаті
    for (var t = 0; t < game.tools.length; t++) {
        var toolId = game.tools[t];
        var problemDef = TOOL_PROBLEMS.find(function(p) { return p.toolId === toolId; });
        if (!problemDef) continue;
        
        // 90% шанс, що проблема з'явиться (майже гарантовано)
        if (Math.random() < 0.9) {
            // Вибираємо випадкову кімнату
            var randomRoom = randomItem(ROOMS);
            rooms[randomRoom.id].toolProblems[problemDef.id] = false;
        }
    }
    
    var project = {
        id: ++game.projectIdCounter,
        clientName: client.name,
        clientAvatar: client.avatar,
        rooms: rooms,
        reward: reward,
        completed: false,
        accepted: false
    };

    
    var msgText = randomItem(CLIENT_MESSAGES);
    var msg = {
        id: ++game.messageIdCounter,
        clientName: client.name,
        clientAvatar: client.avatar,
        text: msgText,
        projectId: project.id,
        time: new Date().toLocaleTimeString('uk-UA', { hour: '2-digit', minute: '2-digit' }),
        read: false,
        answered: false
    };
    
    game.messages.unshift(msg);
    game.projects.push(project);
    
    updateUI();
    updateBadge();
    showNotification("Нове замовлення від " + client.name + "!");
}

// ========== ВЗЯТИ ЗАМОВЛЕННЯ ==========

function acceptOrder(msgId) {
    var msg = game.messages.find(function(m) { return m.id === msgId; });
    if (!msg || msg.answered) return;
    
    msg.answered = true;
    msg.read = true;
    
    var project = game.projects.find(function(p) { return p.id === msg.projectId; });
    if (project) {
        project.accepted = true;
        showNotification("Ви взяли замовлення! Заходьте в будинок!");
    }
    
    updateUI();
}

function declineOrder(msgId) {
    var msg = game.messages.find(function(m) { return m.id === msgId; });
    if (!msg || msg.answered) return;
    
    msg.answered = true;
    msg.read = true;
    
    game.projects = game.projects.filter(function(p) { return p.id !== msg.projectId; });
    
    showNotification("Ви відмовились від замовлення");
    updateUI();
}

// ========== ЗАЙТИ В БУДИНОК ==========


function enterHouse(projectId) {
    game.currentProjectId = projectId;
    game.currentRoomIndex = 0;
    game.selectedTool = null;
    
    var allTabs = document.querySelectorAll('.tab');
    for (var i = 0; i < allTabs.length; i++) {
        allTabs[i].classList.remove('active');
    }
    document.getElementById('house-location').classList.add('active');
    
    document.querySelector('nav').style.display = 'none';
    
    // Оновлюємо інвентар
    updateInventory();
    
    // Скидаємо активний слот
    var slots = document.querySelectorAll('.inv-slot');
    for (var i = 0; i < slots.length; i++) {
        slots[i].classList.remove('active');
    }
    
    // Ховаємо canvas
    var canvas = document.getElementById('paint-canvas');
    canvas.style.display = 'none';
    
    renderRoom();
}


function backFromHouse() {
    game.currentProjectId = null;
    game.selectedTool = null;
    
    document.getElementById('house-location').classList.remove('active');
    document.querySelector('nav').style.display = 'flex';
    
    var allTabs = document.querySelectorAll('.tab');
    for (var i = 0; i < allTabs.length; i++) {
        allTabs[i].classList.remove('active');
    }
    document.getElementById('projects').classList.add('active');
    
    var navBtns = document.querySelectorAll('.nav-btn');
    for (var i = 0; i < navBtns.length; i++) {
        navBtns[i].classList.remove('active');
        if (navBtns[i].dataset.tab === 'projects') {
            navBtns[i].classList.add('active');
        }
    }
    
    // Ховаємо canvas
    var canvas = document.getElementById('paint-canvas');
    canvas.style.display = 'none';
    
    updateUI();
}

// ========== ВИБІР ІНСТРУМЕНТУ ==========

function selectTool(tool) {
    if (!game.currentProjectId) return;
    
    game.selectedTool = tool;
    
    var slots = document.querySelectorAll('.inv-slot');
    for (var i = 0; i < slots.length; i++) {
        slots[i].classList.remove('active');
    }
    
    if (tool === "trash") {
        document.getElementById('slot-trash').classList.add('active');
        document.getElementById('tool-hint').textContent = "🖱️ Клікніть на сміття, щоб викинути у відро";
        document.getElementById('paint-canvas').style.display = 'none';
        document.getElementById('room-scene').style.cursor = 'pointer';
    } else if (tool === "roller") {
        document.getElementById('slot-roller').classList.add('active');
        document.getElementById('tool-hint').textContent = "🖱️ Клікніть на плями на стіні, щоб зафарбувати";
        document.getElementById('paint-canvas').style.display = 'none';
        document.getElementById('room-scene').style.cursor = 'crosshair';
    } else {
        // Вибір інструменту для toolProblems
        var slot = document.getElementById('slot-' + tool);
        if (slot) {
            slot.classList.add('active');
            var problemDef = TOOL_PROBLEMS.find(function(p) { return p.toolId === tool; });
            if (problemDef) {
                document.getElementById('tool-hint').textContent = "🖱️ Клікніть на " + problemDef.name.toLowerCase() + ", щоб виправити";
            }
            document.getElementById('paint-canvas').style.display = 'none';
            document.getElementById('room-scene').style.cursor = 'pointer';
        }
    }
}


// ========== CANVAS ДЛЯ ФАРБУВАННЯ (візуальний ефект) ==========

var paintCanvas = null;
var paintCtx = null;

function setupCanvas() {
    var canvas = document.getElementById('paint-canvas');
    var scene = document.getElementById('room-scene');
    
    canvas.width = scene.offsetWidth;
    canvas.height = scene.offsetHeight;
    
    paintCanvas = canvas;
    paintCtx = canvas.getContext('2d');
}

// ========== РЕНДЕР КІМНАТИ ==========

function renderRoom() {
    var project = game.projects.find(function(p) { return p.id === game.currentProjectId; });
    if (!project) return;
    
    var roomId = ROOMS[game.currentRoomIndex].id;
    var room = project.rooms[roomId];
    var roomInfo = ROOMS[game.currentRoomIndex];
    
    document.getElementById('location-client').textContent = "👤 " + project.clientName;
    document.getElementById('room-name-display').textContent = roomInfo.icon + " " + roomInfo.name;
    
    // Прогрес (включаючи toolProblems)
    var total = 0;
    var done = 0;
    for (var rId in project.rooms) {
        var r = project.rooms[rId];
        if (r.hasTrash) total++;
        if (r.hasWallDamage) total++;
        if (r.trashFixed) done++;
        if (r.wallFixed) done++;
        // Проблеми від інструментів
        for (var pId in r.toolProblems) {
            total++;
            if (r.toolProblems[pId]) done++;
        }
    }
    var progress = total > 0 ? Math.round((done / total) * 100) : 100;
    document.getElementById('loc-progress-val').textContent = progress;
    
    // Оновлюємо сцену
    var scene = document.getElementById('room-scene');
    var trashEl = document.getElementById('trash');
    var wallDamageEl = document.getElementById('wall-damage');
    var statusBadge = document.getElementById('room-status-badge');
    
    // Показуємо/ховаємо сміття
    if (room.hasTrash && !room.trashFixed) {
        trashEl.style.display = 'flex';
    } else {
        trashEl.style.display = 'none';
    }
    
    // Показуємо/ховаємо пошкодження стін
    if (room.hasWallDamage && !room.wallFixed) {
        wallDamageEl.style.display = 'block';
    } else {
        wallDamageEl.style.display = 'none';
    }
    
    // Показуємо/ховаємо візуальні toolProblems
    var toolProblemsContainer = document.getElementById('tool-problems');
    if (toolProblemsContainer) {
        var hasVisibleProblems = false;
        // Ховаємо всі візуальні елементи проблем
        var allViz = toolProblemsContainer.querySelectorAll('.tool-problem-viz');
        for (var v = 0; v < allViz.length; v++) {
            allViz[v].style.display = 'none';
        }
        // Показуємо тільки ті, що є в поточній кімнаті
        for (var pId in room.toolProblems) {
            if (!room.toolProblems[pId]) {
                hasVisibleProblems = true;
                var vizEl = document.getElementById('prob-' + pId);
                if (vizEl) {
                    vizEl.style.display = 'flex';
                }
            }
        }
        toolProblemsContainer.style.display = hasVisibleProblems ? 'block' : 'none';
    }
    
    // Статус кімнати
    var roomClean = isRoomClean(room);
    if (roomClean) {
        scene.classList.add('clean');
        statusBadge.textContent = "✅ Чисто";
        statusBadge.style.background = "rgba(76, 175, 80, 0.8)";
    } else {
        scene.classList.remove('clean');
        statusBadge.textContent = "⚠️ Потребує ремонту";
        statusBadge.style.background = "rgba(255, 68, 68, 0.8)";
    }
    
    // Ховаємо всі елементи кімнат
    var allRoomItems = document.querySelectorAll('.room-item');
    for (var i = 0; i < allRoomItems.length; i++) {
        allRoomItems[i].style.display = 'none';
    }
    
    // Показуємо елементи поточної кімнати
    var roomClass = roomId + '-item';
    var currentItems = document.querySelectorAll('.' + roomClass);
    for (var i = 0; i < currentItems.length; i++) {
        currentItems[i].style.display = 'block';
    }
    
    // Спеціальна обробка для горища (boxes мають display:flex)
    if (roomId === 'attic') {
        document.getElementById('attic-boxes').style.display = 'flex';
    }
    
    // Очищаємо canvas при переході між кімнатами
    var canvas = document.getElementById('paint-canvas');
    if (canvas) {
        var ctx = canvas.getContext('2d');
        if (ctx) {
            ctx.clearRect(0, 0, canvas.width, canvas.height);
        }
        canvas.style.display = 'none';
    }
    
    // Скидаємо вибраний інструмент при переході
    game.selectedTool = null;
    var slots = document.querySelectorAll('.inv-slot');
    for (var i = 0; i < slots.length; i++) {
        slots[i].classList.remove('active');
    }
    document.getElementById('tool-hint').textContent = "🖱️ Клікніть на інструмент в інвентарі, щоб вибрати";
    document.getElementById('room-scene').style.cursor = 'default';
}

// ========== ОНОВЛЕННЯ ІНВЕНТАРЮ ==========


function updateInventory() {
    var slotsContainer = document.querySelector('.inv-slots');
    if (!slotsContainer) return;
    
    // Базові слоти (відро та валик) завжди є
    var html = '';
    
    // Відро
    html += '<div class="inv-slot" data-tool="trash" id="slot-trash">' +
        '<div class="inv-icon">🗑️</div>' +
        '<div class="inv-key">1</div>' +
        '<div class="inv-name">Відро</div>' +
        '</div>';
    
    // Валик
    html += '<div class="inv-slot" data-tool="roller" id="slot-roller">' +
        '<div class="inv-icon">🎨</div>' +
        '<div class="inv-key">2</div>' +
        '<div class="inv-name">Валик</div>' +
        '</div>';
    
    // Інструменти гравця (крім paint, який вже використовується валиком)
    for (var i = 0; i < game.tools.length; i++) {
        var toolId = game.tools[i];
        var toolDef = TOOLS.find(function(t) { return t.id === toolId; });
        if (!toolDef) continue;
        
        // Пропускаємо paint, бо він використовується через валик
        if (toolId === 'paint') continue;
        
        var problemDef = TOOL_PROBLEMS.find(function(p) { return p.toolId === toolId; });
        var icon = problemDef ? problemDef.icon : toolDef.dirtIcon;
        
        html += '<div class="inv-slot" data-tool="' + toolId + '" id="slot-' + toolId + '">' +
            '<div class="inv-icon">' + icon + '</div>' +
            '<div class="inv-name">' + toolDef.name + '</div>' +
            '</div>';
    }
    
    slotsContainer.innerHTML = html;
    
    // Додаємо обробники кліків для нових слотів (через делегування на контейнер)
    // Видаляємо старий обробник, якщо він був, щоб уникнути дублікатів
    if (!slotsContainer._listenerAttached) {
        slotsContainer.addEventListener('click', function(e) {
            var slot = e.target.closest('.inv-slot');
            if (slot) {
                var tool = slot.dataset.tool;
                selectTool(tool);
            }
        });
        slotsContainer._listenerAttached = true;
    }
}

// Перевірка, чи кімната чиста (включаючи toolProblems)

function isRoomClean(room) {
    if (room.hasTrash && !room.trashFixed) return false;
    if (room.hasWallDamage && !room.wallFixed) return false;
    for (var pId in room.toolProblems) {
        if (!room.toolProblems[pId]) return false;
    }
    return true;
}


// ========== ДІЇ ЗІ СМІТТЯМ ==========

function handleTrashClick(e) {
    if (game.selectedTool !== "trash") {
        showNotification("Виберіть відро (клавіша 1)!");
        return;
    }
    
    var project = game.projects.find(function(p) { return p.id === game.currentProjectId; });
    if (!project) return;
    
    var roomId = ROOMS[game.currentRoomIndex].id;
    var room = project.rooms[roomId];
    
    if (!room.hasTrash || room.trashFixed) return;
    
    // Анімація - сміття летить у відро
    var trashEl = document.getElementById('trash');
    var slotTrash = document.getElementById('slot-trash');
    
    // Отримуємо координати
    var trashRect = trashEl.getBoundingClientRect();
    var slotRect = slotTrash.getBoundingClientRect();
    
    // Створюємо летюче сміття
    var flyingTrash = document.createElement('div');
    flyingTrash.textContent = '🗑️';
    flyingTrash.style.cssText = 'position:fixed;font-size:2em;z-index:9999;pointer-events:none;' +
        'transition:all 0.5s cubic-bezier(0.25, 0.46, 0.45, 0.94);' +
        'left:' + trashRect.left + 'px;top:' + trashRect.top + 'px;';
    document.body.appendChild(flyingTrash);
    
    // Анімуємо політ до відра
    setTimeout(function() {
        flyingTrash.style.left = slotRect.left + 'px';
        flyingTrash.style.top = slotRect.top + 'px';
        flyingTrash.style.transform = 'scale(0.3) rotate(360deg)';
        flyingTrash.style.opacity = '0.5';
    }, 10);
    
    // Завершуємо анімацію
    setTimeout(function() {
        document.body.removeChild(flyingTrash);
        
        room.trashFixed = true;
        showNotification("🗑️ Сміття викинуто! +50$");
        game.money += 50;
        
        // Скидаємо вибраний інструмент
        game.selectedTool = null;
        var slots = document.querySelectorAll('.inv-slot');
        for (var i = 0; i < slots.length; i++) {
            slots[i].classList.remove('active');
        }
        document.getElementById('tool-hint').textContent = "🖱️ Клікніть на інструмент в інвентарі, щоб вибрати";
        document.getElementById('room-scene').style.cursor = 'default';
        
        checkAllDone(project);
        renderRoom();
        updateUI();
    }, 550);
}

// ========== ДІЇ ЗІ СТІНАМИ ==========

function handleWallClick(e) {
    if (game.selectedTool !== "roller") {
        showNotification("Виберіть валик (клавіша 2)!");
        return;
    }
    
    var project = game.projects.find(function(p) { return p.id === game.currentProjectId; });
    if (!project) return;
    
    var roomId = ROOMS[game.currentRoomIndex].id;
    var room = project.rooms[roomId];
    
    if (!room.hasWallDamage || room.wallFixed) return;
    
    if (game.tools.indexOf("paint") === -1) {
        showNotification("Вам потрібна фарба! Купіть у майстерні.");
        return;
    }
    
    // Візуальний ефект - "сплеск" фарби в місці кліку
    var wallEl = document.getElementById('wall-damage');
    var rect = wallEl.getBoundingClientRect();
    var sceneRect = document.getElementById('room-scene').getBoundingClientRect();
    
    var splash = document.createElement('div');
    splash.textContent = '🎨';
    splash.style.cssText = 'position:fixed;font-size:2.5em;z-index:9999;pointer-events:none;' +
        'left:' + (e.clientX - 20) + 'px;top:' + (e.clientY - 20) + 'px;' +
        'transition:all 0.6s ease;opacity:1;transform:scale(0.5);';
    document.body.appendChild(splash);
    
    setTimeout(function() {
        splash.style.transform = 'scale(2) rotate(20deg)';
        splash.style.opacity = '0';
    }, 10);
    
    setTimeout(function() {
        document.body.removeChild(splash);
        
        // Одразу фарбуємо стіну
        room.wallFixed = true;
        
        game.selectedTool = null;
        var slots = document.querySelectorAll('.inv-slot');
        for (var i = 0; i < slots.length; i++) {
            slots[i].classList.remove('active');
        }
        document.getElementById('tool-hint').textContent = "🖱️ Клікніть на інструмент в інвентарі, щоб вибрати";
        document.getElementById('room-scene').style.cursor = 'default';
        
        showNotification("🧱 Стіни пофарбовано! +80$");
        game.money += 80;
        
        checkAllDone(project);
        renderRoom();
        updateUI();
    }, 650);
}

function checkAllDone(project) {
    var allDone = true;
    for (var rId in project.rooms) {
        var r = project.rooms[rId];
        if (r.hasTrash && !r.trashFixed) { allDone = false; break; }
        if (r.hasWallDamage && !r.wallFixed) { allDone = false; break; }
        for (var pId in r.toolProblems) {
            if (!r.toolProblems[pId]) { allDone = false; break; }
        }
    }
    
    if (allDone && !project.completed) {
        showNotification("🎉 Весь будинок готовий! Клієнт чекає!");
    }
}


// ========== НАВІГАЦІЯ КІМНАТАМИ ==========

function nextRoom() {
    if (game.currentRoomIndex < ROOMS.length - 1) {
        game.currentRoomIndex++;
        renderRoom();
    }
}

function prevRoom() {
    if (game.currentRoomIndex > 0) {
        game.currentRoomIndex--;
        renderRoom();
    }
}

// ========== ДІЇ З ПРОБЛЕМАМИ ВІД ІНСТРУМЕНТІВ ==========

function handleToolProblemClick(e) {
    var target = e.target.closest('.tool-problem-viz');
    if (!target) return;
    
    var problemId = target.dataset.problemId;
    var toolId = target.dataset.toolId;
    
    if (game.selectedTool !== toolId) {
        var problemDef = TOOL_PROBLEMS.find(function(p) { return p.id === problemId; });
        var toolDef = TOOLS.find(function(t) { return t.id === toolId; });
        showNotification("Виберіть " + (toolDef ? toolDef.name : "потрібний інструмент") + "!");
        return;
    }
    
    var project = game.projects.find(function(p) { return p.id === game.currentProjectId; });
    if (!project) return;
    
    var roomId = ROOMS[game.currentRoomIndex].id;
    var room = project.rooms[roomId];
    
    if (room.toolProblems[problemId] === undefined || room.toolProblems[problemId]) return;
    
    // Анімація виправлення
    var problemDef = TOOL_PROBLEMS.find(function(p) { return p.id === problemId; });
    
    var splash = document.createElement('div');
    splash.textContent = problemDef.icon;
    splash.style.cssText = 'position:fixed;font-size:2.5em;z-index:9999;pointer-events:none;' +
        'left:' + (e.clientX - 20) + 'px;top:' + (e.clientY - 20) + 'px;' +
        'transition:all 0.5s ease;opacity:1;transform:scale(0.5) rotate(-20deg);';
    document.body.appendChild(splash);
    
    setTimeout(function() {
        splash.style.transform = 'scale(1.5) rotate(20deg)';
        splash.style.opacity = '0';
    }, 10);
    
    setTimeout(function() {
        document.body.removeChild(splash);
        
        room.toolProblems[problemId] = true;
        
        game.selectedTool = null;
        var slots = document.querySelectorAll('.inv-slot');
        for (var i = 0; i < slots.length; i++) {
            slots[i].classList.remove('active');
        }
        document.getElementById('tool-hint').textContent = "🖱️ Клікніть на інструмент в інвентарі, щоб вибрати";
        document.getElementById('room-scene').style.cursor = 'default';
        
        showNotification("✅ " + problemDef.name + " виправлено! +" + problemDef.reward + "$");
        game.money += problemDef.reward;
        
        checkAllDone(project);
        renderRoom();
        updateUI();
    }, 550);
}

// ========== ЗДАТИ ПРОЄКТ ==========

function completeProject(projectId) {
    var project = game.projects.find(function(p) { return p.id === projectId; });
    if (!project || !project.accepted || project.completed) return;
    
    var allDone = true;
    for (var rId in project.rooms) {
        var r = project.rooms[rId];
        if (r.hasTrash && !r.trashFixed) { allDone = false; break; }
        if (r.hasWallDamage && !r.wallFixed) { allDone = false; break; }
        for (var pId in r.toolProblems) {
            if (!r.toolProblems[pId]) { allDone = false; break; }
        }
    }
    
    if (!allDone) {
        showNotification("Ще не все готово! Пройдіться по кімнатах.");
        return;
    }
    
    project.completed = true;
    game.money += project.reward;
    
    if (game.level < 10 && Math.random() < 0.4) {
        game.level++;
        showNotification("Вітаємо! Ви досягли " + game.level + "-го рівня!");
    }
    
    showNotification("Проєкт здано! Отримано " + formatMoney(project.reward));
    
    game.messages = game.messages.filter(function(m) { return m.projectId !== projectId; });
    
    if (game.currentProjectId === projectId) {
        backFromHouse();
    }
    
    updateUI();
}


// ========== МАГАЗИН ==========

function buyTool(toolId) {
    var tool = TOOLS.find(function(t) { return t.id === toolId; });
    if (!tool) return;
    
    if (game.money < tool.price) {
        showNotification("Недостатньо грошей!");
        return;
    }
    
    if (game.tools.indexOf(toolId) !== -1) {
        showNotification("Цей інструмент уже є!");
        return;
    }
    
    game.money -= tool.price;
    game.tools.push(toolId);
    
    // Додаємо нові проблеми в активні проєкти
    var problemDef = TOOL_PROBLEMS.find(function(p) { return p.toolId === toolId; });
    if (problemDef) {
        var activeProjects = game.projects.filter(function(p) { return p.accepted && !p.completed; });
        if (activeProjects.length > 0) {
            for (var ap = 0; ap < activeProjects.length; ap++) {
                var proj = activeProjects[ap];
                // Додаємо проблему у випадкову кімнату (100% шанс для активних)
                var randomRoom = randomItem(ROOMS);
                proj.rooms[randomRoom.id].toolProblems[problemDef.id] = false;
                showNotification("⚠️ У будинку " + proj.clientName + " з'явилась проблема: " + problemDef.name + "!");
            }
        } else {
            // Якщо немає активних проєктів, генеруємо нове замовлення одразу,
            // щоб гравець побачив проблеми в новому будинку
            showNotification("🔧 Тепер ви можете виправляти " + problemDef.name.toLowerCase() + "! Нове замовлення вже в дорозі...");
            setTimeout(function() {
                generateNewOrder();
            }, 1000);
        }
    }
    
    showNotification("Придбано " + tool.name);
    updateUI();
    
    // Якщо гравець зараз в будинку, оновлюємо інвентар і кімнату одразу
    if (game.currentProjectId) {
        updateInventory();
        renderRoom();
    }
}

// ========== ОНОВЛЕННЯ ==========

function updateUI() {
    updateStats();
    renderMessages();
    renderShop();
    renderProjects();
    updateBadge();
}

function updateStats() {
    document.getElementById('money').textContent = formatMoney(game.money);
    document.getElementById('tools-count').textContent = game.tools.length;
    document.getElementById('houses-active').textContent = game.projects.filter(function(p) { return p.accepted && !p.completed; }).length;
    document.getElementById('level').textContent = game.level;
}

function renderMessages() {
    var container = document.getElementById('messages-list');
    
    if (game.messages.length === 0) {
        container.innerHTML = '<p style="text-align:center;color:#666;padding:40px;">Поки що немає замовлень. Клієнти скоро напишуть!</p>';
        return;
    }
    
    var html = '';
    for (var i = 0; i < game.messages.length; i++) {
        var msg = game.messages[i];
        var project = game.projects.find(function(p) { return p.id === msg.projectId; });
        var unreadStyle = !msg.read ? 'border-left: 3px solid #ffd200;' : '';
        var newBadge = !msg.read ? ' NEW' : '';
        
        var actionsHtml = '';
        if (!msg.answered) {
            actionsHtml = '<div class="msg-actions">' +
                '<button class="btn" onclick="acceptOrder(' + msg.id + ')">Взяти замовлення</button>' +
                '<button class="btn btn-secondary" onclick="declineOrder(' + msg.id + ')">Відмовитись</button>' +
                '</div>';
        } else {
            actionsHtml = '<div style="color:#4caf50;font-size:0.85em;margin-top:5px;">Відповідь надіслано</div>';
        }
        
        var projectInfo = '';
        if (project) {
            projectInfo = '<div style="font-size:0.85em;color:#888;margin-top:5px;">Винагорода: ' + formatMoney(project.reward) + '</div>';
        }
        
        html += '<div class="message" style="' + unreadStyle + '">' +
            '<div class="avatar">' + msg.clientAvatar + '</div>' +
            '<div class="content">' +
            '<div class="name">' + msg.clientName + newBadge + '</div>' +
            '<div class="text">' + msg.text + '</div>' +
            projectInfo +
            '<div class="time">' + msg.time + '</div>' +
            actionsHtml +
            '</div>' +
            '</div>';
    }
    container.innerHTML = html;
}

function renderShop() {
    var container = document.getElementById('shop-list');
    var html = '';
    
    for (var i = 0; i < TOOLS.length; i++) {
        var tool = TOOLS[i];
        var owned = game.tools.indexOf(tool.id) !== -1;
        var btnClass = owned ? 'btn btn-secondary' : 'btn';
        var disabled = owned || game.money < tool.price;
        var btnText = owned ? 'Вже є' : (game.money < tool.price ? 'Не вистачає' : 'Купити');
        
        html += '<div class="card">' +
            '<h3>' + tool.name + '</h3>' +
            '<div class="desc">' + tool.desc + '</div>' +
            '<div class="price">' + formatMoney(tool.price) + '</div>' +
            '<button class="' + btnClass + '" onclick="buyTool(\'' + tool.id + '\')"' + (disabled ? ' disabled' : '') + '>' +
            btnText +
            '</button>' +
            '</div>';
    }
    container.innerHTML = html;
}

function renderProjects() {
    var container = document.getElementById('projects-list');
    var active = game.projects.filter(function(p) { return p.accepted && !p.completed; });
    
    if (active.length === 0) {
        container.innerHTML = '<p style="text-align:center;color:#666;padding:40px;">Немає активних проєктів. Візьміть замовлення в повідомленнях!</p>';
        return;
    }
    
    var html = '';
    for (var i = 0; i < active.length; i++) {
        var project = active[i];
        
        var total = 0;
        var done = 0;
        for (var rId in project.rooms) {
            var r = project.rooms[rId];
            if (r.hasTrash) total++;
            if (r.hasWallDamage) total++;
            if (r.trashFixed) done++;
            if (r.wallFixed) done++;
            for (var pId in r.toolProblems) {
                total++;
                if (r.toolProblems[pId]) done++;
            }
        }
        var progress = total > 0 ? Math.round((done / total) * 100) : 100;
        var allDone = done === total && total > 0;
        
        html += '<div class="project-card">' +
            '<h3>🏚️ Реставрація будинку</h3>' +
            '<div class="client-info">Клієнт: ' + project.clientAvatar + ' ' + project.clientName + '</div>' +
            '<div style="font-size:0.9em;color:#888;margin:10px 0;">Винагорода: ' + formatMoney(project.reward) + '</div>' +
            '<div style="margin:10px 0;">' +
            'Прогрес: ' + progress + '%' +
            '<div class="condition-bar">' +
            '<div class="condition-fill ' + (progress >= 100 ? 'perfect' : 'good') + '" style="width:' + progress + '%"></div>' +
            '</div>' +
            '</div>' +
            '<button class="btn" onclick="enterHouse(' + project.id + ')">🔍 Зайти в будинок</button>' +
            '<button class="btn btn-success" onclick="completeProject(' + project.id + ')"' + (!allDone ? ' disabled' : '') + '>' +
            (allDone ? '💰 Здати проєкт' : '⏳ Ще не все готово') +
            '</button>' +
            '</div>';
    }
    container.innerHTML = html;
}


function updateBadge() {
    var unread = 0;
    for (var i = 0; i < game.messages.length; i++) {
        if (!game.messages[i].read) unread++;
    }
    var badge = document.getElementById('msg-badge');
    if (unread > 0) {
        badge.textContent = unread;
        badge.classList.remove('hidden');
    } else {
        badge.classList.add('hidden');
    }
}

// ========== НАВІГАЦІЯ ==========

var navBtns = document.querySelectorAll('.nav-btn');
for (var i = 0; i < navBtns.length; i++) {
    navBtns[i].addEventListener('click', function() {
        if (game.currentProjectId) return;
        
        var allBtns = document.querySelectorAll('.nav-btn');
        for (var j = 0; j < allBtns.length; j++) {
            allBtns[j].classList.remove('active');
        }
        var allTabs = document.querySelectorAll('.tab');
        for (var j = 0; j < allTabs.length; j++) {
            allTabs[j].classList.remove('active');
        }
        
        this.classList.add('active');
        document.getElementById(this.dataset.tab).classList.add('active');
        
        if (this.dataset.tab === 'messages') {
            for (var j = 0; j < game.messages.length; j++) {
                game.messages[j].read = true;
            }
            updateBadge();
        }
    });
}

// Кнопки
document.getElementById('back-btn').addEventListener('click', backFromHouse);
document.getElementById('room-prev').addEventListener('click', prevRoom);
document.getElementById('room-next').addEventListener('click', nextRoom);

// Клік на сміття
document.getElementById('trash').addEventListener('click', handleTrashClick);

// Клік на пошкодження стін
document.getElementById('wall-damage').addEventListener('click', handleWallClick);

// Клік на проблеми від інструментів
document.getElementById('tool-problems').addEventListener('click', handleToolProblemClick);

// ========== КЛАВІШІ ==========


document.addEventListener('keydown', function(e) {
    if (!game.currentProjectId) return;
    
    if (e.key === '1') {
        e.preventDefault();
        selectTool('trash');
    } else if (e.key === '2') {
        e.preventDefault();
        selectTool('roller');
    }
});

// ========== ПОВІДОМЛЕННЯ ==========

function showNotification(text) {
    var el = document.getElementById('notification');
    el.textContent = text;
    el.classList.remove('hidden');
    
    el.style.animation = 'none';
    el.offsetHeight;
    el.style.animation = 'fadeInOut 3s ease forwards';
    
    setTimeout(function() {
        el.classList.add('hidden');
    }, 3000);
}

// ========== ЗАПУСК ==========

document.getElementById('start-btn').addEventListener('click', startGame);

console.log('House Flopper завантажено! Натисніть "Почати гру!"');
