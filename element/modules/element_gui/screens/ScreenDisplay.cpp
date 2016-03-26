/*
    This file is part of Element
    Copyright (C) 2015  Kushview, LLC. All rights reserved.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    GUI class created by the Introjucer!
*/

class ScreenDisplay::Dispatch : public Timer
{
public:

    Dispatch (ScreenDisplay& d)
        : display (d)
    {
        setFrequencyMillis (64);
        startTimer (milliseconds);
    }

    ~Dispatch()
    {
        stopTimer();
    }

    void setFrequencyMillis (int millis)
    {
        const bool wasRunning = isTimerRunning();

        if (wasRunning)
            stopTimer();

        milliseconds = millis;

        if (wasRunning && milliseconds > 0)
            startTimer (milliseconds);
    }

    void timerCallback () {
        display.runDispatchLoop();
    }

    /** refresh rate in hertz */
    double refreshRate() const
    {
        if (milliseconds <= 0)
            return 0.0f;

        return  1000.f / (double) milliseconds;
    }

private:
    ScreenDisplay& display;
    int milliseconds;
};

class DisplayProgressBar :  public ProgressBar
{
    ScreenDisplay& display;

public:
    DisplayProgressBar (ScreenDisplay& parent)
        : ProgressBar (val),
          display (parent),
          val(0.0f)
    {
        setPercentageDisplay (true);
        setInterceptsMouseClicks (false, false);
        setAlwaysOnTop (true);
        setVisible (false);
        setAlpha (0.6);
    }

    void handleProgress (float p)
    {
        val = p;
        repaint();
        MessageManager::getInstance()->runDispatchLoopUntil (1);
    }

    void handleStatus (const String& msg)
    {
        display.setTitle (msg);
    }

    void handleProgressStart()
    {
        val = 0.0f;
        setVisible (true);
    }

    void handleProgressFinished()
    {
        setVisible (false);
        display.setTitle ("Finished Loading...");
    }

private:
    double val;
};

ScreenDisplay::ScreenDisplay ()
{
    addAndMakeVisible (comboBox = new ComboBox ("new combo box"));
    comboBox->setEditableText (false);
    comboBox->setJustificationType (Justification::centredLeft);
    comboBox->setTextWhenNothingSelected (TRANS("Chan"));
    comboBox->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    comboBox->addItem (TRANS("1"), 1);
    comboBox->addItem (TRANS("2"), 2);
    comboBox->addItem (TRANS("3"), 3);
    comboBox->addItem (TRANS("4"), 4);
    comboBox->addItem (TRANS("5"), 5);
    comboBox->addItem (TRANS("6"), 6);
    comboBox->addItem (TRANS("7"), 7);
    comboBox->addItem (TRANS("8"), 8);
    comboBox->addItem (TRANS("9"), 9);
    comboBox->addItem (TRANS("10"), 10);
    comboBox->addItem (TRANS("11"), 11);
    comboBox->addItem (TRANS("12"), 12);
    comboBox->addItem (TRANS("13"), 13);
    comboBox->addItem (TRANS("14"), 14);
    comboBox->addItem (TRANS("15"), 15);
    comboBox->addItem (TRANS("16"), 16);
    comboBox->addListener (this);

    addAndMakeVisible (title = new Label ("title",
                                          TRANS("New Instrument")));
    title->setTooltip (TRANS("Current Instrument"));
    title->setFont (Font (18.00f, Font::plain));
    title->setJustificationType (Justification::centredLeft);
    title->setEditable (false, true, false);
    title->setColour (Label::textColourId, Colours::chartreuse);
    title->setColour (TextEditor::textColourId, Colour (0xffe4e4e4));
    title->setColour (TextEditor::backgroundColourId, Colour (0x00000000));
    title->addListener (this);

    addAndMakeVisible (noteLabel = new Label ("note-label",
                                              TRANS("C0-1")));
    noteLabel->setTooltip (TRANS("Current Note"));
    noteLabel->setFont (Font (17.00f, Font::bold));
    noteLabel->setJustificationType (Justification::centredRight);
    noteLabel->setEditable (false, false, false);
    noteLabel->setColour (Label::textColourId, Colour (0xffb0c19f));
    noteLabel->setColour (TextEditor::textColourId, Colour (0xffe8e8e8));
    noteLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));
    noteLabel->setColour (TextEditor::highlightColourId, Colour (0x40545454));

    addAndMakeVisible (screen = new Component());
    screen->setName ("screen");


    screens = new ScreenManager();
    currentScreen = nullptr;

    setSize (282, 147);

    title->addMouseListener (this, false);
    startTimer (1500);
}

ScreenDisplay::~ScreenDisplay()
{
    stopTimer();

    comboBox = nullptr;
    title = nullptr;
    noteLabel = nullptr;
    screen = nullptr;
    dispatch = nullptr;
}

void ScreenDisplay::paint (Graphics& g)
{
    g.fillAll (Colour (0xa00e0e0e));
}

void ScreenDisplay::resized()
{
    comboBox->setBounds (6, 4, 48, 16);
    title->setBounds (57, -1, 223, 24);
    noteLabel->setBounds (getWidth() - 6 - 59, 0, 59, 24);
    screen->setBounds (0, 24, getWidth() - 0, getHeight() - 24);
    if (currentScreen)
        currentScreen->setBounds (0, 24, getWidth() - 0, getHeight() - 24);
}

void ScreenDisplay::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == comboBox)
    {

    }
}

void ScreenDisplay::labelTextChanged (Label* labelThatHasChanged)
{
    if (labelThatHasChanged == title)
    {

    }
}

void ScreenDisplay::mouseDown (const MouseEvent& e)
{
    if (e.mods.isPopupMenu())
    {
    }
}

void ScreenDisplay::mouseDoubleClick (const MouseEvent& e)
{
    if (e.originalComponent == title) {

    }
}

void ScreenDisplay::runDispatchLoop() { }

void ScreenDisplay::setScreen (const int i)
{
    if (currentScreen)
        removeChildComponent (currentScreen);

    this->currentScreen = screens->getScreen (*this, (int) i);

    if (currentScreen) {
        addAndMakeVisible (currentScreen);
    }

    resized();
}

void ScreenDisplay::setTitle (const String& t)
{
    title->setText (t, dontSendNotification);
}


void ScreenDisplay::timerCallback()
{

}
