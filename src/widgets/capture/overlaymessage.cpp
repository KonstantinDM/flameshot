#include "overlaymessage.h"
#include "colorutils.h"
#include "confighandler.h"
#include "src/core/qguiappcurrentscreen.h"

#include <QApplication>
#include <QDebug>
#include <QLabel>
#include <QPainter>
#include <QPen>
#include <QScreen>

OverlayMessage::OverlayMessage(QWidget* parent, const QRect& targetArea)
  : QLabel(parent)
  , m_targetArea(targetArea)
{
    m_messageStack.push(QString()); // Default message is empty
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_AlwaysStackOnTop);
    setAlignment(Qt::AlignCenter);
    setTextFormat(Qt::RichText);

    m_fillColor = ConfigHandler().uiColor();
    int opacity = ConfigHandler().contrastOpacity();
    m_textColor =
      (ColorUtils::colorIsDark(m_fillColor) ? Qt::white : Qt::black);
    // map a background opacity range 0-255 to a fill opacity range 190-160
    // we do this because an opaque background makes the box look opaque too
    m_fillColor.setAlpha(160 + (180 - 220) / (255.0 - 0) * (opacity - 255));
    setStyleSheet(
      QStringLiteral("QLabel { color: %1; }").arg(m_textColor.name()));

    setMargin(QApplication::fontMetrics().height() / 2);
    QWidget::hide();
}

/**
 * @brief Push a message to the message stack.
 * @param msg Message text formatted as rich text
 */
void OverlayMessage::push(const QString& msg)
{
    m_messageStack.push(msg);
    setText(m_messageStack.top());
    setVisibility(true);
}

void OverlayMessage::pop()
{
    if (m_messageStack.size() > 1) {
        m_messageStack.pop();
    }

    setText(m_messageStack.top());
    setVisibility(m_messageStack.size() > 1);
}

void OverlayMessage::setVisibility(bool visible)
{
    updateGeometry();
    setVisible(visible);
}

void OverlayMessage::pushKeyMap(const QList<QPair<QString, QString>>& map)
{
    push(compileFromKeyMap(map));
}

/**
 * @brief Compile a message from a set of shortcuts and descriptions.
 * @param map List of (shortcut, description) pairs
 */
QString OverlayMessage::compileFromKeyMap(
  const QList<QPair<QString, QString>>& map)
{
    QString str = QStringLiteral("<table>");
    for (const auto& pair : map) {
        str += QStringLiteral("<tr>"
                              "<td align=\"right\"><b>%1 </b></td>"
                              "<td align=\"left\">&nbsp;&nbsp;%2</td>"
                              "</tr>")
                 .arg(pair.first)
                 .arg(pair.second);
    }
    str += QStringLiteral("</table>");
    return str;
}

void OverlayMessage::paintEvent(QPaintEvent* e)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setBrush(QBrush(m_fillColor, Qt::SolidPattern));
    painter.setPen(QPen(m_textColor, 1.5));
    float margin = painter.pen().widthF();
    painter.drawRoundedRect(
      rect() - QMarginsF(margin, margin, margin, margin), 5, 5);

    return QLabel::paintEvent(e);
}

QRect OverlayMessage::boundingRect() const
{
    QRect geom = QRect(QPoint(), sizeHint());
    geom.moveCenter(m_targetArea.center());
    return geom;
}

void OverlayMessage::updateGeometry()
{
    setGeometry(boundingRect());
    QLabel::updateGeometry();
}

