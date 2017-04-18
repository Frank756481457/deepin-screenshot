#include "shapeswidget.h"

#include "utils/calculaterect.h"

#include <QApplication>
#include <QPainter>
#include <QDebug>

const int DRAG_BOUND_RADIUS = 8;
const int SPACING = 12;

ShapesWidget::ShapesWidget(QWidget *parent)
    : QFrame(parent),
      m_selectedIndex(-1),
      m_isMoving(false),
      m_isSelected(false),
      m_isShiftPressed(false),
      m_penColor(Qt::red)
{
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
    setAcceptDrops(true);
    installEventFilter(this);
}

ShapesWidget::~ShapesWidget() {

}

void ShapesWidget::setCurrentShape(QString shapeType) {
    m_currentType = shapeType;
    qDebug() << "shape" << m_currentType;
    qApp->setOverrideCursor(setCursorShape(m_currentType));
}

void ShapesWidget::setPenColor(QColor color) {
    m_penColor = color;
    qDebug() << "shapesWidget Color:" << color;
    update();
}

void ShapesWidget::clearSelected() {
    for(int j = 0; j < m_selectedShape.mainPoints.length(); j++) {
        m_selectedShape.mainPoints[j] = QPointF(0, 0);
    }
}

bool ShapesWidget::clickedOnShapes(QPointF pos) {
    bool onShapes = false;
    m_currentDiagPoints.masterPoint = QPointF(0, 0);
    m_currentDiagPoints.deputyPoint = QPointF(0, 0);
    m_selectedIndex = -1;

    for (int i = 0; i < m_shapes.length(); i++) {
        bool currentOnShape = false;
        if (m_shapes[i].type == "rectangle") {
            if (clickedOnRectPoint(m_shapes[i].mainPoints, pos)) {
                currentOnShape = true;
            }
        }
        if (m_shapes[i].type == "oval") {
            if (clickedOnEllipsePoint(m_shapes[i].mainPoints, pos)) {
                currentOnShape = true;
            }
        }

        if (m_shapes[i].type == "line") {
            if (clickedOnLinePoint(m_shapes[i].mainPoints, m_shapes[i].points, pos)) {
                currentOnShape = true;
            }
        }

        if (currentOnShape) {
            m_currentSelectedDiagPoints.masterPoint = m_shapes[i].mainPoints[0];
            m_currentSelectedDiagPoints.deputyPoint = m_shapes[i].mainPoints[3];
            m_selectedShape = m_shapes[i];
            m_selectedIndex = i;

            onShapes = true;
            break;
        } else {
            update();
            continue;
        }
    }
        return onShapes;
}



    //TODO: selectUnique
bool ShapesWidget::clickedOnRectPoint(FourPoints rectPoints, QPointF pos) {
    m_isSelected = false;
    m_isResize = false;
    m_isRotated = false;

    m_pressedPoint = QPointF(0, 0);
    QPointF point1 = rectPoints[0];
    QPointF point2 = rectPoints[1];
    QPointF point3 = rectPoints[2];
    QPointF point4 = rectPoints[3];

    FourPoints otherFPoints = getAnotherFPoints(rectPoints);
    if (pointClickIn(point1, pos)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = First;
        m_resizeDirection = TopLeft;
        m_pressedPoint = pos;
        return true;
    } else if (pointClickIn(point2, pos)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = Second;
        m_resizeDirection = BottomLeft;
        m_pressedPoint = pos;
        return true;
    } else if (pointClickIn(point3, pos)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = Third;
        m_resizeDirection = TopRight;
        m_pressedPoint = pos;
        return true;
    } else if (pointClickIn(point4, pos)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = Fourth;
        m_resizeDirection = BottomRight;
        m_pressedPoint = pos;
        return true;
    }  else if (pointClickIn(otherFPoints[0], pos)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = Fifth;
        m_resizeDirection = Left;
        m_pressedPoint = pos;
        return true;
    } else if (pointClickIn(otherFPoints[1], pos)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = Sixth;
        m_resizeDirection = Top;
        m_pressedPoint = pos;
        return true;
    } else if (pointClickIn(otherFPoints[2], pos)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = Seventh;
        m_resizeDirection = Right;
        m_pressedPoint = pos;
        return true;
    } else if (pointClickIn(otherFPoints[3], pos)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = Eighth;
        m_resizeDirection = Bottom;
        m_pressedPoint = pos;
        return true;
    } else if (rotateOnPoint(rectPoints, pos)) {
        m_isSelected = true;
        m_isRotated = true;
        m_isResize = false;
        m_resizeDirection = Rotate;
        m_pressedPoint = pos;
        return true;
    } else if (pointOnLine(rectPoints[0], rectPoints[1], pos) || pointOnLine(rectPoints[1],
        rectPoints[3], pos) || pointOnLine(rectPoints[3], rectPoints[2], pos) ||
        pointOnLine(rectPoints[2], rectPoints[0], pos)) {
        m_isSelected = true;
        m_isResize = false;
        m_resizeDirection = Moving;
        m_pressedPoint = pos;
        return true;
    } else {
        m_isSelected = false;
        m_isResize = false;
        m_isRotated = false;
    }

    return false;
}

