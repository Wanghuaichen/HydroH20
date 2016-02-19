#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
/**
 * Main entry point in program
 */
int main(void)
{
	//Hold value of LED
	uint8_t ui8LED = 2;
	//Set system clock
	//Use 16MHz crystal driving = 400MHz PLL
	//SYSDIV_5 divides by 5, in addition to built-in divide by 2, total division = 10
	//Total system clock speed = 40MHz
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	//Use GPIOF pins
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	//Using GPIO F-pins, enable PF1, PF2, PF3 for output
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);

	//Go forever
	while(1)
	{
		// Turn on the LED
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, ui8LED);
		// Delay for a bit, for param/3 CPU cycles
		SysCtlDelay(2000000);
		// Turn off LED
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0x00);
		// Keep off for a bit
		SysCtlDelay(6000000);
		// Cycle through Red, Green and Blue LEDs
		if (ui8LED == 8) {ui8LED = 2;} else {ui8LED = ui8LED*2;}
	}
}
