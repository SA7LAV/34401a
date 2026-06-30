#pragma once
#include <QWidget>
#include <QList>
#include "../models.h"

class QPushButton;
class QHBoxLayout;

class RangePanel : public QWidget {
    Q_OBJECT
public:
    explicit RangePanel(QWidget* parent = nullptr);
    void setMode(MeasMode mode);

signals:
    void rangeSelected(const QString& scpiCmd); // empty string = Auto

private:
    void onSelect(const QString& cmd, QPushButton* btn);

    QHBoxLayout* m_layout{nullptr};
    QList<QPushButton*> m_buttons;
    QPushButton* m_activeBtn{nullptr};
};