bool ShapesWidget::clickedOnEllipsePoint(FourPoints mainPoints, QPointF pos) {
    m_isSelected = false;
    m_isResize = false;
    m_isRotated = false;

    m_pressedPoint = QPoint(0, 0);
    FourPoints otherFPoints = getAnotherFPoints(mainPoints);
    if (pointClickIn(mainPoints[0], pos)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = First;
        m_resizeDirection = TopLeft;
        m_pressedPoint = pos;
        return true;
    } else if (pointClickIn(mainPoints[1], pos)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = Second;
        m_resizeDirection = BottomLeft;
        m_pressedPoint = pos;
        return true;
    } else if (pointClickIn(mainPoints[2], pos)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = Third;
        m_resizeDirection = TopRight;
        m_pressedPoint = pos;
        return true;
    } else if (pointClickIn(mainPoints[3], pos)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = Fourth;
        m_resizeDirection = BottomRight;
        m_pressedPoint = pos;
        return true;
    }  else if (pointClickIn(otherFPoints[0], pos)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = Fifth;
        m_resizeDirection = Left;
        m_pressedPoint = pos;
        return true;
    } else if (pointClickIn(otherFPoints[1], pos)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = Sixth;
        m_resizeDirection = Top;
        m_pressedPoint = pos;
        return true;
    } else if (pointClickIn(otherFPoints[2], pos)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = Seventh;
        m_resizeDirection = Right;
        m_pressedPoint = pos;
        return true;
    } else if (pointClickIn(otherFPoints[3], pos)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = Eighth;
        m_resizeDirection = Bottom;
        m_pressedPoint = pos;
        return true;
    } else if (rotateOnPoint(mainPoints, pos)) {
        m_isSelected = true;
        m_isRotated = true;
        m_isResize = false;
        m_resizeDirection = Rotate;
        m_pressedPoint = pos;
        return true;
    }  else if (pointOnEllipse(mainPoints, pos)) {
            m_isSelected = true;
            m_isResize = false;

            m_resizeDirection = Moving;
            m_pressedPoint = pos;
            return true;
    } else {
        m_isSelected = false;
        m_isResize = false;
        m_isRotated = false;
    }

    return false;
}

