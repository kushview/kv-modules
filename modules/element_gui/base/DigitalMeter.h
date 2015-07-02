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

#ifndef ELEMENT_HIGH_QUALITY_METER_H
#define ELEMENT_HIGH_QUALITY_METER_H

class DigitalMeter;
class DigitalMeterValue;

/** Creates VU meters with scale in decibels */
class DigitalMeterValue : public Component
{
public:
    DigitalMeterValue (DigitalMeter *pMeter);
    ~DigitalMeterValue();

    /** Frame value accessors. */
    void setValue (const float newValue);

    /** Value refreshment. */
    void refresh();

    /** Reset peak holder. */
    void resetPeak();

    virtual void paint (Graphics& g);
    
    /** @internal */
    void resized();

private:
    friend class DigitalMeter;
    DigitalMeter *meter;

    float value;
    int   valueHold;
    float valueDecay;
    int   peak;
    int   peakHold;
    float peakDecay;
    int   peakColor;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DigitalMeterValue)
};


/** Creates VU meters with scale in decibels */
class DigitalMeter : public Component
{
public:
    DigitalMeter (const int numPorts, bool horizontal = false);
    ~DigitalMeter();

    /** Returns the number of ports on this meter */
    int getPortCount() const;
    void setValue (const int port, const float value);
    int getIECScale (const float dB) const;
    int getIECLevel (const int index) const;
    void setPeakFalloff (const int newPeakFalloff);
    int getPeakFalloff() const;
    void refresh();
    void resetPeaks();

    bool isHorizontal() const { return horizontal; }
    bool isVertical()   const { return ! horizontal; }

    enum {
        ColorOver    = 0,
        Color0dB     = 1,
        Color3dB     = 2,
        Color6dB     = 3,
        Color10dB    = 4,
        LevelCount   = 5,
        ColorBack    = 5,
        ColorFore    = 6,
        ColorCount   = 7
    };

    const Colour& color (const int index) const;

    enum ColoursIds
    {
        levelOverColourId    = 0x90900001,
        level0dBColourId     = 0x90900002,
        level3dBColourId     = 0x90900003,
        level6dBColourId     = 0x90900004,
        level10dBColourId    = 0x90900005,
        backgroundColourId   = 0x90900006,
        foregroundColourId   = 0x90900007
    };
    
    /** @internal */
    void paint (Graphics& g);
    /** @internal */
    void resized ();
    
protected:
    virtual DigitalMeterValue* createDigitalMeterValue();

private:
    friend class DigitalMeterValue;
    
    int portCount;
    DigitalMeterValue** values;
    float scale;
    int levels[LevelCount];
    Colour colors[ColorCount];
    int peakFalloff;
    bool horizontal;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DigitalMeter)
};

#endif  // ELEMENT_HIGH_QUALITY_METER_H
