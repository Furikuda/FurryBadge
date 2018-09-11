#include "MicroBit.h"
#include "mbed.h"
#include "ble/BLE.h"

MicroBitButton buttonA(MICROBIT_PIN_BUTTON_A, MICROBIT_ID_BUTTON_A);
MicroBitButton buttonB(MICROBIT_PIN_BUTTON_B, MICROBIT_ID_BUTTON_B);

MicroBitMessageBus bus;

MicroBitSerial serial(USBTX, USBRX);

bool ble_ready = false;

const static GapAdvertisingData::Appearance_t APPEARANCE = (GapAdvertisingData::Appearance_t) 0x26dc;
const static uint8_t COMPLETE_NAME[] = {
    'D', 'E', 'F', 'C', 'O', 'N', 'F', 'u', 'r', 's'};

const static uint8_t BEACON_AWOO[] = {
    0xff, 0x71, 0xa0, 0x11, 0x02, 0x3f, 0x10, 0x02 };
const static uint8_t BEACON_EMOTE_BOOP[] = {
    0xff, 0x71, 0xb2, 0x00, 0x00, 0x62, 0x6f, 0x6f, 0x70 };
const static uint8_t BEACON_EMOTE_OWO[] = {
    0xff, 0x71, 0xb2, 0xe6, 0x21, 0x6f, 0x77, 0x6f };

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

/**
 * Builds an Awoo broadcast Advertising Data package
 */
GapAdvertisingData make_awoo_advdata(){
    GapAdvertisingData advData;
    advData.addAppearance(APPEARANCE);
    advData.addFlags(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);
    advData.addData(GapAdvertisingData::COMPLETE_LOCAL_NAME, COMPLETE_NAME, sizeof(COMPLETE_NAME));
    advData.addData(GapAdvertisingData::MANUFACTURER_SPECIFIC_DATA, BEACON_AWOO, sizeof(BEACON_AWOO));
    return advData;
}

/**
 * Builds a Boop emote Advertising Data package
 */
GapAdvertisingData make_boop_advdata(){
    GapAdvertisingData advData;
    advData.addAppearance(APPEARANCE);
    advData.addFlags(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);
    advData.addData(GapAdvertisingData::COMPLETE_LOCAL_NAME, COMPLETE_NAME, sizeof(COMPLETE_NAME));
    advData.addData(GapAdvertisingData::MANUFACTURER_SPECIFIC_DATA, BEACON_EMOTE_BOOP, sizeof(BEACON_EMOTE_BOOP));
    return advData;
}

/**
 * Builds an "OwO" emote Advertising Data package
 */
GapAdvertisingData make_owo_advdata(){
    GapAdvertisingData advData;
    advData.addAppearance(APPEARANCE);
    advData.addFlags(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);
    advData.addData(GapAdvertisingData::COMPLETE_LOCAL_NAME, COMPLETE_NAME, sizeof(COMPLETE_NAME));
    advData.addData(GapAdvertisingData::MANUFACTURER_SPECIFIC_DATA, BEACON_EMOTE_OWO, sizeof(BEACON_EMOTE_OWO));
    return advData;
}

/**
 * Sends the passed Advertisnig data package over the blueteeths
 */
int send_beacon(GapAdvertisingData advData) {

    if (!ble_ready) {
        return 1;
    }

    BLE& ble = BLE::Instance(BLE::DEFAULT_INSTANCE);

    GapAdvertisingParams advParams(
            GapAdvertisingParams::ADV_NON_CONNECTABLE_UNDIRECTED, // type of advertising
            // The following 2 values work ok to be sure we send only one beacon
            2000, // interval in ms
            1 // Timeout in second
    );
    ble.gap().setAdvertisingParams(advParams);

    ble.gap().setAdvertisingPayload(advData);

    /* Start advertising */
    ble.gap().startAdvertising();

    return 0;
}

/**
 * Triggered on Button A press
 */
void onButtonA(MicroBitEvent e)
{
    serial.printf("owo\r\n");
    send_beacon(make_owo_advdata());
}

/**
 * Triggered on Button B press
 */
void onButtonB(MicroBitEvent e)
{
    serial.printf("boop\r\n");
    send_beacon(make_boop_advdata());
}

int main(void)
{

    BLE& ble = BLE::Instance(BLE::DEFAULT_INSTANCE);
    /* Initialize BLE baselayer, always do this first! */
    ble.init(bleInitComplete);

    /* sets up call backs on button press events */
    bus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_CLICK, onButtonA);
    bus.listen(MICROBIT_ID_BUTTON_B, MICROBIT_BUTTON_EVT_CLICK, onButtonB);

    serial.printf("I have booted :3\r\n");

    // We're done
    while (1){
        // Todo, have a better 'sleep'
    }
}
