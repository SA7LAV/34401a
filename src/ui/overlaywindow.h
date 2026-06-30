#pragma once
#include <QWidget>
#include <QPoint>

class QLabel;
class QTimer;

class OverlayWindow : public QWidget {
    Q_OBJECT
public:
    explicit OverlayWindow(QWidget* parent = nullptr);

    void setUnit(const QString& unit);
    void applyStyle();

public slots:
    void updateValue(double rawValue);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void moveEvent(QMoveEvent* event) override;

private:
    void restorePos();
    void savePos();

    QLabel* m_valLabel{nullptr};
    QLabel* m_prefixLabel{nullptr};
    QLabel* m_unitLabel{nullptr};
    QTimer* m_saveTimer{nullptr};
    QPoint  m_dragPos;
    QString m_unit{"VDC"};
};
