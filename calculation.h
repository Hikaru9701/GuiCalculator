#ifndef CALCULATION_H
#define CALCULATION_H

#include <QObject>
#include <QDebug>
#include <QStack>
#include <QString>
#include <QStringList>
#include <QVariant>

class Calculation : public QObject
{
	Q_OBJECT
public:
	explicit Calculation(QObject* parent = nullptr);

public slots:
	void formulaCalculator(const QString& formula);

signals:
	void sendAnswer(const QString& ans);

private:
	QStringList infixToPostfix(const QStringList& infix);
	QStringList devideFormula(const QString& formula);
	double postfixCalculator(const QStringList& express);
};



#endif // CALCULATION_H
