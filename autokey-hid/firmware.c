#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>

#include "usbdrv.h"
#include "usbhid.h"

PROGMEM const char usbHidReportDescriptor[] = {    /* USB report descriptor */
    HID_USAGE_PAGE1(0x01),	// Generic Desktop Page
    HID_USAGE1(0x06),		// Keyboard
    HID_COLL1(0x01),		// Application

    HID_USAGE_PAGE1(0x07),	//   Keyboard
    HID_USAGE_MIN1(0xe0),	//   
    HID_USAGE_MAX1(0xe7),	//   
    HID_LOGICAL_MIN1(0),	//
    HID_LOGICAL_MAX1(1),
    HID_REPORT_SIZE1(1),
    HID_REPORT_COUNT1(8),
    HID_INPUT1(0x02),		//   Input (Data, Var, Abs) for modifier

    HID_USAGE_PAGE1(0x07),	//   Keyboard
    HID_USAGE_MIN1(0),
    HID_USAGE_MAX1(101),
    HID_LOGICAL_MIN1(0),
    HID_LOGICAL_MAX1(101),
    HID_REPORT_COUNT1(7),
    HID_REPORT_SIZE1(8),
    HID_INPUT1(0x00),		//   Input (Data, Array) for keycode
    HID_END_COLL0,
};

STATIC_ASSERT(sizeof(usbHidReportDescriptor) == USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH);

static struct {
    uint8_t mod;
    uint8_t code;
    uint8_t padding[6];
} hid_report;

static uint8_t idle_rate = 10;
static uint8_t pushsw_pressed;
static int16_t nvdata_len, nvdata_idx;
static uint8_t nvdata;

usbMsgLen_t usbFunctionSetup(uchar data[8])
{
    usbRequest_t *rq = (void *)data;

    switch(rq->bmRequestType & USBRQ_TYPE_MASK) {
    case USBRQ_TYPE_CLASS:
	switch(rq->bRequest) {
	case USBRQ_HID_GET_REPORT:
	    /* Control IN transaction */
	    hid_report.mod = hid_report.code = 0;
	    usbMsgPtr = (usbMsgPtr_t)&hid_report;
	    return sizeof(hid_report);

	case USBRQ_HID_GET_IDLE:
	    /* Control IN transaction */
	    usbMsgPtr = (usbMsgPtr_t)&idle_rate;
	    return 1;

	case USBRQ_HID_SET_IDLE:
	    /* Control OUT transaction */
	    idle_rate = rq->wValue.bytes[1];
	    break;
	}
	break;

    case USBRQ_TYPE_VENDOR:
	switch(rq->bRequest) {
	case 1:
	    /* Control OUT transaction */
	    eeprom_busy_wait();
	    eeprom_update_byte((uint8_t *)rq->wIndex.word, rq->wValue.bytes[0]);
	    break;

	case 2:
	    /* Control IN transaction */
	    eeprom_busy_wait();
	    nvdata = eeprom_read_byte((uint8_t *)rq->wIndex.word);
	    usbMsgPtr = (usbMsgPtr_t)&nvdata;
	    return 1;
	}
    }
    return 0;
}

static void init(void)
{
    PORTD |= 1;
    pushsw_pressed = 0;
    nvdata_idx = -1;
    eeprom_busy_wait();
    if(eeprom_read_word((uint16_t*)0) != 0xaa55) {
	nvdata_len = 0;
    } else {
	eeprom_busy_wait();
	nvdata_len = eeprom_read_word((uint16_t*)2);
    }

    /*
     * V-USB
     */
    usbInit();
    usbDeviceDisconnect();
    _delay_ms(100);

    usbDeviceConnect();
    sei();
}

void main(void)
{
    init();

    for(;;) {
	_delay_us(100);
	usbPoll();

	/* ボタン押された？ */
	if(pushsw_pressed == 0 && bit_is_clear(PIND, PIND0)) {
	    pushsw_pressed = 1;
	    if(nvdata_len > 0)
		nvdata_idx = 0;
	}

	/* キーコード転送シーケンス途中？ */
	if(nvdata_idx != -1) {
	    if(!usbInterruptIsReady())
		continue;

	    if(nvdata_idx & 1) {
		/* ブレークコード */
		hid_report.mod = 0;
		hid_report.code = 0;
	    } else {
		/* メイクコード */
		eeprom_busy_wait();
		hid_report.mod = eeprom_read_byte((uint8_t *)(nvdata_idx + 4));
		eeprom_busy_wait();
		hid_report.code = eeprom_read_byte((uint8_t *)(nvdata_idx + 5));
	    }
	    usbSetInterrupt((void*)&hid_report, sizeof(hid_report));

	    if(++nvdata_idx >= nvdata_len)
		nvdata_idx = -1;

	    continue;
	}

	if(bit_is_set(PIND, PIND0)) {
	    /* ボタン離された？ */
	    pushsw_pressed = 0;
	}
    }
}
