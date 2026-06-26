/*
 * cat.h: Helper functions for computer aided tuning (CAT)
 *
 *  Created on: Jun 22, 2026
 *      Author: mtrowbri
 */

#ifndef APP_CAT_H_
#define APP_CAT_H_

#include <stdint.h>

// Helper functions
void UIntToText(char *buffer, uint32_t value, int digits, int offset);

uint32_t TextToUInt(const char *buffer, int start, int len);

/**
 * Enumeration for the Kenwood TS-480 audio filter settings as they apply to the BK4819 REG43.
 * I only implemented the SSB/FM/AM settings; revisit if we add CW or FSK to this firmware
 */
enum KenwoodAudioFilterBw_t
{
  KENWOOD_AF_NORMAL = 0, // 3.0 kHz
  KENWOOD_AF_NAR = 0b011000000, // 2.0 kHz
  KENWOOD_AF_NAR2 = 0b100000000, // 1.0 kHz
  BK4819_AF_MASK = 0b111000000
};

typedef enum KenwoodAudioFilterBw_t KenwoodAudioFilterBw_t;

void applyAudioFilter(const KenwoodAudioFilterBw_t bandwidth);

KenwoodAudioFilterBw_t getAudioFilter();

// Command handler functions

/**
 * FW Filter Width (audio/DSP)
 * @param cat_buffer
 * @param cat_pos
 * @param Port to send the response out on (UART or USART)
 */
void handleCatsFW(char *cat_buffer, const uint8_t cat_pos, const uint32_t port);

/**
 * PS Power state (sleeping or not)
 * @param cat_buffer
 * @param cat_pos
 * @param Port to send the response out on (UART or USART)
 */
void handleCatsPS(char *cat_buffer, const uint8_t cat_pos, const uint32_t port);

/**
 * AI automated information messages
 * @param cat_buffer
 * @param cat_pos
 * @param port
 */
void handleCatsAI(char *cat_buffer, const uint8_t cat_pos, const uint32_t port);

/**
 * KS [Morse] Keyer Speed
 * @param cat_buffer
 * @param cat_pos
 * @param port
 */
void handleCatsKS(char *cat_buffer, const uint8_t cat_pos, const uint32_t port);

void handleCatsRM(char *cat_buffer, const uint8_t cat_pos, const uint32_t port);

#endif /* APP_CAT_H_ */
