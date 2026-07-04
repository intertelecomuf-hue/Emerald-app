// Game state
const game = {
    resources: {
        wood: 0,
        stone: 0,
        glass: 0,
        nails: 0,
        coins: 5
    },
    tools: {
        axe: { level: 1, name: 'Стара сокира', bonus: 0, durability: 10, maxDurability: 10, treesPerDura: 5 },
        pickaxe: { level: 0, name: '—', bonus: 0, durability: 0, maxDurability: 0 }
    },
    selectedTool: 'axe',
    house: {
        floor: false, walls: false, roof: false,
        window: false, door: false, balcony: false,
        fence: false, ladder: false, flag: false,
        level: 1
    },
    stats: {
        actionsDone: 0,
        comfort: 0,
        treesCut: 0
    },
    interior: {
        inside: false,
        items: {},
        maxSlots: 5
    },
    view: {
        offset: 0,
        isChoping: false
    },
    trees: [
        { id: 'tree-1', x: 80, hits: 5, maxHits: 5, alive: true },
        { id: 'tree-2', x: 250, hits: 5, maxHits: 5, alive: true },
        { id: 'tree-3', x: 420, hits: 5, maxHits: 5, alive: true },
        { id: 'tree-4', x: 590, hits: 5, maxHits: 5, alive: true }
    ],
    rocks: [
        { id: 'rock-1', x: 140, hits: 3, maxHits: 3, alive: true },
        { id: 'rock-2', x: 340, hits: 3, maxHits: 3, alive: true },
        { id: 'rock-3', x: 530, hits: 3, maxHits: 3, alive: true }
    ]
};

// Select tool
function selectTool(tool) {
    game.selectedTool = tool;
    document.querySelectorAll('.inv-slot').forEach(s => s.classList.remove('active'));
    document.getElementById(`slot-${tool}`).classList.add('active');
    
    const toolData = game.tools[tool];
    const icon = tool === 'axe' ? '🪓' : '⛏️';
    document.getElementById('fp-hand-tool').textContent = icon;
    document.getElementById('current-tool').textContent = `${icon} ${toolData.name}`;
    document.getElementById('tool-durability').textContent = toolData.durability;
    document.getElementById('tool-maxdura').textContent = toolData.maxDurability;
}

// Look around
function lookAround(dx) {
    game.view.offset = Math.max(-200, Math.min(200, game.view.offset + dx));
    
    document.querySelectorAll('.fp-tree').forEach(el => {
        el.style.transform = `translateX(${game.view.offset}px)`;
    });
    document.querySelectorAll('.fp-rock').forEach(el => {
        el.style.transform = `translateX(${game.view.offset}px)`;
    });
    document.getElementById('fp-home-tree').style.transform = `translateX(calc(-50% + ${game.view.offset}px))`;
}

// Get target based on selected tool (axe → trees, pickaxe → rocks)
function getTarget() {
    const centerX = 350 + game.view.offset;
    let nearest = null;
    let minDist = Infinity;
    let type = null;
    
    if (game.selectedTool === 'axe') {
        // Check only trees
        for (const tree of game.trees) {
            if (!tree.alive) continue;
            const dist = Math.abs(tree.x - centerX);
            if (dist < minDist) {
                minDist = dist;
                nearest = tree;
                type = 'tree';
            }
        }
    } else if (game.selectedTool === 'pickaxe') {
        // Check only rocks
        for (const rock of game.rocks) {
            if (!rock.alive) continue;
            const dist = Math.abs(rock.x - centerX);
            if (dist < minDist) {
                minDist = dist;
                nearest = rock;
                type = 'rock';
            }
        }
    }
    
    return (nearest && minDist < 120) ? { target: nearest, type } : null;
}

