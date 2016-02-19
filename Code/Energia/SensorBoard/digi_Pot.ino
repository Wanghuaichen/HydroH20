
/**
 *
 * Sets up the digital potentiometer using 
 */
void  setupDigiPot()  {             
  //Values  
  uint8_t gain0 = 25;
  uint8_t gain1 = 25;
  altSPI.setModule(0);
  altSPI.begin();
  altSPI.setClockDivider(SPI_CLOCK_DIV64);  // max clock speed = 10 MHz
  altSPI.setBitOrder(MSBFIRST);
  altSPI.setDataMode(SPI_MODE0);            // clock idles low, read on rising edge

  altSPI.trans2ByteA(0);   // command 0; write to channel 0
  altSPI.trans2ByteB(gain0);   // 
  
  altSPI.trans2ByteA(16);   // command 16; write to channel 1
  altSPI.trans2ByteB(gain1);  // 
  
  altSPI.end();
}



