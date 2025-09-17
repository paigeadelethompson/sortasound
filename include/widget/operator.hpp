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

#pragma once

#include <QWidget>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QColor>
#include <QFont>
#include <QFontMetrics>
#include <QSize>
#include <QPoint>
#include <QVector>
#include <QMap>
#include <QString>
#include <array>
#include <vector>

#include "../theme/theme.hpp"

/**
 * @brief Widget for displaying FM synthesis operator graphs
 * 
 * Displays the 32 different FM synthesis algorithms as visual graphs
 * showing operator connections, modulation paths, and feedback loops.
 * Each algorithm is rendered exactly as shown in the reference image.
 */
class OperatorGraphWidget : public QWidget
{
    Q_OBJECT

public:
    explicit OperatorGraphWidget(QWidget *parent = nullptr);
    ~OperatorGraphWidget();

    /**
     * @brief Set the current algorithm to display
     * @param algorithm Algorithm number (0-31)
     */
    void setAlgorithm(int algorithm);

    /**
     * @brief Get the current algorithm being displayed
     * @return Current algorithm number (0-31)
     */
    int getCurrentAlgorithm() const { return currentAlgorithm_; }

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    /**
     * @brief Structure representing an operator position and properties
     */
    struct OperatorInfo {
        int number;           // Operator number (1-6)
        QPoint position;      // Position in the grid
        bool isCarrier;       // True if this operator is a carrier (output)
        bool isModulator;     // True if this operator is a modulator
        bool hasFeedback;     // True if this operator has feedback
        QColor fillColor;     // Fill color for the operator
        QColor textColor;     // Text color for the operator
    };

    /**
     * @brief Structure representing a connection between operators
     */
    struct Connection {
        int fromOp;           // Source operator number (1-6)
        int toOp;             // Destination operator number (1-6)
        QPoint fromPos;       // Source position
        QPoint toPos;         // Destination position
        bool isFeedback;      // True if this is a feedback connection
        bool isHorizontal;    // True if connection is horizontal
        bool isVertical;      // True if connection is vertical
        bool isDiagonal;      // True if connection is diagonal
    };

    /**
     * @brief Structure representing a complete algorithm layout
     */
    struct AlgorithmLayout {
        int algorithmNumber;
        int rows;             // Number of rows (1-4)
        int cols;             // Number of columns (1-6)
        std::array<OperatorInfo, 6> operators;
        std::vector<Connection> connections;
        QSize gridSize;       // Size of the operator grid
    };

    /**
     * @brief Initialize all 32 algorithm layouts
     */
    void initializeAlgorithms();

    /**
     * @brief Create a specific algorithm layout
     * @param algorithm Algorithm number (0-31)
     * @return AlgorithmLayout structure
     */
    AlgorithmLayout createAlgorithmLayout(int algorithm);

    /**
     * @brief Calculate operator positions based on grid layout
     * @param layout Reference to algorithm layout
     */
    void calculatePositions(AlgorithmLayout& layout);

    /**
     * @brief Draw an operator at the specified position
     * @param painter QPainter object
     * @param op Operator information
     * @param size Size of the operator square
     */
    void drawOperator(QPainter& painter, const OperatorInfo& op, int size);

    /**
     * @brief Draw a connection between operators
     * @param painter QPainter object
     * @param conn Connection information
     * @param opSize Size of operators (for connection endpoints)
     */
    void drawConnection(QPainter& painter, const Connection& conn, int opSize);

    /**
     * @brief Draw a feedback loop
     * @param painter QPainter object
     * @param conn Feedback connection
     * @param opSize Size of operators
     */
    void drawFeedbackLoop(QPainter& painter, const Connection& conn, int opSize);

    /**
     * @brief Calculate the optimal size for the graph
     * @return QSize representing the optimal widget size
     */
    QSize calculateOptimalSize() const;

    /**
     * @brief Get theme colors for operators
     * @param isCarrier True if operator is a carrier
     * @param isModulator True if operator is a modulator
     * @return QColor for operator fill
     */
    QColor getOperatorColor(bool isCarrier, bool isModulator) const;

    int currentAlgorithm_;
    std::array<AlgorithmLayout, 32> algorithms_;
    QSize operatorSize_;
    QSize gridSpacing_;
    QPen connectionPen_;
    QPen feedbackPen_;
    QFont operatorFont_;
    ThemeManager& themeManager_;
    
    // Constants
    static constexpr int MIN_OPERATOR_SIZE = 20;
    static constexpr int MAX_OPERATOR_SIZE = 40;
    static constexpr int GRID_PADDING = 10;
    static constexpr int CONNECTION_LINE_WIDTH = 2;
    static constexpr int FEEDBACK_LINE_WIDTH = 2;
};