// Hit (chop/mine)
function hitTarget(e) {
    if (e) e.preventDefault();
    if (game.view.isChoping) return;
    
    const result = getTarget();
    if (!result) {
        const toolName = game.selectedTool === 'axe' ? 'дерева' : 'каменя';
        addMessage(`❌ Поруч немає ${toolName}!`, 'error');
        return;
    }
    
    const tool = game.tools[game.selectedTool];
    if (tool.durability <= 0) {
        addMessage(`❌ ${tool.name} зламалася! Покращте її.`, 'error');
        return;
    }
    
    // Start animation
    game.view.isChoping = true;
    const worldEl = document.getElementById('game-world');
    worldEl.classList.add('chop');
    
    // Reduce durability
    tool.durability--;
    
    // Hit
    result.target.hits--;
    game.stats.actionsDone++;
    
    const el = document.getElementById(result.target.id);
    const hitsEl = el.querySelector(`.${result.type === 'tree' ? 'fp-tree' : 'fp-rock'}-hits`);
    if (hitsEl) hitsEl.textContent = result.target.hits + ' ударів';
    
    // Shake
    el.style.animation = 'none';
    el.offsetHeight;
    el.style.animation = 'targetShake 0.3s ease';
    
    if (result.target.hits <= 0) {
        result.target.alive = false;
        
        let resourceAmount = 0;
        let resourceName = '';
        
        if (result.type === 'tree') {
            resourceAmount = Math.floor(Math.random() * 3) + 2 + game.tools.axe.bonus;
            game.resources.wood += resourceAmount;
            game.stats.treesCut++;
            resourceName = 'деревини';
        } else {
            resourceAmount = Math.floor(Math.random() * 2) + 2;
            game.resources.stone += resourceAmount;
            resourceName = 'каміння';
        }
        
        // Fall animation
        el.style.transition = 'all 0.6s';
        if (result.type === 'tree') {
            el.style.transform = `translateX(${game.view.offset}px) rotate(90deg) scale(0)`;
        } else {
            el.style.transform = `translateX(${game.view.offset}px) scale(0)`;
        }
        el.style.opacity = '0';
        
        addMessage(`✅ Видобуто! +${resourceAmount} ${resourceName}.`, 'success');
        
        // Respawn
        setTimeout(() => {
            result.target.hits = result.target.maxHits;
            result.target.alive = true;
            el.style.transition = 'all 0.3s';
            el.style.transform = `translateX(${game.view.offset}px)`;
            el.style.opacity = '1';
            if (hitsEl) hitsEl.textContent = result.target.hits + ' ударів';
        }, 5000);
    } else {
        const name = result.type === 'tree' ? 'Дереву' : 'Каменю';
        addMessage(`🪓 Удар! ${name} залишилось ${result.target.hits} ударів.`, 'info');
    }
    
    if (tool.durability <= 0) {
        addMessage(`💔 ${tool.name} зламалася! Покращте її.`, 'error');
    }
    
    updateUI();
    
    setTimeout(() => {
        worldEl.classList.remove('chop');
        game.view.isChoping = false;
    }, 300);
}

// Crafting
function craftNails() {
    if (game.resources.stone < 2) {
        addMessage('❌ Потрібно 2 каміння для крафту цвяхів!', 'error');
        return;
    }
    game.resources.stone -= 2;
    const amount = Math.floor(Math.random() * 3) + 3;
    game.resources.nails += amount;
    game.stats.actionsDone++;
    updateUI();
    addMessage(`🔩 Ви зробили ${amount} цвяхів!`, 'craft');
}

function sellResources() {
    let total = 0;
    if (game.resources.wood > 0) { total += game.resources.wood * 2; game.resources.wood = 0; }
    if (game.resources.stone > 0) { total += game.resources.stone * 3; game.resources.stone = 0; }
    if (game.resources.glass > 0) { total += game.resources.glass * 5; game.resources.glass = 0; }
    if (game.resources.nails > 0) { total += game.resources.nails * 2; game.resources.nails = 0; }
    if (total === 0) { addMessage('❌ Немає ресурсів для продажу!', 'error'); return; }
    game.resources.coins += total;
    game.stats.actionsDone++;
    updateUI();
    addMessage(`💰 Ви продали ресурси за ${total} крон!`, 'craft');
}

// Tool upgrades
function upgradeAxe() {
    const cost = game.tools.axe.level * 15;
    if (game.resources.coins < cost) { addMessage(`❌ Потрібно ${cost} крон!`, 'error'); return; }
    if (game.tools.axe.level >= 5) { addMessage('❌ Максимальний рівень!', 'error'); return; }
    game.resources.coins -= cost;
    game.tools.axe.level++;
    game.tools.axe.bonus = game.tools.axe.level - 1;
    game.tools.axe.treesPerDura = 5 * Math.pow(2, game.tools.axe.level - 1);
    game.tools.axe.maxDurability = game.tools.axe.treesPerDura;
    game.tools.axe.durability = game.tools.axe.maxDurability;
    const names = ['', 'Звичайна сокира', 'Гостра сокира', 'Сталева сокира', 'Міфічна сокира'];
    game.tools.axe.name = names[game.tools.axe.level] || 'Легендарна сокира';
    game.stats.actionsDone++;
    selectTool('axe');
    updateUI();
    addMessage(`🪓 Сокиру покращено до ${game.tools.axe.level} рівня! Міцність: ${game.tools.axe.maxDurability}, +${game.tools.axe.bonus} дерева.`, 'craft');
}