bool ShapesWidget::clickedOnLinePoint(FourPoints mainPoints,
                                      QList<QPointF> points, QPointF pos) {
    m_isSelected = false;
    m_isResize = false;
    m_isRotated = false;

    m_pressedPoint = QPoint(0, 0);
    FourPoints otherFPoints = getAnotherFPoints(mainPoints);
    if (pointClickIn(mainPoints[0], pos)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = First;
        m_resizeDirection = TopLeft;
        m_pressedPoint = pos;
        return true;
    } else if (pointClickIn(mainPoints[1], pos)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = Second;
        m_resizeDirection = BottomLeft;
        m_pressedPoint = pos;
        return true;
    } else if (pointClickIn(mainPoints[2], pos)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = Third;
        m_resizeDirection = TopRight;
        m_pressedPoint = pos;
        return true;
    } else if (pointClickIn(mainPoints[3], pos)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = Fourth;
        m_resizeDirection = BottomRight;
        m_pressedPoint = pos;
        return true;
    }  else if (pointClickIn(otherFPoints[0], pos)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = Fifth;
        m_resizeDirection = Left;
        m_pressedPoint = pos;
        return true;
    } else if (pointClickIn(otherFPoints[1], pos)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = Sixth;
        m_resizeDirection = Top;
        m_pressedPoint = pos;
        return true;
    } else if (pointClickIn(otherFPoints[2], pos)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = Seventh;
        m_resizeDirection = Right;
        m_pressedPoint = pos;
        return true;
    } else if (pointClickIn(otherFPoints[3], pos)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = Eighth;
        m_resizeDirection = Bottom;
        m_pressedPoint = pos;
        return true;
    } else if (rotateOnPoint(mainPoints, pos)) {
        m_isSelected = true;
        m_isRotated = true;
        m_isResize = false;
        m_resizeDirection = Rotate;
        m_pressedPoint = pos;
        return true;
    }  else if (pointOnArLine(points, pos)) {
            m_isSelected = true;
            m_isResize = false;

            m_resizeDirection = Moving;
            m_pressedPoint = pos;
            return true;
    } else {
        m_isSelected = false;
        m_isResize = false;
        m_isRotated = false;
    }

    return false;
}

bool ShapesWidget::hoverOnRect(FourPoints rectPoints, QPointF pos) {
    FourPoints tmpFPoints = getAnotherFPoints(rectPoints);
    if (pointClickIn(rectPoints[0], pos)) {
        m_resizeDirection = TopLeft;
        return true;
    } else if (pointClickIn(rectPoints[1], pos)) {
        m_resizeDirection = BottomLeft;
        return true;
    } else if (pointClickIn(rectPoints[2], pos)) {
        m_resizeDirection = TopRight;
        return true;
    } else if (pointClickIn(rectPoints[3], pos)) {
        m_resizeDirection = BottomRight;
        return true;
    } else if (rotateOnPoint(rectPoints, pos)) {
        m_resizeDirection = Rotate;
        return true;
    }  else if (pointClickIn(tmpFPoints[0], pos)) {
        m_resizeDirection = Left;
        return true;
    } else if (pointClickIn(tmpFPoints[1], pos)) {
        m_resizeDirection = Top;
        return true;
    }  else if (pointClickIn(tmpFPoints[2], pos)) {
        m_resizeDirection = Right;
        return true;
    } else if (pointClickIn(tmpFPoints[3], pos)) {
        m_resizeDirection = Bottom;
        return true;
    } else if (pointOnLine(rectPoints[0],  rectPoints[1], pos) || pointOnLine(rectPoints[1],
        rectPoints[3], pos)|| pointOnLine(rectPoints[3], rectPoints[2], pos) ||
               pointOnLine(rectPoints[2], rectPoints[0], pos)) {
        m_resizeDirection = Moving;
        return true;
    } else {
        m_resizeDirection = Outting;
    }
    return false;
}

