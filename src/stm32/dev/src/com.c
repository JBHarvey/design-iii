#include <com.h>

uint8_t bFlagSendData = 0;

uint8_t readUSB() {
	/* USB configured OK, drivers OK */
	if (TM_USB_VCP_GetStatus() == TM_USB_VCP_CONNECTED) {
		/* Turn on GREEN led */
		//TM_DISCO_LedOn (LED_GREEN);
		//TM_DISCO_LedOff (LED_RED);
		uint8_t readCharacter;

		if (TM_USB_VCP_Getc(&readCharacter) == TM_USB_VCP_DATA_OK) {
			//TM_USB_VCP_Putc(readCharacter);
			return readCharacter;
		}
	} else {
		/* USB not OK */
		//TM_DISCO_LedOff (LED_GREEN);
		//TM_DISCO_LedOn (LED_RED);
		return 0;
	}
	return 0;
}

