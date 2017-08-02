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

/** An LV2 parameter, e.g. a Control input/output */
class LV2Parameter :  public Parameter
{
public:
    LV2Parameter (const LilvPlugin* pg, const LilvPort* pt)
        : Parameter (),
          portIndex (lilv_port_get_index (pg, pt)),
          plugin (pg), port (pt)
    {
        LilvNode* n = lilv_port_get_name (plugin, port);
        const LilvNode* s = lilv_port_get_symbol (plugin, port);
        setName (lilv_node_as_string (n));
        setSymbol (lilv_node_as_string (s));
        lilv_node_free (n);

        LilvScalePoints* points = lilv_port_get_scale_points (plugin, port);
        LILV_FOREACH (scale_points, iter, points)
        {
            const LilvScalePoint* point = lilv_scale_points_get (points, iter);
            scaleVals.add (lilv_node_as_float (lilv_scale_point_get_value (point)));
            scaleNames.add (CharPointer_UTF8 (lilv_node_as_string (lilv_scale_point_get_label (point))));
        }
        lilv_scale_points_free (points);
    }

    int getNumSteps() const { return scaleVals.size(); }
    uint32 getPortIndex() const { return portIndex; }
    void setPortIndex (uint32 index) { portIndex = index; }

private:

    uint32 portIndex;
    const LilvPlugin* plugin;
    const LilvPort*   port;
    Array<float>      scaleVals;
    StringArray       scaleNames;
};
