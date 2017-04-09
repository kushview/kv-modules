#ifndef EL_PATCH_MATRIX_COMPONENT_H
#define EL_PATCH_MATRIX_COMPONENT_H

class QuadrantLayout : public Component
{
public:
    enum Quadrant { Q1 = 0, Q2, Q3, Q4 };

    QuadrantLayout();
    virtual ~QuadrantLayout();

    void paint (Graphics& g) override;
    virtual void resized() override;

    void setQuadrantComponent (QuadrantLayout::Quadrant q, Component*);
    Component* getQauadrantComponent (QuadrantLayout::Quadrant q) const;

protected:
    virtual void updateCenter();
    void setCenter (const int x, const int y);
    void setCenterX (const int x);
    void setCenterY (const int y);

private:
    Rectangle<int> q1area, q2area, q3area, q4area;
    int centerX, centerY;
    OwnedArray<Component> quadrants;
    bool deleteQuadrants;
    void updateQuadrantBounds();
};

class PatchMatrixComponent : public Component
{
public:
    PatchMatrixComponent();
    ~PatchMatrixComponent();

    void setMatrixCellSize (const int thickness);
    void setMatrixCellSize (const int horizontal, const int vertical);

    virtual int getNumColumns() = 0;
    virtual int getNumRows() = 0;
    virtual void paintMatrixCell (Graphics& g, const int width, const int height,
                                  const int row, const int column) = 0;
    virtual void matrixCellClicked (const int row, const int col, const MouseEvent& ev);


    void mouseDown (const MouseEvent& ev) override;
    void paint (Graphics &g) override;

    void setThickness (const int thickness) { verticalThickness = horizontalThickness = thickness; repaint(); }
    int getRowThickness() const { return verticalThickness; }
    int getColumnThickness() const { return horizontalThickness; }
    
    int getColumnForPixel (const int x);
    int getRowForPixel (const int y);
    void setOffsetX (const int x) { offsetX = x; }
    void setOffsetY (const int y) { offsetY = y; }

private:
    int verticalThickness, horizontalThickness;
    int offsetX, offsetY;
};

#endif // EL_PATCH_MATRIX_COMPONENT_H
