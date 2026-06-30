#pragma once
#include <QObject>
#include <QString>
#include <QMap>

class Translations : public QObject {
    Q_OBJECT
public:
    static Translations& instance();

    QString tr(const QString& key) const;
    // tr() with named placeholders: tr("status_error", {{"msg", "some error"}})
    QString tr(const QString& key, const QMap<QString, QString>& args) const;

    void setLanguage(const QString& lang);
    QString currentLanguage() const { return m_lang; }

signals:
    void languageChanged(const QString& lang);

private:
    explicit Translations(QObject* parent = nullptr);
    QString m_lang{"de"};
};

// Free convenience function — named tl() to avoid collision with QObject::tr()
inline QString tl(const QString& key)
{
    return Translations::instance().tr(key);
}
