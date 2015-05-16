/*
 ==============================================================================

 This file is part of the JUCETICE project - Copyright 2009 by Lucio Asnaghi.

 JUCETICE is based around the JUCE library - "Jules' Utility Class Extensions"
 Copyright 2007 by Julian Storer.

 ------------------------------------------------------------------------------

 JUCE and JUCETICE can be redistributed and/or modified under the terms of
 the GNU General Public License, as published by the Free Software Foundation;
 either version 2 of the License, or (at your option) any later version.

 JUCE and JUCETICE are distributed in the hope that they will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with JUCE and JUCETICE; if not, visit www.gnu.org/licenses or write to
 Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 Boston, MA 02111-1307 USA

 ==============================================================================

   @author  Rui Nuno Capela
   @tweaker Lucio Asnaghi

 ==============================================================================
*/

#if EL_COMPLETION
#include "JuceHeader.h"
#endif

// Meter level limits (in dB).
#define HQ_METER_MAXDB        (+4.0f)
#define HQ_METER_MINDB        (-70.0f)

// The decay rates (magic goes here :).
// - value decay rate (faster)
#define HQ_METER_DECAY_RATE1    (1.0f - 3E-2f)
// - peak decay rate (slower)
#define HQ_METER_DECAY_RATE2    (1.0f - 3E-6f)
// Number of cycles the peak stays on hold before fall-off.
#define HQ_METER_PEAK_FALLOFF    16

HighQualityMeterValue::HighQualityMeterValue (HighQualityMeter *pMeter)
  : meter (pMeter),
    value (0.0f),
    valueHold (0),
    valueDecay (HQ_METER_DECAY_RATE1),
    peak (0),
    peakHold (0),
    peakDecay (HQ_METER_DECAY_RATE2),
    peakColor (HighQualityMeter::Color6dB)
{
}

HighQualityMeterValue::~HighQualityMeterValue()
{
}

void HighQualityMeterValue::setValue (const float fValue)
{
    value = fValue;
}

void HighQualityMeterValue::peakReset ()
{
    peak = 0;
}

void HighQualityMeterValue::refresh ()
{
    if (value > 0.001f || peak > 0)
        repaint();
}

void HighQualityMeterValue::paint (Graphics& g)
{
    const int w = getWidth();
    const int h = getHeight();
    int y = 0;

    if (isEnabled())
    {
        g.setColour (meter->color (HighQualityMeter::ColorBack));
        g.fillRect (0, 0, w, h);

        y = meter->iec_level (HighQualityMeter::Color0dB);

        g.setColour (meter->color (HighQualityMeter::ColorFore));
        g.drawLine (0, h - y, w, h - y);
    }
    else
    {
        g.setColour (Colours::black);
        g.fillRect (0, 0, w, h);
    }

    float dB = HQ_METER_MINDB;
    if (value > 0.0f)
        dB = 20.0f * log10f (value);

    if (dB < HQ_METER_MINDB)
        dB = HQ_METER_MINDB;
    else if (dB > HQ_METER_MAXDB)
        dB = HQ_METER_MAXDB;

    int y_over = 0;
    int y_curr = 0;

    y = meter->getIECScale (dB);
    if (valueHold < y)
    {
        valueHold = y;
        valueDecay = HQ_METER_DECAY_RATE1;
    }
    else
    {
        valueHold = int (float (valueHold * valueDecay));
        if (valueHold < y)
        {
            valueHold = y;
        }
        else
        {
            valueDecay *= valueDecay;
            y = valueHold;
        }
    }

    int iLevel;
    for (iLevel = HighQualityMeter::Color10dB;
         iLevel > HighQualityMeter::ColorOver && y >= y_over;
         iLevel--)
    {
        y_curr = meter->iec_level (iLevel);

//        g.setColour (m_pMeter->color (iLevel));

        g.setGradientFill (ColourGradient (meter->color (iLevel), 0, h - y_over,
                                           meter->color (iLevel-1), 0, h - y_curr,
                                           false));

        if (y < y_curr)
            g.fillRect(0, h - y, w, y - y_over);
        else
            g.fillRect(0, h - y_curr, w, y_curr - y_over);

        y_over = y_curr;
    }

    if (y > y_over)
    {
        g.setColour (meter->color (HighQualityMeter::ColorOver));
        g.fillRect (0, h - y, w, y - y_over);
    }

    if (peak < y)
    {
        peak = y;
        peakHold = 0;
        peakDecay = HQ_METER_DECAY_RATE2;
        peakColor = iLevel;
    }
    else if (++peakHold > meter->getPeakFalloff())
    {
        peak = int (float (peak * peakDecay));
        if (peak < y) {
            peak = y;
        } else {
            if (peak < meter->iec_level (HighQualityMeter::Color10dB))
                peakColor = HighQualityMeter::Color6dB;
            peakDecay *= peakDecay;
        }
    }

    g.setColour (meter->color (peakColor));
    g.drawLine (0, h - peak, w, h - peak);
}

