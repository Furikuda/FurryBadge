#include "MicroBit.h"
#include "mbed.h"
#include "ble/BLE.h"

MicroBit uBit;

bool ble_ready = false;

const static GapAdvertisingData::Appearance_t APPEARANCE = (GapAdvertisingData::Appearance_t) 0x26dc;
const static uint8_t COMPLETE_NAME[] = {
    'D', 'E', 'F', 'C', 'O', 'N', 'F', 'u', 'r', 's'};

const static uint8_t BEACON_AWOO[] = {
    0xff, 0x71, 0xa0, 0xff, 0xff, 0x40, 0xff, 0xff };
const static uint8_t BEACON_EMOTE_BOOP[] = {
    0xff, 0x71, 0xb2, 0x00, 0x00, 0x62, 0x6f, 0x6f, 0x70 };
const static uint8_t BEACON_EMOTE_OWO[] = {
    0xff, 0x71, 0xb2, 0x00, 0x00, 0x62, 0x6f, 0x7f, 0x6f };


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
    BLE& ble   = params->ble;
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

    ble_ready = 1;
}

int send_beacon(const uint8_t payload[]) {

    if (!ble_ready) {
        return 1;
    }

    BLE& ble = BLE::Instance(BLE::DEFAULT_INSTANCE);

    ble.stopAdvertising();

    // Set some flags
    ble.gap().accumulateAdvertisingPayload(
            GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE
    );
    ble.gap().setAdvertisingType(GapAdvertisingParams::ADV_NON_CONNECTABLE_UNDIRECTED);

    GapAdvertisingData advData;
    advData.addAppearance(APPEARANCE);
    advData.addFlags(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);
    advData.addData(GapAdvertisingData::COMPLETE_LOCAL_NAME, COMPLETE_NAME, sizeof(COMPLETE_NAME));
    advData.addData(GapAdvertisingData::MANUFACTURER_SPECIFIC_DATA, payload, sizeof(payload));
    ble.gap().setAdvertisingPayload(advData);

    /* Set advertising interval. Longer interval == longer battery life */
    ble.gap().setAdvertisingInterval(200); /* 200ms */

    /* Start advertising */
    ble.gap().startAdvertising();
    uBit.sleep(1000);

    ble.gap().stopAdvertising();

    return 0;
}

void onButtonA(MicroBitEvent e)
{
    uBit.serial.printf("OwO \n");
    send_beacon(BEACON_EMOTE_OWO);
}

void onButtonB(MicroBitEvent e)
{
    uBit.serial.printf("BooP\n");
    send_beacon(BEACON_EMOTE_BOOP);
}

void onButtonAB(MicroBitEvent e)
{
    uBit.serial.printf("AWOO \n");
    send_beacon(BEACON_AWOO);
}

int main(void)
{
    BLE& ble = BLE::Instance(BLE::DEFAULT_INSTANCE);
    /* Initialize BLE baselayer, always do this first! */
    ble.init(bleInitComplete);

    // Initialise the micro:bit runtime.
    uBit.init();

    // Register to receive events when any buttons are clicked, including the A+B virtual button (both buttons at once).
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_CLICK, onButtonA);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_B, MICROBIT_BUTTON_EVT_CLICK, onButtonB);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_AB, MICROBIT_BUTTON_EVT_CLICK, onButtonAB);

    // We're done, so just enter a power efficient sleep while we wait for an event.
    while (1)
        uBit.sleep(10000);

}
