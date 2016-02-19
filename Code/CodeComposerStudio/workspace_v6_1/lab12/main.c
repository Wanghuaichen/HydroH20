#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/pwm.h"
#include "driverlib/uart.h"
#include "inc/hw_ints.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "utils/uartstdio.h"
#include "utils/ustdlib.h"
#include "SensorPins.h"

#define TicksPerMS 40000
#define ALL_BITS (0xff<<2)
#define index_start_byte 0
#define index_rw 1
#define index_reg_address 2
#define index_data_length 3
#define index_data 4
#define write 0x00
#define read 0x01
#define reg_address_page 0x07
#define reg_address_opr 0x3D
#define reg_address_lia 0x28
#define ndof_mode 0x0C
#define page_zero 0x00
#define start_byte 0xAA
#define packet_length_write 4
#define packet_length_read 3

/*---------------------------- Module Variables ---------------------------*/
static uint8_t array_tx[20]; //array to hold the byte sequence we send
static uint8_t array_rx[20];
static uint8_t tx_data = 0x00; //the byte that actually matters
static uint8_t rx_data = 0x00; //the byte that actually matters
static uint8_t index=1;
static uint8_t index_rx=0;
static bool pageselectdone = false;
static bool oprmodeselectdone = false;

/*---------------------------- Module Functions ---------------------------*/
void uart_init(void);
void ConfigureUART(void);
bool isdone_pageselect(void);
bool isdone_oprmode(void);
void isr_uart1(void);
void isr_uart2(void);


int dataLength = 0;

int sendChars(char* data) {
	dataLength = strlen(data);
	int i = 0;
	for (i = 0; i < dataLength; i++) {
		UARTCharPut(UART2_BASE, data[i]);
	}
	return 0;
}

void uart2_init() {
	//initializes UART2
	//sets up transmit and receive interrupts
	//0. Init Clock to UART2 and Port D
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART2);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	//Give it time for clocks to start
	SysCtlDelay(10);
	//Disable UART before programming
	UARTDisable(UART2_BASE);
	//1.  set baud rate, txe/rxe, stp2 (clear then and fen), and wlen (8 bit_)
	UARTConfigSetExpClk(UART2_BASE, SysCtlClockGet(), 115200,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
	UARTFIFODisable(UART2_BASE); //DISABLE FIFO. This should make my interrupt every time I get a byte.
	//1.5. Set up AFSEL To D6, D7
	HWREG(GPIO_PORTD_BASE+GPIO_O_LOCK) = GPIO_LOCK_KEY;
	HWREG(GPIO_PORTD_BASE+GPIO_O_CR) |= GPIO_PIN_7;
	GPIOPinConfigure(GPIO_PD6_U2RX);
	GPIOPinConfigure(GPIO_PD7_U2TX);
	//Set D6 as Input (RX) and D7 as Output (TX)
	GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_7);
	GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, GPIO_PIN_6);
	//A little redundancy shouldn't hurt.
	GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_6 | GPIO_PIN_7);
	//Enable Global NVIC Interrupt
	IntEnable(INT_UART2);
	//Enable Local Interrupts
	UARTIntEnable(UART2_BASE, (UART_INT_TX | UART_INT_RX)); //enable Tx and Rx int
	UARTTxIntModeSet(UART2_BASE, UART_TXINT_MODE_EOT); //set Tx mode to EOT
	UARTFIFOLevelSet(UART2_BASE, UART_FIFO_TX1_8, UART_FIFO_RX1_8); //set Rx to trigger with two bytes
	//Disable for now so I don't get stuck in the TX isr.
	UARTIntDisable(UART2_BASE, UART_INT_RX);
	//Link a function to the UART Interrupt
	UARTIntRegister(UART2_BASE, isr_uart2);
	//Enable UART2
	UARTEnable(UART2_BASE);

	//these will change based on our state, but we can use this as a table of contents
	array_tx[index_start_byte] = start_byte;
	array_tx[index_rw] = write;
	array_tx[index_reg_address] = reg_address_page;
	array_tx[index_data_length] = 0x01;
	array_tx[index_data] = page_zero;
}

