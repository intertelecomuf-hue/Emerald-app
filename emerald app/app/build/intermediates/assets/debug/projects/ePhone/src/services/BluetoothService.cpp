/**
 * BluetoothService.cpp — Classic Bluetooth (SPP) scanner singleton
 *
 * Uses ESP32 Bluetooth Classic for device discovery and SPP connections.
 */

#include "BluetoothService.h"
#include <esp_bt.h>
#include <esp_bt_main.h>
#include <esp_bt_device.h>
#include <esp_gap_bt_api.h>

// Forward declarations for static callbacks
static void bt_gap_callback(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param);

BluetoothService& BluetoothService::instance() {
    static BluetoothService inst;
    return inst;
}

BluetoothService::BluetoothService()
    : _state(BT_IDLE)
    , _numDevices(0)
    , _scanStartTime(0)
{
    _connectedName[0] = '\0';
    _connectedAddress[0] = '\0';
    for (int i = 0; i < MAX_BT_DEVICES; i++) {
        _devices[i].name[0] = '\0';
        _devices[i].address[0] = '\0';
        _devices[i].cod = 0;
        _devices[i].isConnected = false;
    }
}

void BluetoothService::begin() {
    if (_state != BT_IDLE) return;

    Serial.println("[BTService] Initializing Classic Bluetooth...");

    // Release Classic Bluetooth memory first
    esp_bt_controller_mem_release(ESP_BT_MODE_BLE);
    
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    bt_cfg.mode = ESP_BT_MODE_CLASSIC_BT;
    
    esp_err_t err;
    err = esp_bt_controller_init(&bt_cfg);
    if (err != ESP_OK) {
        Serial.printf("[BTService] BT controller init failed: %d\n", err);
        return;
    }
    
    err = esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT);
    if (err != ESP_OK) {
        Serial.printf("[BTService] BT controller enable failed: %d\n", err);
        return;
    }
    
    err = esp_bluedroid_init();
    if (err != ESP_OK) {
        Serial.printf("[BTService] Bluedroid init failed: %d\n", err);
        return;
    }
    
    err = esp_bluedroid_enable();
    if (err != ESP_OK) {
        Serial.printf("[BTService] Bluedroid enable failed: %d\n", err);
        return;
    }
    
    // Set device name
    esp_bt_dev_set_device_name("ePhone");
    
    // Register GAP callback
    err = esp_bt_gap_register_callback(bt_gap_callback);
    if (err != ESP_OK) {
        Serial.printf("[BTService] GAP callback register failed: %d\n", err);
        return;
    }
    
    // Set scan mode to connectable + discoverable
    esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
    
    Serial.println("[BTService] Classic Bluetooth initialized");
}

void BluetoothService::startScan() {
    if (_state == BT_SCANNING) return;
    
    _numDevices = 0;
    _state = BT_SCANNING;
    _scanStartTime = millis();
    
    Serial.println("[BTService] Starting device discovery...");
    
    // Start inquiry (scan)
    esp_bt_gap_start_discovery(ESP_BT_INQ_MODE_GENERAL_INQUIRY, 10, 0);
}

bool BluetoothService::isScanComplete() {
    if (_state != BT_SCANNING) return true;
    
    // Check timeout
    if (millis() - _scanStartTime > SCAN_TIMEOUT) {
        Serial.printf("[BTService] Scan timeout, found %d devices\n", _numDevices);
        _state = BT_IDLE;
        return true;
    }
    
    return false;
}

void BluetoothService::addDevice(const char* name, const char* address, uint32_t cod) {
    if (_numDevices >= MAX_BT_DEVICES) return;
    
    BTDeviceInfo& dev = _devices[_numDevices];
    strncpy(dev.name, name, BT_NAME_LEN - 1);
    dev.name[BT_NAME_LEN - 1] = '\0';
    strncpy(dev.address, address, 17);
    dev.address[17] = '\0';
    dev.cod = cod;
    dev.isConnected = false;
    
    _numDevices++;
    Serial.printf("[BTService] Added device #%d: %s (%s)\n", _numDevices - 1, name, address);
}

