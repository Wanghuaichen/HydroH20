#include "mbed.h"
#include "MODSERIAL.h"
#include <string> 
using namespace std;
#include "XBEE_PRO_S2_TEST.h"

DigitalOut myled1(LED1);
DigitalOut myled2(LED2);
DigitalOut myled3(LED3);
DigitalOut myled4(LED4);
DigitalOut reset(p11);
DigitalIn cts(p30);
DigitalOut rts(p29);
MODSERIAL xbee(p9, p10);
MODSERIAL pc(USBTX, USBRX);
string SL = "00000000";
string SH = "000000";
int a = 0;
#define LOOPBACK 1
#define TO_END_POINT 2
#define TO_COORDINATOR 3

#define TEST_TYPE LOOPBACK

int test = TEST_TYPE

int main(){
    rts = 1;
    pc.printf("MBED XBEE TEST\r\n");
    reset = 0;
    wait(1);
    reset = 1;
    wait(5);
    
    if(test == LOOPBACK){
        while(1){
            if(pc.readable()){
                rts = 0;
                while(cts);
                xbee.putc(pc.getc());
                rts = 1;
            }
            if(xbee.readable()){
                pc.putc(xbee.getc());
            }
        }
        */

    /* TRANSMISSION
    while(1){
        if(xbee.readable()){
            a = xbee.getc();
            switch(a){
                case '1': myled1 = !myled1; xbeeSendData("Toggling 1\r\n"); break;
                case '2': myled2 = !myled2; xbeeSendData("Toggling 2\r\n"); break;
                case '3': myled3 = !myled3; xbeeSendData("Toggling 3\r\n"); break;
                case '4': myled4 = !myled4; xbeeSendData("Toggling 4\r\n"); break;
                default: xbeeSendData("Nope.\r\n"); break;
            }
        }
    }
    */
    
    if(!xbeeCM())pc.printf("Failed to enter Command Mode!\r\n");
    else pc.printf("Entered Command Mode!\r\n");
    if(!xbeeSendCommand("ATDH000000\r"))pc.printf("Failed to set DH!\r\n");
    else pc.printf("Set DH!\r\n");
    if(!xbeeSendCommand("ATDL00000000\r"))pc.printf("Failed to set DL!\r\n");
    else pc.printf("Set DL!\r\n");
    //xbeeSendCommand("ATWR\r"); // Write any changed registers
    xbeeGetAddress();
    if(!xbeeSendCommand("ATCN\r"))pc.printf("Failed to exit Command Mode!\r\n");
    else pc.printf("Exited Command Mode!\r\n");
    wait(5);
    
    pc.printf("Address: ");
    pc.printf(SH.c_str());
    pc.printf(SL.c_str());
    pc.printf("\r\n");
    
    while(1){
        myled1 = 0;
        wait(1);
        myled1 = 1;
        xbeeSendData("Hello there!");
        wait(1);
    }
}

// Enter XBEE AT Command Mode
// Returns 1 on success
int xbeeCM(){
    int success = 1;
    int a = 0;
    wait(1);
    rts = 0;
    while(cts);
    xbee.printf("+++");
    wait(1);
    rts = 1;
    while(xbee.rxBufferEmpty());
    a = xbee.getc();
    if(a != 'O')success = 0;
    while(xbee.rxBufferEmpty());
    a = xbee.getc();
    if(a != 'K')success = 0;
    while(xbee.rxBufferEmpty());
    xbee.getc();
    return success;
}

// Gets the address of the attatched unit
// Saves it in LH and SH as hexadecimal
void xbeeGetAddress(){
    int a = 0;
    rts = 0;
    while(cts);
    xbee.printf("ATSL\r");
    for(int i = 0; i < 8; i++){
        while(xbee.rxBufferEmpty());
        SL[i] = xbee.getc();
    }
    while(xbee.rxBufferEmpty());
    xbee.getc();
    
    a = 0;
    rts = 0;
    while(cts);
    xbee.printf("ATSH\r");
    rts = 1;
    for(int i = 0; i < 6; i++){
        while(xbee.rxBufferEmpty());
        SH[i] = xbee.getc();
    }
    while(xbee.rxBufferEmpty());
    xbee.getc();
    rts = 1;
}

// Sends AT Command
// Returns 1 on success
int xbeeSendCommand(const char * data){
    int success = 1;
    int a = 0;
    rts = 0;
    while(cts);
    xbee.printf(data);
    rts = 1;
    while(xbee.rxBufferEmpty());
    a = xbee.getc();
    if(a != 'O')success = 0;
    while(xbee.rxBufferEmpty());
    a = xbee.getc();
    if(a != 'K')success = 0;
    while(xbee.rxBufferEmpty());
    xbee.getc();
    return success;
}

// Sends formatted data packet
// Packets begin with '|'
// Next 14 characters are the device's address in hex
// Next character is ':'
// Following characters are data
// Packets end with '|'
void xbeeSendData(const char * data){
    rts = 0;
    while(cts);
    xbee.putc('|');
    xbee.printf(SH.c_str());
    xbee.printf(SL.c_str());
    xbee.putc(':');
    xbee.printf(data);
    xbee.putc('|');
    rts = 1;
}