bool ShapesWidget::hoverOnEllipse(FourPoints mainPoints, QPointF pos) {
    FourPoints tmpFPoints = getAnotherFPoints(mainPoints);

    if (pointClickIn(mainPoints[0], pos)) {
        m_resizeDirection = TopLeft;
        return true;
    } else if (pointClickIn(mainPoints[1], pos)) {
        m_resizeDirection = BottomLeft;
        return true;
    } else if (pointClickIn(mainPoints[2], pos)) {
        m_resizeDirection = TopRight;
        return true;
    } else if (pointClickIn(mainPoints[3], pos)) {
        m_resizeDirection = BottomRight;
        return true;
    } else if (rotateOnPoint(mainPoints, pos)) {
        m_resizeDirection = Rotate;
        return true;
    }  else if (pointClickIn(tmpFPoints[0], pos)) {
        m_resizeDirection = Left;
        return true;
    } else if (pointClickIn(tmpFPoints[1], pos)) {
        m_resizeDirection = Top;
        return true;
    }  else if (pointClickIn(tmpFPoints[2], pos)) {
        m_resizeDirection = Right;
        return true;
    } else if (pointClickIn(tmpFPoints[3], pos)) {
        m_resizeDirection = Bottom;
        return true;
    }  else if (pointOnEllipse(mainPoints, pos)) {
        m_isSelected = true;
        m_isResize = false;

        m_resizeDirection = Moving;
        m_pressedPoint = pos;
        return true;
   } else {
        m_resizeDirection = Outting;
    }
    return false;
}

bool ShapesWidget::hoverOnLine(FourPoints mainPoints, QList<QPointF> points, QPointF pos) {
    FourPoints tmpFPoints = getAnotherFPoints(mainPoints);

    if (pointClickIn(mainPoints[0], pos)) {
        m_resizeDirection = TopLeft;
        return true;
    } else if (pointClickIn(mainPoints[1], pos)) {
        m_resizeDirection = BottomLeft;
        return true;
    } else if (pointClickIn(mainPoints[2], pos)) {
        m_resizeDirection = TopRight;
        return true;
    } else if (pointClickIn(mainPoints[3], pos)) {
        m_resizeDirection = BottomRight;
        return true;
    } else if (rotateOnPoint(mainPoints, pos)) {
        m_resizeDirection = Rotate;
        return true;
    }  else if (pointClickIn(tmpFPoints[0], pos)) {
        m_resizeDirection = Left;
        return true;
    } else if (pointClickIn(tmpFPoints[1], pos)) {
        m_resizeDirection = Top;
        return true;
    }  else if (pointClickIn(tmpFPoints[2], pos)) {
        m_resizeDirection = Right;
        return true;
    } else if (pointClickIn(tmpFPoints[3], pos)) {
        m_resizeDirection = Bottom;
        return true;
    }  else if (pointOnArLine(points, pos)) {
        m_isSelected = true;
        m_isResize = false;

        m_resizeDirection = Moving;
        m_pressedPoint = pos;
        return true;
   } else {
        m_resizeDirection = Outting;
    }
    return false;
}
bool ShapesWidget::hoverOnShapes(Toolshape toolShape, QPointF pos) {

    if (toolShape.type == "rectangle") {
        return hoverOnRect(toolShape.mainPoints, pos);
    } else if (toolShape.type == "oval") {
        return hoverOnEllipse(toolShape.mainPoints, pos);
    } else if (toolShape.type == "line") {
        return hoverOnLine(toolShape.mainPoints, toolShape.points, pos);
    }

    return false;
}

bool ShapesWidget::rotateOnPoint(FourPoints mainPoints,
                                 QPointF pos) {
    bool result = hoverOnRotatePoint(mainPoints, pos);
    return result;
}

bool ShapesWidget::hoverOnRotatePoint(FourPoints mainPoints,
                                      QPointF pos) {
    QPointF rotatePoint = getRotatePoint(mainPoints[0], mainPoints[1],
                                                                        mainPoints[2], mainPoints[3]);
    rotatePoint = QPointF(rotatePoint.x() - 5, rotatePoint.y() - 5);
    bool result = false;
    if (pos.x() >= rotatePoint.x() - SPACING && pos.x() <= rotatePoint.x() + SPACING
            && pos.y() >= rotatePoint.y() - SPACING && pos.y() <= rotatePoint.y() + SPACING) {
        result = true;
    } else {
        result = false;
    }

    m_pressedPoint = rotatePoint;
    return result;
}

