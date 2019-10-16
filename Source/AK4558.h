/*
  ==============================================================================

    AK4558.h
    Created: 16 Oct 2019 12:01:27pm
    Author:  Akiyuki Okayasu

  ==============================================================================
*/

#pragma once

#if JUCE_LINUX
#include <chrono>
#include <thread>
#include <pigpio.h>

class ak4558
{
public:
  static void ak4558Config(int i2cHandle, const unsigned int ak4558ResetGPIO)
  {
    using std::chrono::milliseconds;
    using std::this_thread::sleep_for;

    int result;
    char i2cTXBuffer[TX_PACKET_SIZE];

    // AK4558 Reset
    sleep_for(milliseconds(1)); //1ms sleep
    gpioWrite(ak4558ResetGPIO, GPIOLOW);
    sleep_for(milliseconds(1)); //1ms sleep
    gpioWrite(ak4558ResetGPIO, GPIOHIGH);

    //Wait for AK4558 internal LDO wake up
    sleep_for(milliseconds(15)); //15ms sleep

    // PLL PowerDown
    // BICK FS: 64FS
    i2cTXBuffer[0] = (BICK64FS << 1) | EXT_POWERDOWN;
    result = i2cWriteI2CBlockData(i2cHandle, AK4558_I2C_PLL, i2cTXBuffer, TX_PACKET_SIZE);
    if (result)
    {
      std::cout << "Error: i2c send, result:" << result << std::endl;
    }

    // Audio format: 32bit I2S
    // Ch: Stereo
    // SoftMute: OFF
    i2cTXBuffer[0] = (STEREO << 6) | (I2S_32BIT << 3) | (ATTENUATIONTIME << 1) | SOFTMUTE_OFF;
    result = i2cWriteI2CBlockData(i2cHandle, AK4558_I2C_CONTROL1, i2cTXBuffer, TX_PACKET_SIZE);
    if (result)
    {
      std::cout << "Error: i2c send, result:" << result << std::endl;
    }

    // LPF: Short delay sharp rolloff
    i2cTXBuffer[0] = FILTER_SHORTDELAY_SHARPROLLOFF;
    result = i2cWriteI2CBlockData(i2cHandle, AK4558_I2C_LPF, i2cTXBuffer, TX_PACKET_SIZE);
    if (result)
    {
      std::cout << "Error: i2c send, result:" << result << std::endl;
    }

    // HPF: HPF ON, Sharp roll-off filter, short delay filter
    i2cTXBuffer[0] = (ADC_ROLLOFF_FILTER_SHARP << 3) | (ADC_SHORTDELAY_FILTER_SHORTDELAY << 2) | HPF_L_ON_R_ON;
    result = i2cWriteI2CBlockData(i2cHandle, AK4558_I2C_HPF, i2cTXBuffer, TX_PACKET_SIZE);
    if (result)
    {
      std::cout << "Error: i2c send, result:" << result << std::endl;
    }

    // LOUT volume set to 0dB
    i2cTXBuffer[0] = DACVOLUME_0dB;
    result = i2cWriteI2CBlockData(i2cHandle, AK4558_I2C_LOUT, i2cTXBuffer, TX_PACKET_SIZE);
    if (result)
    {
      std::cout << "Error: i2c send, result:" << result << std::endl;
    }

    // ROUT volume set to 0dB
    i2cTXBuffer[0] = DACVOLUME_0dB;
    result = i2cWriteI2CBlockData(i2cHandle, AK4558_I2C_ROUT, i2cTXBuffer, TX_PACKET_SIZE);
    if (result)
    {
      std::cout << "Error: i2c send, result:" << result << std::endl;
    }

    // Power save
    // Sample rate: 96kHz or 88.2kHz
    i2cTXBuffer[0] = (FS96_882 << 3) | LOPS_POWERSAVE;
    result = i2cWriteI2CBlockData(i2cHandle, AK4558_I2C_MODE, i2cTXBuffer, TX_PACKET_SIZE);
    if (result)
    {
      std::cout << "Error: i2c send, result:" << result << std::endl;
    }

    // PLL Powerup
    // BICK FS: 64FS
    i2cTXBuffer[0] = (BICK64FS << 1) | PLL_POWERUP;
    result = i2cWriteI2CBlockData(i2cHandle, AK4558_I2C_PLL, i2cTXBuffer, TX_PACKET_SIZE);
    if (result)
    {
      std::cout << "Error: i2c send, result:" << result << std::endl;
    }

    sleep_for(milliseconds(10)); //10ms sleep

    // ADC/DAC ON
    i2cTXBuffer[0] = ADC_ON_DAC_ON;
    result = i2cWriteI2CBlockData(i2cHandle, AK4558_I2C_POWER, i2cTXBuffer, TX_PACKET_SIZE);
    if (result)
    {
      std::cout << "Error: i2c send, result:" << result << std::endl;
    }

    sleep_for(milliseconds(300)); //300ms sleep

    // Operation Start
    // Samplerate: 96kHz or 88.2kHz
    i2cTXBuffer[0] = (FS96_882 << 3) | LOPS_NORMALOPERATION;
    result = i2cWriteI2CBlockData(i2cHandle, AK4558_I2C_MODE, i2cTXBuffer, TX_PACKET_SIZE);
    if (result)
    {
      std::cout << "Error: i2c send, result:" << result << std::endl;
    }
  }

private:
  static constexpr unsigned int GPIOHIGH = 1;
  static constexpr unsigned int GPIOLOW = 0;

