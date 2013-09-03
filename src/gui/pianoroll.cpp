/*
 * pianoroll.cpp
 *
 *  Created on: Jan 26, 2013
 *      Author: mfisher
 */

#if 0

#include "gui/pianoroll.hpp"

namespace Drumbot {




void getKeyPosition (int midiNoteNumber, float keyWidth, int& x, int& w)
{
    jassert (midiNoteNumber >= 0 && midiNoteNumber < 128);

    static const float blackNoteWidth = 0.7f;

    static const float notePos[] = { 0.0f, 1 - blackNoteWidth * 0.6f,
                                     1.0f, 2 - blackNoteWidth * 0.4f,
                                     2.0f,
                                     3.0f, 4 - blackNoteWidth * 0.7f,
                                     4.0f, 5 - blackNoteWidth * 0.5f,
                                     5.0f, 6 - blackNoteWidth * 0.3f,
                                     6.0f };

    static const float widths[] = { 1.0f, blackNoteWidth,
                                    1.0f, blackNoteWidth,
                                    1.0f,
                                    1.0f, blackNoteWidth,
                                    1.0f, blackNoteWidth,
                                    1.0f, blackNoteWidth,
                                    1.0f };

    const int octave = midiNoteNumber / 12;
    const int note = midiNoteNumber % 12;

    x = roundFloatToInt ((octave * 7.0f * keyWidth) + notePos [note] * keyWidth);
    w = roundFloatToInt (widths [note] * keyWidth);
}



PianoRoll::PianoRoll()
    : keyboard (state, MidiKeyboardComponent::verticalKeyboardFacingRight)
{
    keyboard.setKeyWidth(20);
    addAndMakeVisible (&keyboard);
    keyboard.setScrollButtonsVisible (false);
    keyboard.getBlackNoteLength();
    resized();
}


PianoRoll::~PianoRoll ()
{

}

void
PianoRoll::paint (Graphics& g)
{
    Timeline::paint (g);

    for (int i = keyboard.getLowestVisibleKey(); i < 128; ++i)
    {
        int y, w;
        getKeyPosition (i, keyboard.getKeyWidth(), y, w);
        g.drawHorizontalLine (y, 0, getWidth());
    }

#if 0

    int keyspan = keyboard.getKeyStartPosition (keyboard.getLowestVisibleKey()) +
                  keyboard.getKeyStartPosition (keyboard.getLowestVisibleKey() + 12);

    g.setColour (Colour (0xff313131));

    for (double y = getHeight() - 1; y >= 0; y -= keyspan)
    {
        g.drawHorizontalLine (y, 0, getWidth());
        g.drawHorizontalLine (y - (keyboard.getKeyWidth()*3), 0, getWidth());

        float kw = keyboard.getKeyWidth() / 2.0;

        int c = 12;
        for (double yy = kw; yy <= keyspan + kw; yy += kw)
        {
            if (c == 1 || c == 3 || c == 6 || c == 8 || c == 10)
                g.setColour (Colour (0xff414141));
            else
                g.setColour (Colour (0xff616161));

            g.fillRect (0.0f, y + yy, (float)getWidth(), kw );
            --c;
        }

    }
#endif
}

void
PianoRoll::resized()
{
    Timeline::resized();

    keyboard.setSize (100, getHeight());
    RectangleList list;
    std::cout << "lowest: " << keyboard.getLowestVisibleKey() << std::endl;
    std::cout << keyboard.getKeyStartPosition (keyboard.getLowestVisibleKey() + 1) << std::endl;
}

}}

#endif