function upgradePickaxe() {
    const cost = game.tools.pickaxe.level === 0 ? 10 : game.tools.pickaxe.level * 20;
    if (game.resources.coins < cost) { addMessage(`❌ Потрібно ${cost} крон!`, 'error'); return; }
    if (game.tools.pickaxe.level >= 5) { addMessage('❌ Максимальний рівень!', 'error'); return; }
    game.resources.coins -= cost;
    game.tools.pickaxe.level++;
    game.tools.pickaxe.bonus = game.tools.pickaxe.level - 1;
    game.tools.pickaxe.maxDurability = 10 * Math.pow(2, game.tools.pickaxe.level - 1);
    game.tools.pickaxe.durability = game.tools.pickaxe.maxDurability;
    const names = ['', 'Каменярна кирка', 'Залізна кирка', 'Сталева кирка', 'Алмазна кирка'];
    game.tools.pickaxe.name = names[game.tools.pickaxe.level] || 'Міфічна кирка';
    game.stats.actionsDone++;
    selectTool('pickaxe');
    updateUI();
    addMessage(`⛏️ Кирку покращено до ${game.tools.pickaxe.level} рівня! Міцність: ${game.tools.pickaxe.maxDurability}, +${game.tools.pickaxe.bonus} каміння.`, 'craft');
}

// Building functions (unchanged)
function buildFloor() {
    if (!checkResources(5, 0, 0, 3)) return;
    if (game.house.floor) { addMessage('❌ Підлога вже побудована!', 'error'); return; }
    spendResources(5, 0, 0, 3);
    game.house.floor = true;
    document.getElementById('floor').classList.add('visible');
    game.stats.actionsDone++;
    updateComfort();
    updateUI();
    addMessage('🪵 Ви побудували підлогу!', 'success');
}

function buildWall() {
    if (!checkResources(8, 0, 0, 5)) return;
    if (game.house.walls) { addMessage('❌ Стіни вже побудовані!', 'error'); return; }
    if (!game.house.floor) { addMessage('❌ Спочатку побудуйте підлогу!', 'error'); return; }
    spendResources(8, 0, 0, 5);
    game.house.walls = true;
    document.getElementById('wall-left').classList.add('visible');
    document.getElementById('wall-right').classList.add('visible');
    document.getElementById('wall-back').classList.add('visible');
    game.stats.actionsDone++;
    updateComfort();
    updateUI();
    addMessage('🧱 Ви побудували стіни!', 'success');
}

function buildRoof() {
    if (!checkResources(10, 5, 0, 5)) return;
    if (game.house.roof) { addMessage('❌ Дах вже побудований!', 'error'); return; }
    if (!game.house.walls) { addMessage('❌ Спочатку побудуйте стіни!', 'error'); return; }
    spendResources(10, 5, 0, 5);
    game.house.roof = true;
    document.getElementById('roof').classList.add('visible');
    game.stats.actionsDone++;
    updateComfort();
    updateUI();
    addMessage('🏠 Ви побудували дах!', 'success');
}

function buildWindow() {
    if (!checkResources(3, 0, 2, 0)) return;
    if (game.house.window) { addMessage('❌ Вікно вже побудоване!', 'error'); return; }
    if (!game.house.walls) { addMessage('❌ Спочатку побудуйте стіни!', 'error'); return; }
    spendResources(3, 0, 2, 0);
    game.house.window = true;
    document.getElementById('window').classList.add('visible');
    game.stats.actionsDone++;
    updateComfort();
    updateUI();
    addMessage('🪟 Ви вставили вікно!', 'success');
}

function buildDoor() {
    if (!checkResources(6, 0, 0, 3)) return;
    if (game.house.door) { addMessage('❌ Двері вже побудовані!', 'error'); return; }
    if (!game.house.walls) { addMessage('❌ Спочатку побудуйте стіни!', 'error'); return; }
    spendResources(6, 0, 0, 3);
    game.house.door = true;
    document.getElementById('door').classList.add('visible');
    game.stats.actionsDone++;
    updateComfort();
    updateUI();
    addMessage('🚪 Ви встановили двері!', 'success');
}

