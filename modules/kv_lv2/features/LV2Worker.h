/*
    This file is part of the element modules for the JUCE Library
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

#ifndef EL_LV2_WORKER_H
#define EL_LV2_WORKER_H

class LV2Worker :  public LV2Feature,
                   public WorkerBase
{
public:
    LV2Worker (WorkThread& thread, uint32 bufsize,
               LV2_Handle handle = nullptr,
               LV2_Worker_Interface* iface = nullptr);

    ~LV2Worker();

    void setInterface (LV2_Handle handle, LV2_Worker_Interface* iface);

    const String& getURI() const;
    const LV2_Feature* getFeature() const;

    void endRun();
    void processRequest (uint32 size, const void* data);
    void processResponse (uint32 size, const void* data);

private:
    String uri;
    LV2_Worker_Interface* worker;
    LV2_Handle plugin;
    LV2_Worker_Schedule data;
    LV2_Feature feat;
};


#endif /* EL_LV2_WORKER_H */
