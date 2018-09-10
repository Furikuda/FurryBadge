//#include "MicroBit.h"
#include "mbed.h"
#include "ble/BLE.h"

//MicroBit uBit;

/**
 * This function is called when the ble initialization process has failed
 */
void onBleInitError(BLE &ble, ble_error_t error)
{
    /* Avoid compiler warnings */
    (void) ble;
    (void) error;
    
    /* Initialization error handling should go here */
}    

/**
 * Callback triggered when the ble initialization process has finished
 */
void bleInitComplete(BLE::InitializationCompleteCallbackContext *params)
{
    BLE&        ble   = params->ble;
    ble_error_t error = params->error;

    if (error != BLE_ERROR_NONE) {
        /* In case of error, forward the error handling to onBleInitError */
        onBleInitError(ble, error);
        return;
    }

    /* Ensure that it is the default instance of BLE */
    if(ble.getInstanceID() != BLE::DEFAULT_INSTANCE) {
        return;
    }

    /* Sacrifice 3B of 31B to Advertising Flags */
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE );
    ble.gap().setAdvertisingType(GapAdvertisingParams::ADV_NON_CONNECTABLE_UNDIRECTED);

//    const static uint8_t MyData[] = {0xff, 0x71,   0xa0, 0x12, 0x01, 0x3f, 0x12, 0x01 };
 //   const static uint8_t MyData[] = {0xff, 0x71,   0xa0, 0xff, 0xff,      64,  0xff, 0xff };
    //                              |manufacturer| awoo| serial (beacon)| TTL| Originating badge serial

    const static uint8_t CompleteName[] = {'D', 'E', 'F', 'C', 'O', 'N', 'F', 'u', 'r', 's'};

    // emote
    const static uint8_t MyData[] = {0xff, 0x71,   0xb2, 0x00, 0x00, 0x62, 0x6f, 0x6f, 0x70 };

    GapAdvertisingData   advData;
    advData.addAppearance((GapAdvertisingData::Appearance_t) 0x26dc);
    advData.addFlags(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);
    advData.addData(GapAdvertisingData::COMPLETE_LOCAL_NAME, CompleteName, sizeof(CompleteName));
    advData.addData(GapAdvertisingData::MANUFACTURER_SPECIFIC_DATA, MyData, sizeof(MyData));
    ble.gap().setAdvertisingPayload(advData);

    /* Set advertising interval. Longer interval == longer battery life */
    ble.gap().setAdvertisingInterval(1000); /* 1000ms */

    /* Start advertising */
    ble.gap().startAdvertising();
}

int main(void)
{
    BLE& ble = BLE::Instance(BLE::DEFAULT_INSTANCE);
 
    /* Initialize BLE baselayer, always do this first! */
    ble.init(bleInitComplete);

    /* Infinite loop waiting for BLE events */
    while (true) {
        /* Save power while waiting for callback events */
        ble.waitForEvent();
    }
}