function buildBalcony() {
    if (!checkResources(12, 0, 0, 6)) return;
    if (game.house.balcony) { addMessage('❌ Балкон вже побудований!', 'error'); return; }
    if (!game.house.walls) { addMessage('❌ Спочатку побудуйте стіни!', 'error'); return; }
    spendResources(12, 0, 0, 6);
    game.house.balcony = true;
    document.getElementById('balcony').classList.add('visible');
    game.stats.actionsDone++;
    updateComfort();
    updateUI();
    addMessage('🌿 Ви побудували балкон!', 'success');
}

function buildFence() {
    if (!checkResources(8, 0, 0, 4)) return;
    if (game.house.fence) { addMessage('❌ Огорожа вже побудована!', 'error'); return; }
    if (!game.house.floor) { addMessage('❌ Спочатку побудуйте підлогу!', 'error'); return; }
    spendResources(8, 0, 0, 4);
    game.house.fence = true;
    document.getElementById('fence').classList.add('visible');
    game.stats.actionsDone++;
    updateComfort();
    updateUI();
    addMessage('⛩️ Ви встановили огорожу!', 'success');
}

function buildLadder() {
    if (!checkResources(5, 0, 0, 2)) return;
    if (game.house.ladder) { addMessage('❌ Драбина вже побудована!', 'error'); return; }
    spendResources(5, 0, 0, 2);
    game.house.ladder = true;
    document.getElementById('ladder').classList.add('visible');
    game.stats.actionsDone++;
    updateComfort();
    updateUI();
    addMessage('🪜 Ви зробили драбину!', 'success');
}

function buildFlag() {
    if (!checkResources(2, 0, 1, 0)) return;
    if (game.house.flag) { addMessage('❌ Прапорець вже встановлений!', 'error'); return; }
    if (!game.house.roof) { addMessage('❌ Спочатку побудуйте дах!', 'error'); return; }
    spendResources(2, 0, 1, 0);
    game.house.flag = true;
    document.getElementById('flag').classList.add('visible');
    game.stats.actionsDone++;
    updateComfort();
    updateUI();
    addMessage('🚩 Ви встановили прапорець!', 'success');
}

function addLevel() {
    if (!checkResources(20, 10, 0, 10)) return;
    if (game.house.level >= 4) { addMessage('❌ Максимум 3 поверхи!', 'error'); return; }
    if (!game.house.roof) { addMessage('❌ Спочатку побудуйте дах!', 'error'); return; }
    spendResources(20, 10, 0, 10);
    game.house.level++;
    const levelEl = document.getElementById(`level-${game.house.level - 1}`);
    if (levelEl) levelEl.classList.add('visible');
    game.stats.actionsDone++;
    updateComfort();
    updateUI();
    addMessage(`⬆️ Ви додали ${game.house.level - 1}-й поверх!`, 'success');
}

function checkResources(wood, stone, glass, nails) {
    if (game.resources.wood < wood) { addMessage(`❌ Недостатньо деревини! Потрібно ${wood}.`, 'error'); return false; }
    if (game.resources.stone < stone) { addMessage(`❌ Недостатньо каміння! Потрібно ${stone}.`, 'error'); return false; }
    if (game.resources.glass < glass) { addMessage(`❌ Недостатньо скла! Потрібно ${glass}.`, 'error'); return false; }
    if (game.resources.nails < nails) { addMessage(`❌ Недостатньо цвяхів! Потрібно ${nails}.`, 'error'); return false; }
    return true;
}

function spendResources(wood, stone, glass, nails) {
    game.resources.wood -= wood;
    game.resources.stone -= stone;
    game.resources.glass -= glass;
    game.resources.nails -= nails;
}

function updateComfort() {
    let c = 0;
    if (game.house.floor) c += 10;
    if (game.house.walls) c += 15;
    if (game.house.roof) c += 20;
    if (game.house.window) c += 10;
    if (game.house.door) c += 10;
    if (game.house.balcony) c += 15;
    if (game.house.fence) c += 5;
    if (game.house.ladder) c += 5;
    if (game.house.flag) c += 5;
    c += (game.house.level - 1) * 15;
    game.stats.comfort = Math.min(c, 100);
}

