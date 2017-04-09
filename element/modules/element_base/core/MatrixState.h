#ifndef UH_MATRIX_STATE_H
#define UH_MATRIX_STATE_H

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

    ~MatrixState() { }

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

    inline bool isCellToggled (int row, int column) const { return toggled [getIndexForCell (row, column)]; }

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
    
    inline void resize (int r, int c)
    {
        if (r < 1) r = 1;
        if (c < 1) c = 1;
        
        BigInteger bits;
        bits.setRange (0, r * c, false);
        
        // TODO: retain set bits
        
        numRows = r; numColumns = c;
        toggled.swapWith (bits);
    }
    
    inline bool sameSizeAs(const MatrixState& o)
    {
        return this->numRows == o.numRows &&
            this->numColumns == o.numColumns;
    }
private:
    BigInteger toggled;
    int numRows, numColumns;
};

#endif // UH_MATRIX_STATE_H
