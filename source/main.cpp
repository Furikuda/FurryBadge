#include "MicroBit.h"
#include "mbed.h"
#include "ble/BLE.h"

MicroBit uBit;

bool ble_ready = false;

const static GapAdvertisingData::Appearance_t APPEARANCE = (GapAdvertisingData::Appearance_t) 0x26dc;
const static uint8_t COMPLETE_NAME[] = {
    'D', 'E', 'F', 'C', 'O', 'N', 'F', 'u', 'r', 's'};
const static uint8_t BEACON_AWOO[] = {
    0xff, 0x71, 0xa0, 0x11, 0x02, 0x3f, 0x10, 0x02 };
const static uint8_t BEACON_EMOTE_BOOP[] = {
    0xff, 0x71, 0xb2, 0x00, 0x00, 0x62, 0x6f, 0x6f, 0x70 };
const static uint8_t BEACON_EMOTE_OWO[] = {
    0xff, 0x71, 0xb2, 0xe6, 0x21, 0x5e, 0x2e, 0x5e };


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
    uBit.serial.printf("BLE ready\r\n");
}

GapAdvertisingData make_awoo_advdata(){
    GapAdvertisingData advData;
    advData.addAppearance(APPEARANCE);
    advData.addFlags(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);
    advData.addData(GapAdvertisingData::COMPLETE_LOCAL_NAME, COMPLETE_NAME, sizeof(COMPLETE_NAME));
    advData.addData(GapAdvertisingData::MANUFACTURER_SPECIFIC_DATA, BEACON_AWOO, sizeof(BEACON_AWOO));
    return advData;
}

GapAdvertisingData make_boop_advdata(){
    GapAdvertisingData advData;
    advData.addAppearance(APPEARANCE);
    advData.addFlags(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);
    advData.addData(GapAdvertisingData::COMPLETE_LOCAL_NAME, COMPLETE_NAME, sizeof(COMPLETE_NAME));
    advData.addData(GapAdvertisingData::MANUFACTURER_SPECIFIC_DATA, BEACON_EMOTE_BOOP, sizeof(BEACON_EMOTE_BOOP));
    return advData;
}

GapAdvertisingData make_owo_advdata(){
    GapAdvertisingData advData;
    advData.addAppearance(APPEARANCE);
    advData.addFlags(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);
    advData.addData(GapAdvertisingData::COMPLETE_LOCAL_NAME, COMPLETE_NAME, sizeof(COMPLETE_NAME));
    advData.addData(GapAdvertisingData::MANUFACTURER_SPECIFIC_DATA, BEACON_EMOTE_OWO, sizeof(BEACON_EMOTE_OWO)+1);
    return advData;
}

int send_beacon(GapAdvertisingData advData) {

    if (!ble_ready) {
        return 1;
    }

    BLE& ble = BLE::Instance(BLE::DEFAULT_INSTANCE);

    GapAdvertisingParams advParams(
            GapAdvertisingParams::ADV_NON_CONNECTABLE_UNDIRECTED, // type of advertising
            1000, // interval in ms
            1 // Timeout in second
    );

    ble.gap().setAdvertisingParams(advParams);
    ble.gap().setAdvertisingPayload(advData);

    /* Start advertising */
    uBit.serial.printf("Start adv\r\n");
    ble.gap().startAdvertising();
//    uBit.sleep(3000);
//    uBit.serial.printf("Stop adv\r\n");
//    ble.gap().stopAdvertising();

    return 0;
}

void onButtonA(MicroBitEvent e)
{
    BLE& ble = BLE::Instance(BLE::DEFAULT_INSTANCE);
    uBit.serial.printf("OwO\r\n");
    ble.stopAdvertising();
    ble.gap().accumulateAdvertisingPayload(
           GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);
    ble.gap().setAdvertisingType(GapAdvertisingParams::ADV_NON_CONNECTABLE_UNDIRECTED);
    GapAdvertisingData advData;
    advData.addAppearance(APPEARANCE);
    advData.addFlags(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);
    advData.addData(GapAdvertisingData::COMPLETE_LOCAL_NAME, COMPLETE_NAME, sizeof(COMPLETE_NAME));
    advData.addData(GapAdvertisingData::MANUFACTURER_SPECIFIC_DATA, BEACON_EMOTE_OWO, sizeof(BEACON_EMOTE_OWO));
    ble.gap().setAdvertisingPayload(advData);
    ble.gap().setAdvertisingInterval(200); /* 200ms */
    ble.gap().startAdvertising();
    uBit.sleep(1000);
    ble.gap().stopAdvertising();

}

void onButtonB(MicroBitEvent e)
{
    uBit.serial.printf("BooP\r\n");
    send_beacon(make_boop_advdata());
}

void onButtonAB(MicroBitEvent e)
{
    uBit.serial.printf("AWOO\r\n");
    send_beacon(make_awoo_advdata());
}

int main(void)
{
    uBit.serial.printf("\033[0;0H"); // set cursor to 0,0
    uBit.serial.printf("\033[2J"); // clear screen
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
