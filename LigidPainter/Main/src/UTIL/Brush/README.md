LIGIDPAINTER'S BRUSH SYSTEM

Declerations are in the UTIL/Util.hpp

Reading *.ligidbrush file :
    Is mentioned in the ./ReadBrushFile.cpp ( + code sample)

Reading *.ligidbrush file :
    Is mentioned in the ./WriteBrushFile.cpp ( + code sample)

Brush system in the LigidPainter : 


Import Brushes -> Create a brush class and store necessary brush data in it. 

Using brushes 
            -> Move necessary brush data in the Brush class to the GUI elements. 
                (Kinda like paintingPanel.sections[2].elements[4].rangeBar.value = this->sizeJitter)
            
            -> Move the data in the GUI to the painter.brushProperties (is whole different struct)
            -> Then use the brush data in the painter class while painting

Modifying brushes
                    -> Move the necessary brush data to the Brush class from the GUI elements 
                        (Kinda like this->sizeJitter = paintingPanel.sections[2].elements[4].rangeBar.value)