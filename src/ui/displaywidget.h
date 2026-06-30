#pragma once
#include <QWidget>
#include <QLabel>
#include <QBoxLayout>
#include <QString>
#include <optional>

class DisplayWidget : public QWidget {
    Q_OBJECT
public:
    explicit DisplayWidget(QWidget* parent = nullptr);

    void setUnit(const QString& unit);
    void resetStats();
    void applyDisplayConfig();

public slots:
    void updateValue(double rawValue);

private:
    void buildUi();
    QLabel* statBlock(const QString& title, QBoxLayout* layout);

    QLabel* m_valLabel{nullptr};
    QLabel* m_prefixLabel{nullptr};
    QLabel* m_unitLabel{nullptr};
    QWidget* m_statsFrame{nullptr};
    QLabel* m_minLabel{nullptr};
    QLabel* m_maxLabel{nullptr};
    QLabel* m_avgLabel{nullptr};

    QString m_unit{"VDC"};
    std::optional<double> m_min;
    std::optional<double> m_max;
    double m_sum{0.0};
    int    m_count{0};
};
