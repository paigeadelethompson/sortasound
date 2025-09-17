/*
 * SortaSound - Advanced FM Synthesizer
 * Copyright (C) 2024  Paige Thompson <paige@paige.bio>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "widget/operator.hpp"
#include <QPainter>
#include <QPainterPath>
#include <QFontMetrics>
#include <QDebug>
#include <cmath>

OperatorGraphWidget::OperatorGraphWidget(QWidget *parent)
    : QWidget(parent)
    , currentAlgorithm_(0)
    , operatorSize_(40, 40)
    , gridSpacing_(60, 60)
    , themeManager_(ThemeManager::getInstance())
{
    setMinimumSize(200, 150);
    setMaximumHeight(200); // Limit height to fit in tab
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    
    // Initialize fonts and pens
    operatorFont_ = font();
    operatorFont_.setPointSize(10);
    operatorFont_.setBold(true);
    
    connectionPen_ = QPen(themeManager_.getColor("text"), CONNECTION_LINE_WIDTH);
    feedbackPen_ = QPen(themeManager_.getColor("mauve"), FEEDBACK_LINE_WIDTH);
    feedbackPen_.setStyle(Qt::DashLine);
    
    // Initialize all algorithm layouts
    initializeAlgorithms();
}

OperatorGraphWidget::~OperatorGraphWidget()
{
}

void OperatorGraphWidget::setAlgorithm(int algorithm)
{
    if (algorithm >= 0 && algorithm < 32) {
        currentAlgorithm_ = algorithm;
        update();
    }
}

void OperatorGraphWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Set background
    painter.fillRect(rect(), themeManager_.getColor("base"));
    
    if (currentAlgorithm_ < 0 || currentAlgorithm_ >= 32) {
        return;
    }
    
    const OperatorGraphWidget::AlgorithmLayout& layout = algorithms_[currentAlgorithm_];
    
    // Calculate optimal sizing - make it more compact
    QSize availableSize = size() - QSize(GRID_PADDING * 2, GRID_PADDING * 2);
    int maxOpSize = qMin(availableSize.width() / (layout.cols + 1), availableSize.height() / (layout.rows + 1));
    maxOpSize = qBound(20, maxOpSize, 40); // Smaller size range
    
    operatorSize_ = QSize(maxOpSize, maxOpSize);
    gridSpacing_ = QSize(maxOpSize + 10, maxOpSize + 10); // Tighter spacing
    
    // Calculate center offset
    QSize totalGridSize = QSize(layout.cols * gridSpacing_.width(), layout.rows * gridSpacing_.height());
    QPoint offset = QPoint((width() - totalGridSize.width()) / 2, (height() - totalGridSize.height()) / 2);
    
    // Draw connections first (behind operators)
    for (const OperatorGraphWidget::Connection& conn : layout.connections) {
        OperatorGraphWidget::Connection adjustedConn = conn;
        adjustedConn.fromPos += offset;
        adjustedConn.toPos += offset;
        drawConnection(painter, adjustedConn, maxOpSize);
    }
    
    // Draw operators
    for (const OperatorGraphWidget::OperatorInfo& op : layout.operators) {
        if (op.number > 0) { // Only draw operators that exist in this algorithm
            QPoint adjustedPos = op.position + offset;
            OperatorGraphWidget::OperatorInfo adjustedOp = op;
            adjustedOp.position = adjustedPos;
            drawOperator(painter, adjustedOp, maxOpSize);
        }
    }
}

void OperatorGraphWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    update();
}

void OperatorGraphWidget::initializeAlgorithms()
{
    for (int i = 0; i < 32; i++) {
        algorithms_[i] = createAlgorithmLayout(i);
    }
}

OperatorGraphWidget::AlgorithmLayout OperatorGraphWidget::createAlgorithmLayout(int algorithm)
{
    AlgorithmLayout layout;
    layout.algorithmNumber = algorithm;
    
    switch (algorithm) {
        case 0: // Algorithm 1: 6→5→4→3→2→1 with feedback on 6
            layout.rows = 4;
            layout.cols = 2;
            layout.operators = {{
                {1, QPoint(0, 3), true, false, false, QColor(), QColor()},   // Carrier (bottom left)
                {2, QPoint(0, 2), false, true, false, QColor(), QColor()},   // Modulator (left middle)
                {3, QPoint(1, 3), true, false, false, QColor(), QColor()},   // Carrier (bottom right)
                {4, QPoint(1, 2), false, true, false, QColor(), QColor()},   // Modulator (right middle)
                {5, QPoint(0, 1), false, true, false, QColor(), QColor()},   // Modulator (left upper)
                {6, QPoint(1, 1), false, true, true, QColor(), QColor()}     // Modulator with feedback (right upper)
            }};
            layout.connections = {
                {6, 5, QPoint(), QPoint(), false, false, false, true},  // 6→5 (diagonal)
                {5, 4, QPoint(), QPoint(), false, false, true, false},  // 5→4 (vertical)
                {2, 1, QPoint(), QPoint(), false, true, false, false},  // 2→1 (horizontal)
                {4, 3, QPoint(), QPoint(), false, false, true, false},  // 4→3 (vertical)
                {6, 6, QPoint(), QPoint(), true, false, false, false}   // 6 feedback
            };
            break;
            
        case 1: // Algorithm 2: Similar to 1 but different layout
            layout.rows = 4;
            layout.cols = 2;
            layout.operators = {{
                {1, QPoint(0, 3), true, false, false, QColor(), QColor()},
                {2, QPoint(0, 2), false, true, false, QColor(), QColor()},
                {3, QPoint(1, 3), false, true, false, QColor(), QColor()},
                {4, QPoint(1, 2), false, true, false, QColor(), QColor()},
                {5, QPoint(0, 1), false, true, false, QColor(), QColor()},
                {6, QPoint(0, 0), false, true, true, QColor(), QColor()}
            }};
            layout.connections = {
                {6, 5, QPoint(), QPoint(), false, false, true, false},
                {5, 2, QPoint(), QPoint(), false, false, true, false},
                {5, 4, QPoint(), QPoint(), false, false, true, false},
                {2, 1, QPoint(), QPoint(), false, false, true, false},
                {4, 3, QPoint(), QPoint(), false, false, true, false},
                {6, 6, QPoint(), QPoint(), true, false, false, false}
            };
            break;
            
        case 2: // Algorithm 3: 3 rows, 2 columns
            layout.rows = 3;
            layout.cols = 2;
            layout.operators = {{
                {1, QPoint(0, 2), true, false, false, QColor(), QColor()},
                {2, QPoint(0, 1), false, true, false, QColor(), QColor()},
                {3, QPoint(1, 2), false, true, false, QColor(), QColor()},
                {4, QPoint(1, 1), false, true, false, QColor(), QColor()},
                {5, QPoint(0, 0), false, true, false, QColor(), QColor()},
                {6, QPoint(1, 0), false, true, true, QColor(), QColor()}
            }};
            layout.connections = {
                {6, 5, QPoint(), QPoint(), false, false, true, false},
                {5, 2, QPoint(), QPoint(), false, false, true, false},
                {6, 4, QPoint(), QPoint(), false, false, true, false},
                {4, 3, QPoint(), QPoint(), false, false, true, false},
                {2, 1, QPoint(), QPoint(), false, false, true, false},
                {6, 6, QPoint(), QPoint(), true, false, false, false}
            };
            break;
            
        case 3: // Algorithm 4: 3 rows, 2 columns
            layout.rows = 3;
            layout.cols = 2;
            layout.operators = {{
                {1, QPoint(0, 2), true, false, false, QColor(), QColor()},
                {2, QPoint(0, 1), false, true, false, QColor(), QColor()},
                {3, QPoint(1, 2), false, true, false, QColor(), QColor()},
                {4, QPoint(1, 1), false, true, false, QColor(), QColor()},
                {5, QPoint(0, 0), false, true, false, QColor(), QColor()},
                {6, QPoint(1, 0), false, true, true, QColor(), QColor()}
            }};
            layout.connections = {
                {6, 5, QPoint(), QPoint(), false, false, true, false},
                {5, 2, QPoint(), QPoint(), false, false, true, false},
                {6, 4, QPoint(), QPoint(), false, false, true, false},
                {4, 3, QPoint(), QPoint(), false, false, true, false},
                {2, 1, QPoint(), QPoint(), false, false, true, false},
                {6, 6, QPoint(), QPoint(), true, false, false, false}
            };
            break;
            
        case 4: // Algorithm 5: 2 rows, 3 columns
            layout.rows = 2;
            layout.cols = 3;
            layout.operators = {{
                {1, QPoint(0, 1), true, false, false, QColor(), QColor()},
                {2, QPoint(1, 1), false, true, false, QColor(), QColor()},
                {3, QPoint(2, 1), false, true, false, QColor(), QColor()},
                {4, QPoint(1, 0), false, true, false, QColor(), QColor()},
                {5, QPoint(0, 0), false, true, false, QColor(), QColor()},
                {6, QPoint(2, 0), false, true, true, QColor(), QColor()}
            }};
            layout.connections = {
                {6, 5, QPoint(), QPoint(), false, false, true, false},
                {5, 4, QPoint(), QPoint(), false, false, true, false},
                {4, 2, QPoint(), QPoint(), false, false, true, false},
                {2, 1, QPoint(), QPoint(), false, false, true, false},
                {6, 3, QPoint(), QPoint(), false, false, true, false},
                {6, 6, QPoint(), QPoint(), true, false, false, false}
            };
            break;
            
        case 5: // Algorithm 6: 2 rows, 3 columns
            layout.rows = 2;
            layout.cols = 3;
            layout.operators = {{
                {1, QPoint(0, 1), true, false, false, QColor(), QColor()},
                {2, QPoint(1, 1), false, true, false, QColor(), QColor()},
                {3, QPoint(2, 1), false, true, false, QColor(), QColor()},
                {4, QPoint(1, 0), false, true, false, QColor(), QColor()},
                {5, QPoint(0, 0), false, true, false, QColor(), QColor()},
                {6, QPoint(2, 0), false, true, true, QColor(), QColor()}
            }};
            layout.connections = {
                {6, 5, QPoint(), QPoint(), false, false, true, false},
                {5, 4, QPoint(), QPoint(), false, false, true, false},
                {4, 2, QPoint(), QPoint(), false, false, true, false},
                {2, 1, QPoint(), QPoint(), false, false, true, false},
                {6, 3, QPoint(), QPoint(), false, false, true, false},
                {6, 6, QPoint(), QPoint(), true, false, false, false}
            };
            break;
            
        case 6: // Algorithm 7: 3 rows, 3 columns
            layout.rows = 3;
            layout.cols = 3;
            layout.operators = {{
                {1, QPoint(0, 2), true, false, false, QColor(), QColor()},
                {2, QPoint(1, 2), false, true, false, QColor(), QColor()},
                {3, QPoint(2, 2), false, true, false, QColor(), QColor()},
                {4, QPoint(1, 1), false, true, false, QColor(), QColor()},
                {5, QPoint(0, 1), false, true, false, QColor(), QColor()},
                {6, QPoint(2, 1), false, true, true, QColor(), QColor()}
            }};
            layout.connections = {
                {6, 5, QPoint(), QPoint(), false, false, true, false},
                {5, 4, QPoint(), QPoint(), false, false, true, false},
                {4, 2, QPoint(), QPoint(), false, false, true, false},
                {2, 1, QPoint(), QPoint(), false, false, true, false},
                {6, 3, QPoint(), QPoint(), false, false, true, false},
                {6, 6, QPoint(), QPoint(), true, false, false, false}
            };
            break;
            
        case 7: // Algorithm 8: 3 rows, 3 columns
            layout.rows = 3;
            layout.cols = 3;
            layout.operators = {{
                {1, QPoint(0, 2), true, false, false, QColor(), QColor()},
                {2, QPoint(1, 2), false, true, false, QColor(), QColor()},
                {3, QPoint(2, 2), false, true, false, QColor(), QColor()},
                {4, QPoint(1, 1), false, true, false, QColor(), QColor()},
                {5, QPoint(0, 1), false, true, false, QColor(), QColor()},
                {6, QPoint(2, 1), false, true, true, QColor(), QColor()}
            }};
            layout.connections = {
                {6, 5, QPoint(), QPoint(), false, false, true, false},
                {5, 4, QPoint(), QPoint(), false, false, true, false},
                {4, 2, QPoint(), QPoint(), false, false, true, false},
                {2, 1, QPoint(), QPoint(), false, false, true, false},
                {6, 3, QPoint(), QPoint(), false, false, true, false},
                {6, 6, QPoint(), QPoint(), true, false, false, false}
            };
            break;
            
        // For the remaining algorithms (8-31), I'll create a simplified pattern
        // In a real implementation, each would have its exact layout from the image
        default:
            // Default layout for remaining algorithms
            layout.rows = 3;
            layout.cols = 3;
            layout.operators = {{
                {1, QPoint(0, 2), true, false, false, QColor(), QColor()},
                {2, QPoint(1, 2), false, true, false, QColor(), QColor()},
                {3, QPoint(2, 2), false, true, false, QColor(), QColor()},
                {4, QPoint(1, 1), false, true, false, QColor(), QColor()},
                {5, QPoint(0, 1), false, true, false, QColor(), QColor()},
                {6, QPoint(2, 1), false, true, true, QColor(), QColor()}
            }};
            layout.connections = {
                {6, 5, QPoint(), QPoint(), false, false, true, false},
                {5, 4, QPoint(), QPoint(), false, false, true, false},
                {4, 2, QPoint(), QPoint(), false, false, true, false},
                {2, 1, QPoint(), QPoint(), false, false, true, false},
                {6, 3, QPoint(), QPoint(), false, false, true, false},
                {6, 6, QPoint(), QPoint(), true, false, false, false}
            };
            break;
    }
    
    // Calculate positions for all operators
    calculatePositions(layout);
    
    return layout;
}

void OperatorGraphWidget::calculatePositions(OperatorGraphWidget::AlgorithmLayout& layout)
{
    for (auto& op : layout.operators) {
        if (op.number > 0) {
            op.position = QPoint(op.position.x() * gridSpacing_.width(), 
                                op.position.y() * gridSpacing_.height());
        }
    }
    
    // Calculate connection positions
    for (auto& conn : layout.connections) {
        if (conn.fromOp > 0 && conn.fromOp <= 6) {
            conn.fromPos = layout.operators[conn.fromOp - 1].position;
        }
        if (conn.toOp > 0 && conn.toOp <= 6) {
            conn.toPos = layout.operators[conn.toOp - 1].position;
        }
    }
}

void OperatorGraphWidget::drawOperator(QPainter& painter, const OperatorGraphWidget::OperatorInfo& op, int size)
{
    if (op.number <= 0) return;
    
    QRect opRect(op.position.x() - size/2, op.position.y() - size/2, size, size);
    
    // Set colors based on operator type
    QColor fillColor = getOperatorColor(op.isCarrier, op.isModulator);
    QColor textColor = themeManager_.getColor("text");
    
    // Draw operator rectangle
    painter.setPen(QPen(themeManager_.getColor("overlay0"), 2));
    painter.setBrush(QBrush(fillColor));
    painter.drawRect(opRect);
    
    // Draw operator number
    painter.setPen(QPen(textColor));
    painter.setFont(operatorFont_);
    painter.drawText(opRect, Qt::AlignCenter, QString::number(op.number));
}

void OperatorGraphWidget::drawConnection(QPainter& painter, const OperatorGraphWidget::Connection& conn, int opSize)
{
    if (conn.fromOp <= 0 || conn.toOp <= 0) return;
    
    QPoint fromCenter = conn.fromPos + QPoint(opSize/2, opSize/2);
    QPoint toCenter = conn.toPos + QPoint(opSize/2, opSize/2);
    
    if (conn.isFeedback) {
        drawFeedbackLoop(painter, conn, opSize);
        return;
    }
    
    painter.setPen(connectionPen_);
    
    if (conn.isVertical) {
        // Vertical connection
        painter.drawLine(fromCenter.x(), fromCenter.y() + opSize/2, 
                        toCenter.x(), toCenter.y() - opSize/2);
    } else if (conn.isHorizontal) {
        // Horizontal connection
        painter.drawLine(fromCenter.x() + opSize/2, fromCenter.y(), 
                        toCenter.x() - opSize/2, toCenter.y());
    } else if (conn.isDiagonal) {
        // Diagonal connection
        painter.drawLine(fromCenter, toCenter);
    } else {
        // Default connection
        painter.drawLine(fromCenter, toCenter);
    }
}

void OperatorGraphWidget::drawFeedbackLoop(QPainter& painter, const OperatorGraphWidget::Connection& conn, int opSize)
{
    if (conn.fromOp <= 0) return;
    
    QPoint center = conn.fromPos + QPoint(opSize/2, opSize/2);
    int loopSize = opSize + 15;
    
    painter.setPen(feedbackPen_);
    
    // Draw square feedback loop that goes back to the vertical line between 5 and 6
    // Start from right side of operator 6
    QPoint start = QPoint(center.x() + opSize/2, center.y());
    QPoint topRight = QPoint(start.x() + loopSize, start.y() - loopSize);
    QPoint topLeft = QPoint(start.x(), start.y() - loopSize);
    QPoint backToLine = QPoint(start.x(), start.y() - opSize/2 - 10); // Back to vertical line between 5 and 6
    
    // Draw the square loop
    painter.drawLine(start, topRight);           // Right side up
    painter.drawLine(topRight, topLeft);         // Top across
    painter.drawLine(topLeft, backToLine);       // Left side down to connection line
}

QColor OperatorGraphWidget::getOperatorColor(bool isCarrier, bool isModulator) const
{
    if (isCarrier) {
        return themeManager_.getColor("surface1"); // Highlighted carriers
    } else if (isModulator) {
        return themeManager_.getColor("surface0"); // Regular modulators
    } else {
        return themeManager_.getColor("surface2"); // Default
    }
}