void HighQualityMeterValue::resized ()
{
    peak = 0;
}


HighQualityMeter::HighQualityMeter (const int numPorts)
  : portCount (numPorts), // FIXME: Default port count.
    values (0),
    scale (0.0f),
    peakFalloff (HQ_METER_PEAK_FALLOFF)
{
    getLookAndFeel().setColour (HighQualityMeter::levelOverColourId, Colours::yellow.darker());
    getLookAndFeel().setColour (HighQualityMeter::level0dBColourId, Colours::whitesmoke);
    getLookAndFeel().setColour (HighQualityMeter::level3dBColourId, Colours::lightgreen);
    getLookAndFeel().setColour (HighQualityMeter::level6dBColourId, Colours::green);
    getLookAndFeel().setColour (HighQualityMeter::level10dBColourId, Colours::darkgreen.darker());
    getLookAndFeel().setColour (HighQualityMeter::backgroundColourId, Colours::transparentBlack);
    getLookAndFeel().setColour (HighQualityMeter::foregroundColourId, Colours::transparentWhite);

    for (int i = 0; i < LevelCount; i++)
        levels[i] = 0;

    colors[ColorOver] = findColour (levelOverColourId);
    colors[Color0dB]  = findColour (level0dBColourId);
    colors[Color3dB]  = findColour (level3dBColourId);
    colors[Color6dB]  = findColour (level6dBColourId);
    colors[Color10dB] = findColour (level10dBColourId);
    colors[ColorBack] = findColour (backgroundColourId);
    colors[ColorFore] = findColour (foregroundColourId);

    if (portCount > 0)
    {
        values = new HighQualityMeterValue* [portCount];

        for (int iPort = 0; iPort < portCount; iPort++)
        {
            values[iPort] = new HighQualityMeterValue (this);
            addAndMakeVisible (values[iPort]);
        }
    }
}


// Default destructor.
HighQualityMeter::~HighQualityMeter (void)
{
    for (int port = 0; port < portCount; port++)
        delete values[port];
    delete[] values;
}

void HighQualityMeter::resized ()
{
    scale = 0.85f * getHeight();

    levels[Color0dB]  = getIECScale (0.0f);
    levels[Color3dB]  = getIECScale (-3.0f);
    levels[Color6dB]  = getIECScale (-6.0f);
    levels[Color10dB] = getIECScale (-10.0f);

    const int size = getWidth() / portCount;

    for (int port = 0; port < portCount; ++port)
        values[port]->setBounds (port * size, 0, size, getHeight());
}

void HighQualityMeter::paint (Graphics& g)
{
/*
    g.drawBevel (0, 0, getWidth(), getHeight(), 1,
                 Colours::black.withAlpha(0.2f),
                 Colours::white.withAlpha(0.2f));
*/
}


// Child widget accessors.
int HighQualityMeter::getIECScale (const float dB) const
{
    float defaultScale = 1.0;

    if (dB < -70.0)
        defaultScale = 0.0;
    else if (dB < -60.0)
        defaultScale = (dB + 70.0) * 0.0025;
    else if (dB < -50.0)
        defaultScale = (dB + 60.0) * 0.005 + 0.025;
    else if (dB < -40.0)
        defaultScale = (dB + 50.0) * 0.0075 + 0.075;
    else if (dB < -30.0)
        defaultScale = (dB + 40.0) * 0.015 + 0.15;
    else if (dB < -20.0)
        defaultScale = (dB + 30.0) * 0.02 + 0.3;
    else // if (dB < 0.0)
        defaultScale = (dB + 20.0) * 0.025 + 0.5;

    return (int) (defaultScale * scale);
}

int HighQualityMeter::iec_level (const int index) const
{
    return levels[index];
}

int HighQualityMeter::getPortCount () const { return portCount; }
void HighQualityMeter::setPeakFalloff (const int newPeakFalloff) { peakFalloff = newPeakFalloff; }
int HighQualityMeter::getPeakFalloff() const { return peakFalloff; }

void HighQualityMeter::peakReset ()
{
    for (int iPort = 0; iPort < portCount; iPort++)
        values[iPort]->peakReset();
}

void HighQualityMeter::refresh ()
{
    for (int iPort = 0; iPort < portCount; iPort++)
        values[iPort]->refresh();
}

void HighQualityMeter::setValue (const int iPort, const float fValue)
{
    values[iPort]->setValue(fValue);
}

const Colour& HighQualityMeter::color (const int index) const
{
    return colors[index];
}