void ShapesWidget::handleDrag(QPointF oldPoint, QPointF newPoint)  {
    if (m_shapes[m_selectedIndex].mainPoints.length() == 4) {
        for(int i = 0; i < m_shapes[m_selectedIndex].mainPoints.length(); i++) {
            m_shapes[m_selectedIndex].mainPoints[i] = QPointF(
                        m_shapes[m_selectedIndex].mainPoints[i].x() + (newPoint.x() - oldPoint.x()),
                        m_shapes[m_selectedIndex].mainPoints[i].y() + (newPoint.y() - oldPoint.y())
                        );
        }

        for(int i = 0; i < m_shapes[m_selectedIndex].points.length(); i++) {
            m_shapes[m_selectedIndex].points[i] = QPointF(
                        m_shapes[m_selectedIndex].points[i].x() + (newPoint.x() - oldPoint.x()),
                        m_shapes[m_selectedIndex].points[i].y() + (newPoint.y() - oldPoint.y())
                        );
        }
    }
}

////////////////////TODO: perfect handleRotate..
void ShapesWidget::handleRotate(QPointF pos) {
    QPointF centerInPoint = QPointF((m_selectedShape.mainPoints[0].x() + m_selectedShape.mainPoints[3].x())/2,
                                                                 (m_selectedShape.mainPoints[0].y()+ m_selectedShape.mainPoints[3].y())/2);

    qreal angle = calculateAngle(m_pressedPoint, pos, centerInPoint)/35;

    for (int i = 0; i < 4; i++) {
        m_shapes[m_selectedIndex].mainPoints[i] = pointRotate(centerInPoint,
                                                              m_shapes[m_selectedIndex].mainPoints[i], angle);
    }  

    for(int k = 0; k < m_shapes[m_selectedIndex].points.length(); k++) {
        m_shapes[m_selectedIndex].points[k] = pointRotate(centerInPoint,
                                                              m_shapes[m_selectedIndex].points[k], angle);
    }

    m_selectedShape.mainPoints = m_shapes[m_selectedIndex].mainPoints;
    m_hoveredShape.mainPoints =  m_shapes[m_selectedIndex].mainPoints;
    m_pressedPoint = pos;
}

void ShapesWidget::handleResize(QPointF pos, int key) {
        if (m_isResize) {
            FourPoints tmpFourPoint;
            tmpFourPoint = m_selectedShape.mainPoints;
            qDebug() << "handleResize" << key << tmpFourPoint;
            FourPoints newResizeFPoints = resizePointPosition(tmpFourPoint[0], tmpFourPoint[1],
                                                 tmpFourPoint[2], tmpFourPoint[3], pos, key,  m_isShiftPressed);
            m_shapes[m_selectedIndex].mainPoints = newResizeFPoints;
            m_selectedShape.mainPoints = newResizeFPoints;
            m_hoveredShape.mainPoints = newResizeFPoints;
        }
}

void ShapesWidget::mousePressEvent(QMouseEvent *e) {
    m_pressedPoint = e->pos();
    m_isPressed = true;
    if (!clickedOnShapes(m_pressedPoint)) {
        qDebug() << "no one shape be clicked!";
        m_currentDiagPoints.masterPoint = QPoint(0, 0);
        m_currentDiagPoints.deputyPoint = QPoint(0, 0);
        clearSelected();

        m_currentShape.type = m_currentType;
        m_selectedIndex = -1;
        m_isRecording = true;
        if (m_pos1 == QPointF(0, 0)) {
            m_pos1 = e->pos();
            m_currentDiagPoints.masterPoint = m_pos1;
            if (m_currentType == "line") {
                m_currentShape.points.append(m_pos1);
            }
            update();
        }
    } else {
        m_isRecording = false;
        qDebug() << "some on shape be clicked!";
    }

    QFrame::mousePressEvent(e);
}

