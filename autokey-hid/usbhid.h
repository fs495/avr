#ifndef __usbhid_h__
#define __usbhid_h__

/* Main items */
#define HID_INPUT1(v)		0x81, (v)
#define HID_OUTPUT1(v)		0x91, (v)
#define HID_COLL1(v)		0xa1, (v)
#define HID_FEATURE1(v)		0xb1, (v)
#define HID_FEATURE2(v)		0xb2, ((v) & 0xff), (((v) >> 8) & 0xff)
#define HID_END_COLL0		0xc0

/* Global items */
#define HID_USAGE_PAGE1(v)	0x05, (v)
#define HID_USAGE_PAGE2(v)	0x05, ((v) & 0xff), (((v) >> 8) & 0xff)
#define HID_LOGICAL_MIN1(v)	0x15, (v)
#define HID_LOGICAL_MAX1(v)	0x25, (v)
#define HID_PHISYCAL_MIN1(v)	0x35, (v)
#define HID_PHISYCAL_MAX1(v)	0x45, (v)
#define HID_UNIT_EXPONENT1(v)	0x55, (v)
#define HID_UNIT1(v)		0x65, (v)
#define HID_REPORT_SIZE1(v)	0x75, (v)
#define HID_REPORT_ID1(v)	0x85, (v)
#define HID_REPORT_COUNT1(v)	0x95, (v)
#define HID_PUSH1(v)		0xa5, (v)
#define HID_POP1(v)		0xb5, (v)

/* Local items */
#define HID_USAGE1(v)		0x09, (v)
#define HID_USAGE_MIN1(v)	0x19, (v)
#define HID_USAGE_MAX1(v)	0x29, (v)
#define HID_DESIGNATOR_IDX1(v)	0x39, (v)
#define HID_DESIGNATOR_MIN1(v)	0x49, (v)
#define HID_DESIGNATOR_MAX1(v)	0x59, (v)
#define HID_STRING_IDX1(v) 	0x69, (v)
#define HID_STRING_MIN1(v)	0x79, (v)
#define HID_STRING_MAX1(v)	0x89, (v)
#define HID_DELIMITER(v)	0xa9, (v)

#ifndef STATIC_ASSERT
#define STATIC_ASSERT(ex)	typedef char should_be_true[1/!!(ex)]
#endif

#endif /* __usbhid_h__ */
