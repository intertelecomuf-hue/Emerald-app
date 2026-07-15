/**
 * BluetoothService.h — Classic Bluetooth (SPP) scanner singleton
 *
 * Scans for nearby Classic Bluetooth devices and manages connections.
 */

#ifndef BLUETOOTH_SERVICE_H
#define BLUETOOTH_SERVICE_H

#include <Arduino.h>

#define MAX_BT_DEVICES 30
#define BT_NAME_LEN    64

struct BTDeviceInfo {
    char name[BT_NAME_LEN];
    char address[18];  // "XX:XX:XX:XX:XX:XX"
    uint32_t cod;      // Class of Device
    bool isConnected;
};

enum BTState {
    BT_IDLE,
    BT_SCANNING,
    BT_CONNECTING,
    BT_CONNECTED,
    BT_FAILED
};

class BluetoothService {
public:
    static BluetoothService& instance();

    void begin();
    void startScan();
    bool isScanComplete();  // Returns true when scan is done
    int getNumDevices() const;
    const BTDeviceInfo* getDevice(int index) const;
    void addDevice(const char* name, const char* address, uint32_t cod);
    
    void connect(int index);
    void disconnect();
    bool sendData(const uint8_t* data, size_t len);
    int receiveData(uint8_t* buf, size_t maxLen);
    
    BTState getState() const;
    const char* getConnectedName() const;
    const char* getConnectedAddress() const;

private:
    BluetoothService();
    ~BluetoothService() {}
    BluetoothService(const BluetoothService&) = delete;
    BluetoothService& operator=(const BluetoothService&) = delete;

    BTState _state;
    int _numDevices;
    BTDeviceInfo _devices[MAX_BT_DEVICES];
    unsigned long _scanStartTime;
    static const unsigned long SCAN_TIMEOUT = 12000;  // 12 seconds

    char _connectedName[BT_NAME_LEN];
    char _connectedAddress[18];
};

#endif // BLUETOOTH_SERVICE_H
