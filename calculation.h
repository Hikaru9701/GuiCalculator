#ifndef CALCULATION_H
#define CALCULATION_H

#include <QObject>
#include <QDebug>
#include <QStack>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <windows.h>

class Calculation : public QObject
{
	Q_OBJECT
public:
	explicit Calculation(QObject* parent = nullptr);

private slots:
	HRESULT formulaCalculator(const QString& formula);

signals:
	void sendAnswer(const QString& ans);

private:
	HRESULT devideFormula(const QString& formula, QStringList& formulaList);
	HRESULT infixToPostfix(const QStringList& infix, QStringList& postfix);
	HRESULT postfixCalculator(const QStringList& postfix, double& result);
};



#endif // CALCULATION_H
