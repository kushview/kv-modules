/*
  SkinDial.h

  Copyright (C) 2014  Kushview, LLC.  All rights reserved.
  Michael Fisher <mfisher@bketech.com>

*/

#ifndef ELEMENT_SKINDIAL_H
#define ELEMENT_SKINDIAL_H

   class SkinDial : public Slider
   {
   public:

       explicit SkinDial (const String& name = String::empty)
        : Slider (name),
          nframes (0),
          frame (0),
          pixel (0),
          scale(1)
      {
         img = Image::null;
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

          if (img != Image::null && 
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
         if (img == Image::null)
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
         frame = juce::roundDoubleToInt ((double)(nframes - 1) * ratio);
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

#endif /* ELEMENT_SKINDIAL_H */
