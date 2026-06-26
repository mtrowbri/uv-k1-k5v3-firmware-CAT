/*
 * cat.c
 *
 *  Created on: Jun 22, 2026
 *      Author: mtrowbri
 */

#include "cat.h"

#include "driver/bk4819.h"
#include "driver/bk4819-regs.h"

#include "uart.h" // for UART_SendText
#include "functions.h"

// Helper functions
void UIntToText(char* buffer, uint32_t value, int digits, int offset) {
    for (int i = offset + digits - 1; i >= offset; i--) {
        buffer[i] = (value % 10) + '0';
        value /= 10;
    }
}

uint32_t TextToUInt(const char* buffer, int start, int len) {
    uint32_t val = 0;
    for (int i = start; i < start + len; i++) {
        if (buffer[i] >= '0' && buffer[i] <= '9') {
            val = (val * 10) + (buffer[i] - '0');
        }
    }
    return val;
}

void applyAudioFilter( const KenwoodAudioFilterBw_t audioBandwidth)
{
  uint16_t nonAudioSettings = BK4819_ReadRegister(BK4819_REG_43) | ~BK4819_AF_MASK;
  BK4819_WriteRegister(BK4819_REG_43, nonAudioSettings | audioBandwidth);
}

KenwoodAudioFilterBw_t getAudioFilter()
{
  return BK4819_ReadRegister(BK4819_REG_43) | BK4819_AF_MASK;
}

void handleCatsFW(char * cat_buffer, const uint8_t cat_pos, const uint32_t port)
{
  // Set if param P1 is provided
  if ( cat_pos >= 7 )
  {
    // Set the filter width
    uint32_t filterBwHzOrId = TextToUInt(cat_buffer, 2, 4);
    if ( 2 == filterBwHzOrId )
      applyAudioFilter(KENWOOD_AF_NAR2);
    else if ( 1 == filterBwHzOrId )
      applyAudioFilter(KENWOOD_AF_NAR);
    else
      applyAudioFilter(KENWOOD_AF_NORMAL);
  }

  // Query current
  KenwoodAudioFilterBw_t selectedFilterReg43 = getAudioFilter();

  // Format current for output
  char resp[] = "FW    ;";
  uint32_t selectedFilterCats = 0; // normal
  if ( KENWOOD_AF_NAR == selectedFilterReg43 )
    selectedFilterCats = 1;
  else if ( KENWOOD_AF_NAR2 == selectedFilterReg43 )
    selectedFilterCats = 2;
  UIntToText(resp, selectedFilterCats, 4, 2);

  // Send
  UART_SendText(port, resp);
}

void handleCatsPS(char *cat_buffer, const uint8_t cat_pos, const uint32_t port)
{
  // Set if param P1 is provided
  //  0: power off active
  //  1: power on
  //  2: power off (sleep)
  if ( cat_pos >= 4 )
  {
    if ('0' == cat_buffer[2] || '9' == cat_buffer[2])
    {
      // Not differentiating between the types of power off
      FUNCTION_Select(FUNCTION_POWER_SAVE);
    }
    else if (FUNCTION_POWER_SAVE == gCurrentFunction)
    {
      // Commanded to active and asleep, wake up (implied: rx)
      FUNCTION_Select(FUNCTION_FOREGROUND);
    }
  }

  // Prepare response
  char resp[] = "PS1;"; // powered on/active
  if (FUNCTION_POWER_SAVE == gCurrentFunction)
    resp[2] = '9'; // sleeping

  UART_SendText(port, resp);
}

void handleCatsAI(char *cat_buffer, const uint8_t cat_pos, const uint32_t port){
  // TODO supporting this requires scheduling a new task at 1500 ms cadence
  // Just reply with something sane and catalog compliant for hamlib
  char resp[] = "AI0;"; // Automated information messages disabled.
  UART_SendText(port, resp);
}

void handleCatsKS(char *cat_buffer, const uint8_t cat_pos, const uint32_t port)
{
  // TODO CW/Morse is not implemented. Revise when someone implements it.
  char resp[] = "KS010;"; // 10 WPM
  UART_SendText(port, resp);
}

void handleCatsRM(char *cat_buffer, const uint8_t cat_pos, const uint32_t port)
{
  // TODO we could probably implement the ALC part of this command
  char resp[] = "RM10000;RM20000;RM30000;"; // rigctl seems to think this is correct
  UART_SendText(port, resp);
}
