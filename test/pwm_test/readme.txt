/**
  @page " pwm_test.c"  demo
  
  @verbatim
  ******************** (C) COPYRIGHT 2019 MXCHIP MXOS SDK*******************
  * @file    test/pwm_test/readme.txt 
  * @author  MDWG (MXOS Documentation Working Group)
  * @version v2.4.x 
  * @date    17-May-2019
  * @brief   Description of the  "pwm test"  demo.
  ******************************************************************************

  @par Demo Description 
This demo shows:  
    - how to use pwm interface based on a pwm test applicaiton program. 
      Include the following specifical function:
       - how to Print logs through MXOS UART2(system debug serial port of MXOS);
       - how to flip a LED(connected to MXOS GPIO) ON or OFF  status by calling MXOS API;
       - how to Hang up the current thread for a while by calling MXOS API.

    - MXOS UART2 is mapping to UARTx of MCU, which can refer to platform.c and paltform.h.
    - mxos_SYS_LED is mapping to a GPIO, which can refer to platform.c and paltform.h.
 

@par Directory contents 
    - Demos/test/pwm_test/pwm_test.c      Applicaiton program
    - Demos/test/pwm_test/mxos_config.h   MXOS function header file


@par Hardware and Software environment  
    - This demo has been tested on MiCOKit-3165 board.
    - This demo can be easily tailored to any other supported device and development board.

@par How to use it ? 
In order to make the program work, you must do the following :
 - Open your preferred toolchain, 
    - IDE:  IAR 7.30.4 or Keil MDK 5.13.       
    - Debugging Tools: JLINK or STLINK
 - Modify header file path of  "mxos_config.h".   Please referring to "http://mico.io/wiki/doku.php?id=confighchange"
 - Rebuild all files and load your image into target memory.   Please referring to "http://mico.io/wiki/doku.php?id=debug"
 - Run the demo.
 - View operating results and system serial log (Serial port: Baud rate: 115200, data bits: 8bit, parity: No, stop bits: 1).   Please referring to http://mico.io/wiki/doku.php?id=com.mxchip.basic

**/

