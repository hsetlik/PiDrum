/*
  ==============================================================================

    Sequencer.cpp
    Created: 22 Dec 2020 10:27:19pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "Sequencer.h"

Step::Step(float length, int maxSub, int index) : juce::ShapeButton("stepButton", restColorOff, restColorOff, noteColorOff), isSelected(false),
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

//=========================================================================

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
    
    for(int s = 0; s < steps.size(); ++s)
    {
        auto f = steps.getUnchecked(s)->getFactor();
        if(f < 1.0f)
        {
            auto bounds = steps.getUnchecked(s)->getBounds();
            g.setColour(juce::Colours::white);
            g.fillRect(bounds);
        }
    }
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
    printf("%lu steps selected\n", selectedSteps.size());
    toSelect->select();
}

void Track::mouseDown(const juce::MouseEvent &m)
{
    clearSelection();
    //printf("track clicked\n");
}

void Track::mouseDrag(const juce::MouseEvent &m)
{
    if(m.mouseWasDraggedSinceMouseDown())
    {
        Step* selectedStep = stepAtXPos(m.getScreenX());
        if(selectedStep->getIsSelected() == false && selectedSteps.size() < 4)
        {
            selectStep(selectedStep);
        }
    }
}

void Track::increaseSubdivision()
{
  if(selectedSteps.size() > 0)
  {
      auto startingCount = selectedSteps.size();
      auto numMinSubDivs = 0;
      for(int i = 0; i < startingCount; ++i)
      {
          numMinSubDivs += selectedSteps[i]->getNumSubDivs();
      }
      auto numFullSteps = numMinSubDivs / maxSubdivision;
      auto newStepFactor = ((startingCount + 1.0f) / numFullSteps);
      auto rangeStartIndex = 1000;
      for(int i = 0; i < startingCount; ++i)
      {
          int index = steps.indexOf(selectedSteps[i]);
          if(index < 1) index = 1;
          if(index < rangeStartIndex) rangeStartIndex = index;
          
      }
      for(int i = 0; i < selectedSteps.size(); ++i)
      {
          steps.remove(rangeStartIndex);
      }
      auto currentWriteIndex = rangeStartIndex;
      for(int i = 0; i < (startingCount + 1); ++i)
      {
          steps.insert(currentWriteIndex, new Step(newStepFactor, maxSubdivision, currentWriteIndex));
          Step* newest = steps.getUnchecked(currentWriteIndex);
          addAndMakeVisible(newest);
          newest->addListener(this);
          newest->addMouseListener(this, true);
          resized();
          //newest->setBounds(0, 0, 50, 50);
          if(newest->isShowing())
          {
              printf("tuplet showing\n");
              printf("step x : %d\n", newest->getX());
              printf("step y : %d\n", newest->getY());
              printf("step width : %d\n", newest->getWidth());
              printf("step height : %d\n", newest->getHeight());
              newest->repaint();
          }
          currentWriteIndex++;
      }
  }
}

void Track::decreaseSubdivision()
{
    if(selectedSteps.size() > 1)
    {
        auto startingCount = selectedSteps.size();
        auto numMinSubDivs = 0;
        for(int i = 0; i < startingCount; ++i)
        {
            numMinSubDivs += selectedSteps[i]->getNumSubDivs();
        }
        auto numFullSteps = numMinSubDivs / maxSubdivision;
        auto newStepFactor = ((startingCount - 1.0f) / numFullSteps);
        auto rangeStartIndex = 1000;
        for(int i = 0; i < startingCount; ++i)
        {
            int index = steps.indexOf(selectedSteps[i]);
            if(index < rangeStartIndex) rangeStartIndex = index;
            
        }
        for(int i = 0; i < selectedSteps.size(); ++i)
        {
            steps.remove(rangeStartIndex);
        }
        auto currentWriteIndex = rangeStartIndex + 1;
        for(int i = 0; i < (startingCount - 1); ++i)
        {
            steps.insert((currentWriteIndex), new Step(newStepFactor, maxSubdivision, currentWriteIndex));
            Step* newest = steps.getUnchecked(currentWriteIndex);
            addAndMakeVisible(newest);
            newest->addListener(this);
            newest->addMouseListener(this, true);
            resized();
            //newest->setBounds(0, 0, 50, 50);
            if(newest->isShowing())
            {
                printf("tuplet showing\n");
                printf("step x : %d\n", newest->getX());
                printf("step y : %d\n", newest->getY());
                printf("step width : %d\n", newest->getWidth());
                printf("step height : %d\n", newest->getHeight());
                newest->repaint();
            }
            currentWriteIndex++;
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

Track* Sequence::getSelectedTrack()
{
    for(int i = 0; i < tracks.size(); ++i)
    {
        if(tracks.getUnchecked(i)->selectedSteps.size() > 0) return tracks.getUnchecked(i);
    }
    return NULL;
}

bool Sequence::keyPressed(const juce::KeyPress &p)
{
    auto key = p.getTextCharacter();
    switch(key)
    {
        case 'p': //p for play
        {
            togglePlay();
            break;
        }
        case 'f': //f for faster
        {
            setTempo(tempo + 1);
            break;
        }
        case 's': //s for slower
        {
            setTempo(tempo - 1);
            break;
        }
        case 'm': //m for more
        {
            Track* selectedTrack = getSelectedTrack();
            if(selectedTrack != NULL)
            {
                selectedTrack->increaseSubdivision();
            }
            else
            {
                printf("no valid track found\n");
            }
        }
        case 'l':
        {
            Track* selectedTrack = getSelectedTrack();
            if(selectedTrack != NULL)
            {
                selectedTrack->decreaseSubdivision();
            }
            else
            {
                printf("no valid track found\n");
            }
        }
        default:
        {
            break;
        }
    }
    return false;
}

