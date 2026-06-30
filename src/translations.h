/**
 * @file translations.h
 * @brief Runtime UI string lookup with live language switching.
 *
 * All UI strings are stored in a compile-time table inside translations.cpp
 * (no external resource files).  The supported language codes are:
 *   - @c "de"  German (default / fallback)
 *   - @c "en"  English
 *   - @c "sv"  Swedish
 *
 * Lookup strategy: language → key → German fallback → "[key]" sentinel.
 * If the requested key is missing in the active language the German entry is
 * tried; if that is also absent the key name enclosed in brackets is returned.
 *
 * Language changes are broadcast via the languageChanged() signal so that all
 * widgets can retranslate themselves without a restart.
 */
#pragma once
#include <QObject>
#include <QString>
#include <QMap>

/**
 * @brief Singleton that provides localised string lookup for the entire application.
 *
 * Obtain the instance via Translations::instance().  In practice, prefer the
 * free convenience function tl() defined at the bottom of this header.
 */
class Translations : public QObject {
    Q_OBJECT
public:
    /** @brief Returns the application-wide Translations singleton. */
    static Translations& instance();

    /**
     * @brief Returns the localised string for @p key in the current language.
     *
     * Falls back to German and then to "[key]" if the string is absent.
     */
    QString tr(const QString& key) const;

    /**
     * @brief Returns the localised string with named placeholder substitution.
     *
     * Placeholders in the template string have the form @c {name}.
     * Example: @code tr("status_error", {{"msg", "timeout"}}) @endcode
     */
    QString tr(const QString& key, const QMap<QString, QString>& args) const;

    /**
     * @brief Switches the active language to @p lang.
     *
     * @param lang  One of "de", "en", or "sv".  Unknown codes are silently
     *              ignored and the current language is retained.
     *
     * Emits languageChanged() on success, which triggers UI retranslation in
     * all connected widgets.
     */
    void setLanguage(const QString& lang);

    /** @brief Returns the currently active language code. */
    QString currentLanguage() const { return m_lang; }

signals:
    /** @brief Emitted after a successful setLanguage() call. */
    void languageChanged(const QString& lang);

private:
    explicit Translations(QObject* parent = nullptr);
    QString m_lang{"de"}; ///< Default language; must match a key in STRINGS
};

/**
 * @brief Convenience wrapper for Translations::instance().tr(key).
 *
 * Named @c tl() (not @c tr()) to avoid shadowing QObject::tr() inside
 * QObject subclasses.
 */
inline QString tl(const QString& key)
{
    return Translations::instance().tr(key);
}
