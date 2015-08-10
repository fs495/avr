#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "usbdrv.h"
#include "usbhid.h"

PROGMEM const char usbHidReportDescriptor[] = {    /* USB report descriptor */
    HID_USAGE_PAGE1(0x01),	// Generic Desktop
    HID_USAGE1(0x02),		// Mouse
    HID_COLL1(0x01),		// Application
    HID_USAGE1(0x01),		//   Pointer
    HID_COLL1(0x00),		//   Physical
    HID_USAGE_PAGE1(0x09),	//     Button
    HID_USAGE_MIN1(1),		//     from 1
    HID_USAGE_MAX1(1),		//     to 1
    HID_LOGICAL_MIN1(0),	//     0=Off
    HID_LOGICAL_MAX1(1),	//     1=On
    HID_REPORT_COUNT1(1),	//     one button only
    HID_REPORT_SIZE1(1),	//     1 bit state
    HID_INPUT1(0x02),		//     Input(Data, Var, Abs)

    HID_REPORT_COUNT1(7),	//     7 paddings
    HID_REPORT_SIZE1(1),	//     1 bit size
    HID_INPUT1(0x03),		//     Input(Const, Var, Abs)

    HID_USAGE_PAGE1(0x01),	//     Generic Desktop
    HID_USAGE1(0x30),		//     X
    HID_USAGE1(0x31),		//     Y
    HID_LOGICAL_MIN1(0x81),	//     -127
    HID_LOGICAL_MAX1(0x7f),	//     +127
    HID_REPORT_SIZE1(8),	//     8 bits
    HID_REPORT_COUNT1(2),	//     2 values
    HID_INPUT1(0x06),		//     Input(Data, Var, Rel)

    HID_END_COLL0,		//   End Collection
    HID_END_COLL0,		// End Collection
};

STATIC_ASSERT(sizeof(usbHidReportDescriptor) == USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH);

/*
 * 送信するHID Reportを格納するグローバル変数
 */
static struct {
    unsigned char button;
    signed char x, y;
} hid_report;

usbMsgLen_t usbFunctionSetup(uchar data[8])
{
    usbRequest_t *rq = (void *)data;
    if((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS){
	if(rq->bRequest == USBRQ_HID_GET_REPORT) {
	    usbMsgPtr = (usbMsgPtr_t)&hid_report;
	    return sizeof(hid_report);
	}
    }

    return 0;
}

void main(void)
{
    usbInit();
    usbDeviceDisconnect();
    _delay_ms(100);
    usbDeviceConnect();
    sei();

    for(;;) {
	usbPoll();
	if(usbInterruptIsReady()) {
	    DDRB ^= 1;
	    hid_report.button ^= 1;
	    usbSetInterrupt((void*)&hid_report, sizeof(hid_report));
	}

	_delay_ms(1);
    }
}