void ShapesWidget::mouseReleaseEvent(QMouseEvent *e) {
    m_isPressed = false;
    if (m_isRecording && !m_isSelected && m_pos2 != QPointF(0, 0)) {
        m_currentDiagPoints.deputyPoint = m_pos2;
        m_diagPointsList.append(m_currentDiagPoints);

        if (m_currentType != "line") {
            FourPoints rectFPoints = fourPointsOnRect(m_currentDiagPoints);
            m_currentShape.mainPoints = rectFPoints;
            m_shapes.append(m_currentShape);
        } else {
            FourPoints lineFPoints = fourPointsOfLine(m_currentShape.points);
            m_currentShape.mainPoints = lineFPoints;
            m_shapes.append(m_currentShape);

            m_currentShape.points.clear();
        }

        for (int i = 0; i < m_currentShape.mainPoints.length(); i++) {
            m_currentShape.mainPoints[i] = QPointF(0, 0);
        }
        qDebug() << "ShapesWidget num:" << m_shapes.length();
        m_isRecording = false;
        m_isMoving = false;
        m_currentDiagPoints.masterPoint = QPoint(0, 0);
        m_currentDiagPoints.deputyPoint = QPoint(0, 0);
        clearSelected();
    }

    m_pos1 = QPointF(0, 0);
    m_pos2 = QPointF(0, 0);

    update();
    QFrame::mouseMoveEvent(e);
}

void ShapesWidget::mouseMoveEvent(QMouseEvent *e) {
    m_isMoving = true;
    m_movingPoint = e->pos();

    if (m_isRecording && m_isPressed) {
        m_pos2 = e->pos();
        m_currentDiagPoints.deputyPoint = m_pos2;
        if (m_currentShape.type == "line") {
             m_currentShape.points.append(m_pos2);
        }
        update();
    } else if (!m_isRecording && m_isPressed) {
        if (m_isRotated && m_isPressed) {
            handleRotate(e->pos());
            update();
        }

        if (m_isResize && m_isPressed) {
            // resize function
             handleResize(QPointF(e->pos()), m_clickedKey);
            update();
            return;
        }

        if (m_isSelected && m_isPressed && m_selectedIndex != -1) {
            handleDrag(m_pressedPoint, m_movingPoint);
            m_selectedShape = m_shapes[m_selectedIndex];
            m_hoveredShape = m_shapes[m_selectedIndex];

            m_currentSelectedDiagPoints.masterPoint = m_shapes[m_selectedIndex].mainPoints[0];
            m_currentSelectedDiagPoints.deputyPoint =  m_shapes[m_selectedIndex].mainPoints[3];

            m_pressedPoint = e->pos();
            update();
        }
    } else {
        if (!m_isRecording) {
            m_isHovered = false;
            for (int i = 0; i < m_shapes.length(); i++) {
                 if (hoverOnShapes(m_shapes[i],  e->pos())) {
                     m_isHovered = true;
                     m_hoveredShape = m_shapes[i];
                     if (m_resizeDirection == Left) {
                         if (m_isSelected || m_isRotated) {
                            qApp->setOverrideCursor(Qt::SizeHorCursor);
                         } else {
                            qApp->setOverrideCursor(Qt::ClosedHandCursor);
                         }
                     } else if (m_resizeDirection == Top) {
                         if (m_isSelected || m_isRotated) {
                            qApp->setOverrideCursor(Qt::SizeVerCursor);
                         } else {
                            qApp->setOverrideCursor(Qt::ClosedHandCursor);
                         }
                     } else if (m_resizeDirection == Right) {
                         if (m_isSelected || m_isRotated) {
                            qApp->setOverrideCursor(Qt::SizeHorCursor);
                         } else {
                            qApp->setOverrideCursor(Qt::ClosedHandCursor);
                         }
                     } else if (m_resizeDirection == Bottom) {
                         if (m_isSelected || m_isRotated) {
                            qApp->setOverrideCursor(Qt::SizeVerCursor);
                         } else {
                            qApp->setOverrideCursor(Qt::ClosedHandCursor);
                         }
                     }  else if (m_resizeDirection == TopLeft) {
                         if (m_isSelected || m_isRotated) {
                            qApp->setOverrideCursor(Qt::SizeFDiagCursor);
                         } else {
                            qApp->setOverrideCursor(Qt::ClosedHandCursor);
                         }
                     } else if (m_resizeDirection == BottomLeft) {
                         if (m_isSelected || m_isRotated) {
                            qApp->setOverrideCursor(Qt::SizeBDiagCursor);
                         } else {
                            qApp->setOverrideCursor(Qt::ClosedHandCursor);
                         }
                     } else if (m_resizeDirection == TopRight) {
                         if (m_isSelected || m_isRotated) {
                            qApp->setOverrideCursor(Qt::SizeBDiagCursor);
                         } else {
                            qApp->setOverrideCursor(Qt::ClosedHandCursor);
                         }
                     } else if (m_resizeDirection == BottomRight) {
                         if (m_isSelected || m_isRotated) {
                            qApp->setOverrideCursor(Qt::SizeFDiagCursor);
                         } else {
                            qApp->setOverrideCursor(Qt::ClosedHandCursor);
                         }
                     } else if (m_resizeDirection == Rotate) {
                         qApp->setOverrideCursor(setCursorShape("rotate"));
                     } else if (m_resizeDirection == Moving) {
                         qApp->setOverrideCursor(Qt::ClosedHandCursor);
                     } else {
                         qApp->setOverrideCursor(setCursorShape("rect"));
                     }
                     update();
                     break;
                 } else {
                     qApp->setOverrideCursor(setCursorShape(m_currentType));
                     update();
                 }
            }
            if (!m_isHovered) {
                for(int j = 0; j < m_hoveredShape.mainPoints.length(); j++) {
                    m_hoveredShape.mainPoints[j] = QPointF(0, 0);
                }

                m_currentHoverDiagPoints.masterPoint = QPointF(0, 0);
                m_currentHoverDiagPoints.deputyPoint = QPointF(0, 0);
                update();
            }
            if (m_shapes.length() == 0) {
                qApp->setOverrideCursor(setCursorShape(m_currentType));
            }
        } else {
            //TODO text
        }
    }

    QFrame::mouseMoveEvent(e);
}

