#pragma once

#include <QLabel>
#include <QStack>

/**
 * @brief Overlay a message in capture mode.
 *
 * To change the active message call `push`. This will automatically show the
 * widget. Previous messages won't be forgotten and will be reactivated after
 * you call `pop`. You can show/hide the message using `setVisibility` (this
 * won't push/pop anything).
 *
 * @note You have to make sure that widgets pop the messages they pushed when
 * they are closed, to avoid potential bugs and resource leaks.
 */
class OverlayMessage : public QLabel
{
public:
    OverlayMessage(QWidget* parent, const QRect& center);
    void push(const QString& msg);
    void pop();
    void setVisibility(bool visible);
    void showMessage(const QString& msg) {
        push(msg);
    }
    void hideMessage() {
        pop();
    }

    void pushKeyMap(const QList<QPair<QString, QString>>& map);
    static QString compileFromKeyMap(const QList<QPair<QString, QString>>& map);

private:
    QStack<QString> m_messageStack;
    QRect m_targetArea;
    QColor m_fillColor, m_textColor;

    void paintEvent(QPaintEvent*) override;

    QRect boundingRect() const;
    void updateGeometry();
};