function updateUI() {
    document.getElementById('wood').textContent = game.resources.wood;
    document.getElementById('stone').textContent = game.resources.stone;
    document.getElementById('glass').textContent = game.resources.glass;
    document.getElementById('nails').textContent = game.resources.nails;
    document.getElementById('coins').textContent = game.resources.coins;
    document.getElementById('house-level').textContent = game.house.level;
    document.getElementById('comfort').textContent = game.stats.comfort;
    document.getElementById('actions-done').textContent = game.stats.actionsDone;
    document.getElementById('trees-cut').textContent = game.stats.treesCut;
    
    const tool = game.tools[game.selectedTool];
    const icon = game.selectedTool === 'axe' ? '🪓' : '⛏️';
    document.getElementById('current-tool').textContent = `${icon} ${tool.name}`;
    document.getElementById('tool-durability').textContent = tool.durability;
    document.getElementById('tool-maxdura').textContent = tool.maxDurability;
    
    document.getElementById('axe-cost').textContent = game.tools.axe.level * 15;
    document.getElementById('pickaxe-cost').textContent = game.tools.pickaxe.level === 0 ? 10 : game.tools.pickaxe.level * 20;
}

function addMessage(text, type = 'info') {
    const log = document.getElementById('message-log');
    const msg = document.createElement('div');
    msg.className = `msg ${type}`;
    msg.textContent = text;
    log.appendChild(msg);
    log.scrollTop = log.scrollHeight;
    while (log.children.length > 20) log.removeChild(log.firstChild);
}

// Interior functions
const interiorItems = {
    tv: { name: 'Телевізор', icon: '📺', cost: 15, comfort: 8 },
    cabinet: { name: 'Шкафчик', icon: '🗄️', cost: 10, comfort: 5 },
    sofa: { name: 'Диван', icon: '🛋️', cost: 20, comfort: 12 },
    armchair: { name: 'Крісло', icon: '💺', cost: 12, comfort: 7 },
    table: { name: 'Стіл', icon: '🪑', cost: 8, comfort: 4 },
    bed: { name: 'Ліжко', icon: '🛏️', cost: 25, comfort: 15 },
    rug: { name: 'Килим', icon: '🧶', cost: 6, comfort: 3 },
    plant: { name: 'Рослина', icon: '🌱', cost: 10, comfort: 6 },
    painting: { name: 'Картина', icon: '🖼️', cost: 14, comfort: 7 },
    lamp: { name: 'Торшер', icon: '💡', cost: 18, comfort: 9 }
};

function enterHouse() {
    if (!game.house.floor) {
        addMessage('❌ Спочатку побудуйте підлогу!', 'error');
        return;
    }
    if (!game.house.walls) {
        addMessage('❌ Спочатку побудуйте стіни!', 'error');
        return;
    }
    if (!game.house.door) {
        addMessage('❌ Спочатку встановіть двері!', 'error');
        return;
    }
    
    game.interior.inside = true;
    game.interior.maxSlots = 5 + (game.house.level - 1) * 3;
    
    // Switch views
    document.getElementById('interior-view').style.display = 'block';
    document.getElementById('sky').style.display = 'none';
    document.querySelectorAll('.fp-tree').forEach(el => el.style.display = 'none');
    document.querySelectorAll('.fp-rock').forEach(el => el.style.display = 'none');
    document.getElementById('fp-home-tree').style.display = 'none';
    document.getElementById('fp-ground').style.display = 'none';
    document.getElementById('crosshair').style.display = 'none';
    document.getElementById('fp-hand').style.display = 'none';
    document.getElementById('inventory-bar').style.display = 'none';
    
    // Switch panel sections
    document.getElementById('enter-section').style.display = 'none';
    document.getElementById('interior-section').style.display = 'block';
    document.getElementById('build-section').style.display = 'none';
    document.getElementById('craft-section').style.display = 'none';
    document.getElementById('tools-section').style.display = 'none';
    document.getElementById('player-section').style.display = 'none';
    
    document.getElementById('controls-hint').textContent = '🛋️ Ви всередині доміка | Натисніть "Вийти на вулицю" щоб повернутись';
    
    updateInteriorUI();
    addMessage('🏠 Ви зайшли в домік! Облаштуйте інтер\'єр.', 'success');
}

