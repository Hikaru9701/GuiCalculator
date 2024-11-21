#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "calculation.h"
#include <QtWidgets>

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget* parent = nullptr);
	~MainWindow();

signals:
	void sendFormula(const QString& formula);

protected:
//	void keyPressEvent(QKeyEvent *event);
	bool eventFilter(QObject* obj, QEvent* event);

private slots:

	void onInputButtonClicked();
	void on_btnFuncClear_clicked();
	void on_btnFuncEqual_clicked();
	void on_btnFuncAns_clicked();
	void on_btnFuncBack_clicked();
	void on_btnFuncDelete_clicked();
	void on_cmbHistorySelect_currentIndexChanged(int index);
	void on_btnFuncCopy_clicked();
	void changeTheme(QAction* action);
	void closeEvent(QCloseEvent* event);
	void updateLabel();
	void receiveAnswer(const QString& ans);
	void about();
	void exit();
	void aboutQt();
//	void animate_backspace();//(QLabelに変更するので廃棄)
	

private:
	Ui::MainWindow* ui;
	Calculation* calculation;
	QTimer* timer;
	QShortcut* shortcut;
	QMovie* movieShiba;
	QActionGroup* themeGroup;

};
#endif // MAINWINDOW_H
