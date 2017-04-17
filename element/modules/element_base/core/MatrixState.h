/*
    This file is part of the element modules for the JUCE Library
    Copyright (C) 2016-2017  Kushview, LLC.  All rights reserved.

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

#ifndef EL_MATRIX_STATE_H
#define EL_MATRIX_STATE_H

class MatrixState
{
public:
    MatrixState()
    {
        numRows = 1;
        numColumns = 1;
        toggled.setRange (0, numRows * numColumns, false);
    }

    MatrixState (const int rows, const int cols)
    {
        numRows = rows;
        numColumns = cols;
        toggled.setRange (0, numRows * numColumns, false);
    }

    virtual ~MatrixState() { }

    inline int getIndexForCell (int row, int column) const { return (row * numColumns) + column; }
    inline int getNumRows()    const { return numRows; }
    inline int getNumColumns() const { return numColumns; }

    inline void connect (int row, int column)
    {
        if (row < numRows && column < numColumns) {
            const int index = getIndexForCell(row, column);
            toggled.setBit(index, true);
        }
    }

    inline void disconnect (int row, int column)
    {
        jassert(row < numRows && column < numColumns);
        const int index = getIndexForCell (row, column);
        toggled.setBit (index, false);
    }
    
    inline bool connected (const int row, const int col) const {
        return toggled [getIndexForCell (row, col)];
    }
    
    inline bool isCellToggled (int r, int c) const { return connected (r, c); }
    inline bool toggleCell (int row, int column)
    {
        if (row < numRows && column < numColumns)
        {
            const int index = getIndexForCell (row, column);
            const bool toggle = ! toggled[index];
            toggled.setBit (index, toggle);
            return true;
        }

        return false;
    }

    inline bool connectedAtIndex (const int index) const { return toggled [index]; }

   #if JUCE_MODULE_AVAILABLE_juce_data_structures
    inline ValueTree createValueTree (const String& type = "matrix") const
    {
        ValueTree tree (Identifier::isValidIdentifier(type) ? type : "matrix");
        tree.setProperty ("numRows", numRows, nullptr);
        tree.setProperty ("numColumns", numColumns, nullptr);
        tree.setProperty ("toggled", toggled.toString(2), nullptr);
        return tree;
    }

    inline void restoreFromValueTree (const ValueTree& tree)
    {
        numRows = tree.getProperty ("numRows", 0);
        numColumns = tree.getProperty ("numColumns", 0);
        toggled.parseString (tree.getProperty("toggled").toString(), 2);
    }
   #endif
    
    /** Resize the matrix to the specified rows and column sizes */
    void resize (int newNumRows, int newNumColumns);
    
    /** Returns true if the matrices are the same size */
    inline bool sameSizeAs (const MatrixState& o)
    {
        return this->numRows == o.numRows &&
               this->numColumns == o.numColumns;
    }
    
private:
    BigInteger toggled;
    int numRows, numColumns;
};

#endif  /* EL_MATRIX_STATE_H */
