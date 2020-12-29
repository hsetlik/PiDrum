/*
  ==============================================================================

    Sequencer.cpp
    Created: 22 Dec 2020 10:27:19pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "Sequencer.h"

Step::Step(int length, int maxSub, int index) : juce::ShapeButton("stepButton", restColorOff, restColorOff, noteColorOff), isSelected(false),
maxSubdivision(maxSub), factor(length), indexInSequence(index)
{
    state = restOff;
    isNote = false;
    noteColorOff = color.RGBColor(126, 0, 33);
    noteColorOn = color.RGBColor(255, 0, 66);
    restColorOff = color.RGBColor(70, 69, 75);
    restColorOn = color.RGBColor(33, 57, 195);
    setClickingTogglesState(true);
    shouldUseOnColours(true);
}

void Step::paintButton(juce::Graphics &g, bool mouseIsOver, bool mouseIsDown)
{
    switch(state)
    {
        case restOff:
        {
            g.setColour(restColorOff);
            break;
        }
        case restOn:
        {
            g.setColour(restColorOn);
            break;
        }
        case noteOff:
        {
            g.setColour(noteColorOff);
            break;
        }
        case noteOn:
        {
            g.setColour(noteColorOn);
            break;
        }
    }
    g.fillRect(getLocalBounds());
    auto border = getLocalBounds();
    if(isSelected)
    {
        g.setColour(juce::Colours::white);
    }
    else
    {
        g.setColour(juce::Colours::black);
    }
    g.drawRect(border);
}
Track::Track(int length, int minimumSubDiv, analogVoice type) :  label(type), sequenceLength(length), maxSubdivision(minimumSubDiv), drumVoice(type)
{
    for(int i = 0; i < sequenceLength; ++i)
    {
        steps.add(new Step(1, maxSubdivision, i));
        addAndMakeVisible(steps.getLast());
        steps.getLast()->addListener(this);
        steps.getLast()->addMouseListener(this, true);
    }
    //addAndMakeVisible(&selectArea);
    addAndMakeVisible(&label);
    currentStep = steps.getFirst();
    highlight = color.RGBColor(255, 255, 255);
    setInterceptsMouseClicks(true, true);
    //addMouseListener(this, false);
}

void Track::paint(juce::Graphics &g)
{
    auto bounds = currentStep->getBoundsInParent();
    g.setColour(highlight);
    g.fillRect(bounds);
}

void Track::resized()
{
    label.setBounds(0, 0, LABELWIDTH, getHeight());
    
    int rightEdge = LABELWIDTH;
    auto totalWidth = getWidth() - LABELWIDTH;
    auto subDivWidth = totalWidth / maxSubdivision;
    //selectArea.setBounds(LABELWIDTH, 0, totalWidth, getHeight());
   // printf("Total width: %d\n", totalWidth);
    //printf("num subdivisions: %d\n", (sequenceLength * maxSubdivision));
    for(int i = 0; i < steps.size(); ++i)
    {
        auto width = (steps.getUnchecked(i)->getNumSubDivs()) * (float)(subDivWidth / (float)sequenceLength);
        //printf("SubDivWidth: %d\n", subDivWidth);
        steps.getUnchecked(i)->setBounds(rightEdge, 0.0f, width, getHeight());
        rightEdge += width;
    }
}

void Track::buttonClicked(juce::Button *b)
{
    Step* thisStep = dynamic_cast<Step*>(b);
    thisStep->toggleNote();
    thisStep->repaint();
}

void Track::updateStepState(Step *toUpdate)
{
    if(toUpdate-> isNote)
    {
        if(toUpdate == currentStep)
        {
            toUpdate->setState(noteOn);
        }
        else
        {
            toUpdate->setState(noteOff);
        }
    }
    else
    {
        if(toUpdate == currentStep)
        {
            toUpdate->setState(restOn);
        }
        else
        {
            toUpdate->setState(restOff);
        }
    }
}

void Track::updateSteps(int numSubdivsIntoSequence)
{
    int pos = numSubdivsIntoSequence;
    int index = 0;
    for(int i = 0; i < steps.size(); ++i)
    {
        Step* checkStep = steps.getUnchecked(i);
        int noteEnd = index + checkStep->getNumSubDivs();
        if(index <= pos && pos < noteEnd)
        {
            currentStep = checkStep;
        }
        index = noteEnd;
        updateStepState(checkStep);
    }
}

Step* Track::stepAtXPos(int xPos)
{
    
    for(int i = 0; i < steps.size(); ++i)
    {
        auto thisX = steps.getUnchecked(i)->getScreenX();
        auto nextX = thisX + steps.getUnchecked(i)->getWidth();
        if( xPos > thisX && xPos <= nextX)
        {
            return steps.getUnchecked(i);
            break;
        }
    }
    return NULL;
}

void Track::clearSelection()
{
    if(selectedSteps.size() > 0)
    {
        for(int i = 0; i < selectedSteps.size(); ++i)
        {
            selectedSteps[i]->deselect();
        }
    }
    selectedSteps.clear();
}

void Track::selectStep(Step *toSelect)
{
    selectedSteps.push_back(toSelect);
    toSelect->select();
}

void Track::mouseDown(const juce::MouseEvent &m)
{
    clearSelection();
    printf("track clicked\n");
}

void Track::mouseDrag(const juce::MouseEvent &m)
{
    if(m.mouseWasDraggedSinceMouseDown())
    {
        Step* selectedStep = stepAtXPos(m.getScreenX());
        if(selectedStep != NULL)
        {
            selectStep(selectedStep);
        }
        else
        {
            printf("no valid step\n");
        }
    }
}





//==========================================================================================================================

Sequence::Sequence(int length, int maxSubDivs, int temp) : maxSubdivisions(maxSubDivs), tempo(temp), sequenceLength(length)
{
    setWantsKeyboardFocus(true);
    setInterceptsMouseClicks(false, true);
    tracks.add(new Track(sequenceLength, maxSubDivs, kick1));
    tracks.add(new Track(sequenceLength, maxSubDivs, kick2));
    tracks.add(new Track(sequenceLength, maxSubDivs, openHat));
    tracks.add(new Track(sequenceLength, maxSubDivs, closedHat));
    tracks.add(new Track(sequenceLength, maxSubDivs, snare));
    tracks.add(new Track(sequenceLength, maxSubDivs, clap));
    tracks.add(new Track(sequenceLength, maxSubDivs, clave));
    
    for(int i = 0; i < tracks.size(); ++i)
    {
        addAndMakeVisible(*tracks.getUnchecked(i));
    }
    maxDivIndex = 0;
    isPlaying = false;
    auto msPerBeat = (60.0f / tempo) * 250.0f;
    auto msPerDiv = msPerBeat / maxSubdivisions;
    startTimer(msPerDiv);
}

void Sequence::setTempo(int newTempo)
{
    tempo = newTempo;
    stopTimer();
    auto msPerBeat = (60.0f / tempo) * 250.0f;
    auto msPerDiv = msPerBeat / maxSubdivisions;
    startTimer(msPerDiv);
}
void Sequence::hiResTimerCallback()
{
    if(isPlaying)
    {
        incrementIndex();
    }
    for(int i = 0; i < tracks.size(); ++i)
    {
        tracks.getUnchecked(i)->updateSteps(maxDivIndex);
    }
}

void Sequence::incrementIndex()
{
    if(maxDivIndex < (maxSubdivisions * sequenceLength))
    {
        maxDivIndex += 1;
    }
    else
    {
        maxDivIndex = 0;
    }
}

void Sequence::resized()
{
    int trackHeight = getHeight() / tracks.size();
    for(int i = 0; i < tracks.size(); ++i)
    {
        tracks.getUnchecked(i)->setBounds(0, trackHeight * i, getWidth(), trackHeight);
    }
}

void Sequence::paint(juce::Graphics &g)
{
    if(hasKeyboardFocus(false) == false)
    {
        juce::Timer::callAfterDelay (100, [&] { grabKeyboardFocus(); });
    }
}

bool Sequence::keyPressed(const juce::KeyPress &p)
{
    auto key = p.getTextCharacter();
    switch(key)
    {
        case 'p':
        {
            togglePlay();
            break;
        }
        case 'f':
        {
            setTempo(tempo + 1);
            break;
        }
        case 's':
        {
            setTempo(tempo - 1);
            break;
        }
        default:
        {
            break;
        }
    }
    return false;
}