void isr_uart2(void) {
	uint32_t trigger;
	trigger = UARTIntStatus(UART2_BASE, true);
	if ((trigger & UART_INT_RX) == UART_INT_RX) {
		//ISR triggers each time I receive a byte.
		printf("RX: UART2 ISR\n");
		UARTIntClear(UART2_BASE, UART_INT_RX);
		array_rx[index_rx] = UARTCharGet(UART2_BASE);

		if (array_rx[0] == 0xEE && index_rx != 0) {
			index_rx = 0;
			printf("RX: OxEE // Write Response \n");
			switch (array_rx[1]) {
			case 0x01:
				printf("WRITE_SUCCESS \n");
				break;
			case 0x03:
				printf("WRITE_FAIL \n");
				break;
			case 0x04:
				printf("REGMAP_INVALID_ADDRESS \n");
				break;
			case 0x06:
				printf("WRONG_START_BYTE \n");
				break;
			case 0x07:
				printf("BUS_OVER_RUN_ERROR \n");
				break;
			case 0x08:
				printf("MAX_LENGTH_ERROR \n");
				break;
			case 0x09:
				printf("MIN_LENGTH_ERROR \n");
				break;
			case 0x0A:
				printf("RECEIVE_CHARACTER_TIMEOUT \n");
				break;
			}
		}

		if (array_rx[0] == 0xBB && index_rx > packet_length_read) {
			printf("RX: 0xBB // Data Packet Received \n");
			index_rx = 0;
		}

		index_rx++;

	}
	if ((trigger & UART_INT_TX) == UART_INT_TX) {
		//printf("TX: In  UART2 Transmit ISR\n");
		UARTIntClear(UART2_BASE, UART_INT_TX);
		UARTCharPut(UART2_BASE, array_tx[index]);
		//printf("%u \n", index);
		index++;

		if (pageselectdone == false && (index > packet_length_write)) {
			//printf("TX: Page Select Done. Setting Boolean True. \n");
			UARTIntDisable(UART2_BASE, UART_INT_TX);
			index = 1;
			array_tx[index_reg_address] = reg_address_opr;
			array_tx[index_data] = ndof_mode;
			pageselectdone = true;
		}

		else if (pageselectdone == true && oprmodeselectdone == false
				&& index > packet_length_write) {
			//printf("here1 \n");
			UARTIntDisable(UART2_BASE, UART_INT_TX);
			index = 1;
			array_tx[index_reg_address] = reg_address_lia;
			array_tx[index_rw] = read;
			array_tx[index_data_length] = 0x06;
			oprmodeselectdone = true;
			//Start regular 200 ms timer. Within the timer interrupt I will query the LIA.
		}

		else if (pageselectdone == true && oprmodeselectdone == true
				&& index > packet_length_read) {
			//printf("here2 \n");
			UARTIntDisable(UART2_BASE, UART_INT_TX);
			index = 1;
		}

	}
}

int main(void) {

	//Initialize pins
	//PortFunctionInit();
	uart2_init();

	//Set system clock
	SysCtlClockSet(
	SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

	//Setup UART configuration
	UARTConfigSetExpClk(UART2_BASE, SysCtlClockGet(), 115200,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

	/*
	 //enable processor interrupts
	 IntMasterEnable();
	 */

	//Run sensors
	while (1) {
		//Some sample data
		double temp = 21.2;
		double turb = 120.0;
		double cond = 33.0;
		char data[80];

		//Loop counter
		int count;
		for (count = 0; count < 4; count++) {
			sprintf(data,
					"Sending data: \r\nTemperature %.2f, Turbidity: %.2f Conductivity: %.2f\r\n",
					temp, turb, cond);
			sendChars(data);
		}
		char waitData[30];
		//Wait for received data
		while (!UARTCharsAvail(UART2_BASE)) {
			sprintf(waitData, "Waiting for new requests\n");
		}

		char recData[45];
		//data received, print
		while (UARTCharGetNonBlocking(UART2_BASE)) {
			sprintf(recData, "Received data: %s",
					(char *) UARTCharGet(UART2_BASE));
			printf("Hello!");
		}
	}
}
