/*
  skindial.hpp - __CLASS__ Definition

  Copyright (C) 2014  Kushview, LLC.  All rights reserved.
  Michael Fisher <mfisher@bketech.com>

 */
/** @skindial.hpp */

#ifndef ELEMENT_SKINDIAL_H
#define ELEMENT_SKINDIAL_H

   class SkinDial : public Slider
   {
   public:

      SkinDial()
        : Slider(), nframes (0), frame (0), pixel (0)
      {
         img = Image::null;
         setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
         setSliderStyle (Slider::Rotary);
      }

      inline void
      setImage (const Image& source)
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
          
         const int size = frameSize();
         setSize (size, size);

      }

      inline void
      paint (Graphics& g)
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
            g.drawImage (img, 0, 0, size, size, 0, pixel, size, size, false);
         else
            g.drawImage (img, 0, 0, size, size, pixel, 0, size, size, false);
      }

   private:
      Image img;
      int nframes, frame, pixel;

      inline void updateFramePixel()
      {
         const double ratio = valueToProportionOfLength (getValue());
         frame = juce::roundDoubleToInt (nframes * ratio);
         pixel = isImageVertical() ? frame * img.getWidth() : frame * img.getHeight();
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
