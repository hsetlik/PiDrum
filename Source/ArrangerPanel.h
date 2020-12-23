/*
  ==============================================================================

    ArrangerPanel.h
    Created: 22 Dec 2020 10:26:50pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class ArrangerPanel  : public juce::Component
{
public:
    ArrangerPanel();
    ~ArrangerPanel() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ArrangerPanel)
};
