/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PiDrumAudioProcessorEditor::PiDrumAudioProcessorEditor (PiDrumAudioProcessor& p)
    : AudioProcessorEditor (&p),  seq(16,60,120), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    addAndMakeVisible(&seq);
    setSize (800, 480);
}

PiDrumAudioProcessorEditor::~PiDrumAudioProcessorEditor()
{
}

//==============================================================================
void PiDrumAudioProcessorEditor::paint (juce::Graphics& g)
{
   
}

void PiDrumAudioProcessorEditor::resized()
{
    seq.setBounds(0, 0, getWidth(), getHeight());
}
