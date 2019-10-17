/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#if JUCE_LINUX
#include <pigpio.h>
#endif
#include "AK4558.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent : public AudioAppComponent
{
public:
  //==============================================================================
  MainComponent();
  ~MainComponent();

  //==============================================================================
  void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
  void getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) override;
  void releaseResources() override;

  //==============================================================================
  void paint(Graphics &g) override;
  void resized() override;
  void showAudioSettings();

private:
  //==============================================================================

  // GPIO pin for AK4558 Reset
  static constexpr unsigned int AK4558RESET = 5;

  //I2C
  int i2cHandler;
  static constexpr unsigned int i2cAddr = 0b0010000; //AK4558 I2C address
  static constexpr unsigned int i2cPort = 1;         //Raspberry Pi I2C Port 1(0 or 1)
  static constexpr unsigned int i2cFlag = 0;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