void ShapesWidget::paintImgPoint(QPainter &painter, QPointF pos, QPixmap img, bool isResize) {
        if (isResize) {
                painter.drawPixmap(QPoint(pos.x() - DRAG_BOUND_RADIUS,
                                  pos.y() - DRAG_BOUND_RADIUS), img);
        } else {
            painter.drawPixmap(QPoint(pos.x() - 12,
                              pos.y() - 12), img);
        }
}

void ShapesWidget::paintRect(QPainter &painter, FourPoints rectFPoints) {
    painter.drawLine(rectFPoints[0], rectFPoints[1]);
    painter.drawLine(rectFPoints[1], rectFPoints[3]);
    painter.drawLine(rectFPoints[3], rectFPoints[2]);
    painter.drawLine(rectFPoints[2], rectFPoints[0]);
}

void ShapesWidget::paintEllipse(QPainter &painter, FourPoints ellipseFPoints) {
    FourPoints minorPoints = getAnotherFPoints(ellipseFPoints);
    QList<QPointF> eightControlPoints = getEightControlPoint(ellipseFPoints);
    QPainterPath ellipsePath;
    ellipsePath.moveTo(minorPoints[0].x(), minorPoints[0].y());
    ellipsePath.cubicTo(eightControlPoints[0], eightControlPoints[1], minorPoints[1]);
    ellipsePath.cubicTo(eightControlPoints[4], eightControlPoints[5], minorPoints[2]);
    ellipsePath.cubicTo(eightControlPoints[6], eightControlPoints[7], minorPoints[3]);
    ellipsePath.cubicTo(eightControlPoints[3], eightControlPoints[2], minorPoints[0]);
    painter.drawPath(ellipsePath);
}