  //I2C TXBuffer
  static constexpr int TX_PACKET_SIZE = 1;

  //I2C registers
  static constexpr unsigned int AK4558_I2C_POWER = 0x00;
  static constexpr unsigned int AK4558_I2C_PLL = 0x01;
  static constexpr unsigned int AK4558_I2C_DAC = 0x02;
  static constexpr unsigned int AK4558_I2C_CONTROL1 = 0x03;
  static constexpr unsigned int AK4558_I2C_CONTROL2 = 0x04;
  static constexpr unsigned int AK4558_I2C_MODE = 0x05;
  static constexpr unsigned int AK4558_I2C_LPF = 0x06;
  static constexpr unsigned int AK4558_I2C_HPF = 0x07;
  static constexpr unsigned int AK4558_I2C_LOUT = 0x08;
  static constexpr unsigned int AK4558_I2C_ROUT = 0x09;

  // POWER: AK4558_I2C_POWER 0x00
  static constexpr unsigned int ADC_ON_DAC_ON = 0b11111;
  static constexpr unsigned int ADC_ON_DAC_OFF = 0b11001;
  static constexpr unsigned int ADC_OFF_DAC_ON = 0b00111;
  static constexpr unsigned int ADC_OFF_DAC_OFF = 0b00001;

  // PLL: AK4558_I2C_PLL 0x01
  static constexpr unsigned int EXT_POWERDOWN = 0;
  static constexpr unsigned int PLL_POWERUP = 1;
  static constexpr unsigned int BICK64FS = 0b0010;
  static constexpr unsigned int BICK32FS = 0b0011;

  // Audio format: AK4558_I2C_CONTROL1 0x03
  static constexpr unsigned int STEREO = 0b00;
  static constexpr unsigned int I2S_32BIT = 0b111;
  static constexpr unsigned int I2S_16BIT = 0b011;
  static constexpr unsigned int I2S_24BIT = 0b011;
  static constexpr unsigned int ATTENUATIONTIME = 0b00;
  static constexpr unsigned int SOFTMUTE_ON = 1;
  static constexpr unsigned int SOFTMUTE_OFF = 0;

  // Sample rate: AK4558_I2C_MODE 0x05
  static constexpr unsigned int FS48_441 = 0b0100;
  static constexpr unsigned int FS96_882 = 0b0110;
  static constexpr unsigned int FS192_1762 = 0b1000;
  static constexpr unsigned int LOPS_NORMALOPERATION = 0b010;
  static constexpr unsigned int LOPS_POWERSAVE = 0b011u;

  // LPF: AK4558_I2C_FILTER 0x06
  static constexpr unsigned int FILTER_SHORTDELAY_SHARPROLLOFF = 0x09;

  // HPF: AK4558_I2C_HPF 0x07
  static constexpr unsigned int HPF_L_OFF_R_OFF = 0b00;
  static constexpr unsigned int HPF_L_ON_R_ON = 0b11;         //default
  static constexpr unsigned int ADC_ROLLOFF_FILTER_SHARP = 0; //default
  static constexpr unsigned int ADC_ROLLOFF_FILTER_SLOW = 1;
  static constexpr unsigned int ADC_SHORTDELAY_FILTER_NORMAL = 0;
  static constexpr unsigned int ADC_SHORTDELAY_FILTER_SHORTDELAY = 1; //default

  // DAC Volume: AK4558_I2C_LOUT 0x08, AK4558_I2C_ROUT 0x09
  static constexpr unsigned int DACVOLUME_0dB = 0xFF;

  // This class can't be instantiated, it's just a holder for static methods.
  ak4558() = delete;
};
#endif //JUCE_LINUX