/*
    This file is part of the Kushview Modules for JUCE
    Copyright (C) 2014  Kushview, LLC.  All rights reserved.

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
*/

#ifndef EL_SKINDIAL_H
#define EL_SKINDIAL_H

   class SkinDial : public Slider
   {
   public:

       explicit SkinDial (const String& name = String())
        : Slider (name),
          nframes (0),
          frame (0),
          pixel (0),
          scale(1)
      {
         img = Image();
         setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
         setSliderStyle (Slider::Rotary);
      }

      inline bool hitTest (int x, int y) override
       {
           x *= scale;
           y *= scale;
           return img.getPixelAt(x, y).getAlpha() == 0xFF;
       }

      inline void setImage (const Image& source)
      {
         img = source;

          if (! img.isNull() &&
              img.getWidth() >= 1 &&
              img.getHeight() >= 1)
          {
              nframes = isImageVertical() ? (img.getHeight() / img.getWidth())
                                          : (img.getWidth()  / img.getHeight());
          }
          else
          {
              nframes = 1;
          }

          jassert (nframes >= 1);
         const int size = frameSize();
         setSize (size, size);

      }

       inline void setScale (const int newScale)
       {
           if (newScale < 1)
               scale = 1;
           else
               scale = newScale;
       }

      inline void paint (Graphics& g) override
      {
         if (img.isNull())
         {
            Slider::paint (g);
            return;
         }

         // should probably do this somewhere else
         updateFramePixel();

         const int size (frameSize());

         if (isImageVertical())
            g.drawImage (img, 0, 0, size / scale, size / scale,
                         0, pixel, size, size, false);
         else
            g.drawImage (img, 0, 0, size / scale, size / scale,
                         pixel, 0, size, size, false);
      }

   private:
      Image img;
      int nframes, frame, pixel, scale;

      inline void updateFramePixel()
      {
         const double ratio = valueToProportionOfLength (getValue());
         frame = juce::roundToInt ((double)(nframes - 1) * ratio);
         pixel = frame * frameSize();
      }

      inline bool isImageVertical() const
      {
         return img.getHeight() > img.getWidth();
      }

      inline int  frameSize() const
      {
         return isImageVertical() ? img.getWidth() : img.getHeight();
      }

   };

#endif /* EL_SKINDIAL_H */