void ShapesWidget::paintLine(QPainter &painter, QList<QPointF> lineFPoints) {
    for (int k = 0; k < lineFPoints.length() - 2; k++) {
        painter.drawLine(lineFPoints[k], lineFPoints[k+1]);
    }
}

void ShapesWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);
    QPen pen;
    pen.setColor(m_penColor);
    pen.setWidth(3);
    painter.setPen(pen);

    if (m_currentDiagPoints.masterPoint != QPointF(0, 0) &&
            m_currentDiagPoints.deputyPoint != QPointF(0, 0)) {
        FourPoints currentFPoint = fourPointsOnRect(m_currentDiagPoints);

        if (m_currentType == "rectangle") {
            paintRect(painter, currentFPoint);
        } else if (m_currentType == "oval") {
            paintEllipse(painter, currentFPoint);
        } else if (m_currentType == "line") {
            paintLine(painter, m_currentShape.points);
        }
    }

    for(int i = 0; i < m_shapes.length(); i++) {
        if (m_shapes[i].type == "rectangle") {
            paintRect(painter, m_shapes[i].mainPoints);
        } else if (m_shapes[i].type == "oval") {
            paintEllipse(painter, m_shapes[i].mainPoints);
        } else if (m_currentType == "line") {
            paintLine(painter, m_shapes[i].points);
        }
    }

    if (m_selectedShape.mainPoints[0] != QPointF(0, 0)) {
        QPixmap resizePointImg(":/resources/images/size/resize_handle_big.png");
        for ( int i = 0; i < m_selectedShape.mainPoints.length(); i ++) {
            paintImgPoint(painter, m_selectedShape.mainPoints[i], resizePointImg);
        }

        FourPoints anotherFPoints = getAnotherFPoints(m_selectedShape.mainPoints);
        for (int j = 0; j < anotherFPoints.length(); j++) {
            paintImgPoint(painter, anotherFPoints[j], resizePointImg);
        }

        QPointF rotatePoint = getRotatePoint(m_selectedShape.mainPoints[0], m_selectedShape.mainPoints[1],
                m_selectedShape.mainPoints[2], m_selectedShape.mainPoints[3]);
        QPointF middlePoint((m_selectedShape.mainPoints[0].x() +  m_selectedShape.mainPoints[2].x())/2,
                           (m_selectedShape.mainPoints[0].y() +  m_selectedShape.mainPoints[2].y())/2);
        painter.setPen(QColor(Qt::white));
        painter.drawLine(rotatePoint, middlePoint);
        QPixmap rotatePointImg(":/resources/images/size/rotate.png");
        paintImgPoint(painter, rotatePoint, rotatePointImg, false);

        if (m_selectedShape.type == "oval" || m_selectedShape.type == "line") {
            paintRect(painter,  m_selectedShape.mainPoints);
        }
    }

    if (m_hoveredShape.mainPoints[0] != QPointF(0, 0)) {
        pen.setWidth(1);
        pen.setColor(QColor(Qt::white));
        painter.setPen(pen);
        if (m_hoveredShape.type == "rectangle") {
            paintRect(painter, m_hoveredShape.mainPoints);
        } else if (m_hoveredShape.type == "oval") {
            paintEllipse(painter, m_hoveredShape.mainPoints);
        } else if (m_hoveredShape.type == "line") {
            paintLine(painter, m_hoveredShape.points);
        }
    } else {
        qDebug() << "hoveredShape.type:" << m_hoveredShape.type;
    }
}

bool ShapesWidget::eventFilter(QObject *watched, QEvent *event) {
    Q_UNUSED(watched);

    if (event->type() == QEvent::Enter) {
        qApp->setOverrideCursor(setCursorShape(m_currentType));
    }

    if (event->type() == QKeyEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Escape) {
            qApp->quit();
        }
    }
    return false;

}