#pragma once
#include <QWidget>
#include <QMap>
#include "../models.h"

class QPushButton;

class ModePanel : public QWidget {
    Q_OBJECT
public:
    explicit ModePanel(QWidget* parent = nullptr);
    void select(MeasMode mode);

signals:
    void modeChanged(MeasMode mode);

private:
    void onButtonClicked(MeasMode mode);

    MeasMode m_active{MeasMode::VDC};
    QMap<MeasMode, QPushButton*> m_buttons;
};