function exitHouse() {
    game.interior.inside = false;
    
    // Switch views back
    document.getElementById('interior-view').style.display = 'none';
    document.getElementById('sky').style.display = '';
    document.querySelectorAll('.fp-tree').forEach(el => el.style.display = '');
    document.querySelectorAll('.fp-rock').forEach(el => el.style.display = '');
    document.getElementById('fp-home-tree').style.display = '';
    document.getElementById('fp-ground').style.display = '';
    document.getElementById('crosshair').style.display = '';
    document.getElementById('fp-hand').style.display = '';
    document.getElementById('inventory-bar').style.display = '';
    
    // Switch panel sections back
    document.getElementById('enter-section').style.display = '';
    document.getElementById('interior-section').style.display = 'none';
    document.getElementById('build-section').style.display = '';
    document.getElementById('craft-section').style.display = '';
    document.getElementById('tools-section').style.display = '';
    document.getElementById('player-section').style.display = '';
    
    document.getElementById('controls-hint').textContent = 'WASD — ходити | ← → — повернути голову | ПКМ/E — рубнути | Q/1/2 — змінити інструмент';
    
    updateUI();
    addMessage('🌳 Ви вийшли на вулицю!', 'info');
}

function buyInterior(itemId) {
    const item = interiorItems[itemId];
    if (!item) return;
    
    if (game.interior.items[itemId]) {
        addMessage(`❌ ${item.name} вже є в доміку!`, 'error');
        return;
    }
    
    const usedSlots = Object.keys(game.interior.items).length;
    if (usedSlots >= game.interior.maxSlots) {
        addMessage(`❌ Недостатньо місця! Максимум ${game.interior.maxSlots} предметів.`, 'error');
        return;
    }
    
    if (game.resources.coins < item.cost) {
        addMessage(`❌ Потрібно ${item.cost} крон!`, 'error');
        return;
    }
    
    game.resources.coins -= item.cost;
    game.interior.items[itemId] = true;
    game.stats.actionsDone++;
    
    // Show item
    document.getElementById(`item-${itemId}`).classList.add('visible');
    
    // Add comfort
    game.stats.comfort = Math.min(100, game.stats.comfort + item.comfort);
    
    updateInteriorUI();
    updateUI();
    addMessage(`✅ Ви придбали ${item.icon} ${item.name}! Комфорт +${item.comfort}.`, 'success');
}

function updateInteriorUI() {
    const usedSlots = Object.keys(game.interior.items).length;
    document.getElementById('interior-slots').textContent = usedSlots;
    document.getElementById('interior-max').textContent = game.interior.maxSlots;
    
    // Disable buttons for items already bought
    for (const id of Object.keys(interiorItems)) {
        const btn = document.querySelector(`.interior-btn[onclick*="'${id}'"]`);
        if (btn) {
            btn.disabled = !!game.interior.items[id];
        }
    }
}

// Initialize
document.addEventListener('DOMContentLoaded', () => {
    // Keyboard
    document.addEventListener('keydown', (e) => {
        switch(e.key) {
            case 'ArrowLeft': case 'a': lookAround(-30); e.preventDefault(); break;
            case 'ArrowRight': case 'd': lookAround(30); e.preventDefault(); break;
            case 'ArrowUp': case 'w': lookAround(-60); e.preventDefault(); break;
            case 'ArrowDown': case 's': lookAround(60); e.preventDefault(); break;
            case ' ': case 'e': hitTarget(e); e.preventDefault(); break;
            case '1': selectTool('axe'); e.preventDefault(); break;
            case '2': selectTool('pickaxe'); e.preventDefault(); break;
            case 'q': 
                const next = game.selectedTool === 'axe' ? 'pickaxe' : 'axe';
                selectTool(next);
                e.preventDefault();
                break;
        }
    });
    
    // Right click
    document.getElementById('game-world').addEventListener('contextmenu', (e) => hitTarget(e));
    
    // Click on trees/rocks
    document.querySelectorAll('.fp-tree, .fp-rock').forEach(el => {
        el.addEventListener('click', () => hitTarget(new Event('click')));
    });
    
    // Animation styles
    const style = document.createElement('style');
    style.textContent = `
        @keyframes targetShake {
            0%, 100% { transform: translateX(0); }
            25% { transform: translateX(-4px); }
            75% { transform: translateX(4px); }
        }
    `;
    document.head.appendChild(style);
    
    selectTool('axe');
    updateUI();
    addMessage('🏡 Ласкаво просимо! У вас є 5 крон.', 'info');
    addMessage('🪓 WASD — ходити | ПКМ/E — бити | Q/1/2 — змінити інструмент', 'info');
});
