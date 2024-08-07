#ifndef DELEGATEHORA_H
#define DELEGATEHORA_H
#include <QStyledItemDelegate>
#include <QDateTime>
#include <QLocale>


class DelegateHora : public QStyledItemDelegate
{
public:
    DelegateHora(QObject *parent = nullptr);
    QString displayText(const QVariant &value, const QLocale &locale) const override;

};

#endif // DELEGATEHORA_H
