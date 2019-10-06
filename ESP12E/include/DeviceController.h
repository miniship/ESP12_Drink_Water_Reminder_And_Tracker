#ifndef DeviceController_h
#define DeviceController_h

namespace controller {
    enum DeviceMode {
        AccessPoint,
        Station
    };

    enum AlertMode {
        Interval,
        Schedule
    };

    void doSetup();
    void doLoop();
    DeviceMode getDeviceMode();
    AlertMode getAlertMode();
    void switchDeviceMode();
    void switchToDeviceMode(DeviceMode mode);
    void switchAlertMode();
    void switchToAlertMode(AlertMode mode);
    void switchModes(DeviceMode dMode, AlertMode aMode);
}

#endif