int BluetoothService::getNumDevices() const {
    return _numDevices;
}

const BTDeviceInfo* BluetoothService::getDevice(int index) const {
    if (index < 0 || index >= _numDevices) return nullptr;
    return &_devices[index];
}

void BluetoothService::connect(int index) {
    if (index < 0 || index >= _numDevices) return;
    
    _state = BT_CONNECTING;
    strncpy(_connectedName, _devices[index].name, BT_NAME_LEN - 1);
    _connectedName[BT_NAME_LEN - 1] = '\0';
    strncpy(_connectedAddress, _devices[index].address, 17);
    _connectedAddress[17] = '\0';
    
    Serial.printf("[BTService] Connecting to %s (%s)...\n", _connectedName, _connectedAddress);
    
    // For Classic Bluetooth SPP, we'd use esp_spp_connect() here
    // For now, simulate connection (actual SPP connection requires SPP profile setup)
    _state = BT_CONNECTED;
    _devices[index].isConnected = true;
    Serial.printf("[BTService] Connected to %s\n", _connectedName);
}

void BluetoothService::disconnect() {
    if (_state == BT_CONNECTED) {
        Serial.printf("[BTService] Disconnecting from %s\n", _connectedName);
        // Find and mark device as disconnected
        for (int i = 0; i < _numDevices; i++) {
            if (strcmp(_devices[i].address, _connectedAddress) == 0) {
                _devices[i].isConnected = false;
                break;
            }
        }
        _connectedName[0] = '\0';
        _connectedAddress[0] = '\0';
        _state = BT_IDLE;
        Serial.println("[BTService] Disconnected");
    }
}

bool BluetoothService::sendData(const uint8_t* data, size_t len) {
    if (_state != BT_CONNECTED) return false;
    // SPP send would go here
    Serial.printf("[BTService] Sending %d bytes\n", len);
    return true;
}

int BluetoothService::receiveData(uint8_t* buf, size_t maxLen) {
    if (_state != BT_CONNECTED) return 0;
    // SPP receive would go here
    return 0;
}

BTState BluetoothService::getState() const {
    return _state;
}

const char* BluetoothService::getConnectedName() const {
    return _connectedName;
}

const char* BluetoothService::getConnectedAddress() const {
    return _connectedAddress;
}

// ============================================================
// GAP Callback — handles discovery events
// ============================================================
static void bt_gap_callback(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param) {
    BluetoothService& bt = BluetoothService::instance();
    
    switch (event) {
        case ESP_BT_GAP_DISC_RES_EVT: {
            // Device found during discovery
            if (bt.getNumDevices() >= MAX_BT_DEVICES) break;
            
            esp_bd_addr_t &bda = param->disc_res.bda;
            char addrStr[18];
            snprintf(addrStr, sizeof(addrStr),
                     "%02x:%02x:%02x:%02x:%02x:%02x",
                     bda[0], bda[1], bda[2], bda[3], bda[4], bda[5]);
            
            // Get device name from properties
            char devName[BT_NAME_LEN] = "Unknown";
            uint32_t devCod = 0;
            
            for (int i = 0; i < param->disc_res.num_prop; i++) {
                esp_bt_gap_dev_prop_t &prop = param->disc_res.prop[i];
                if (prop.type == ESP_BT_GAP_DEV_PROP_BDNAME) {
                    size_t nameLen = (prop.len < BT_NAME_LEN - 1) ? prop.len : BT_NAME_LEN - 1;
                    memcpy(devName, prop.val, nameLen);
                    devName[nameLen] = '\0';
                } else if (prop.type == ESP_BT_GAP_DEV_PROP_COD) {
                    devCod = *(uint32_t*)prop.val;
                }
            }
            
            // Store the device
            bt.addDevice(devName, addrStr, devCod);
            break;
        }
        
        case ESP_BT_GAP_DISC_STATE_CHANGED_EVT: {
            if (param->disc_st_chg.state == ESP_BT_GAP_DISCOVERY_STOPPED) {
                Serial.printf("[BTService] Discovery stopped, %d devices found\n",
                             bt.getNumDevices());
            }
            break;
        }
        
        default:
            break;
    }
}
