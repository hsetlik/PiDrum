/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "SequencerPanel.h"

//==============================================================================
/**
*/
class PiDrumAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    PiDrumAudioProcessorEditor (PiDrumAudioProcessor&);
    ~PiDrumAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    Sequence seq;
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PiDrumAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PiDrumAudioProcessorEditor)
};
