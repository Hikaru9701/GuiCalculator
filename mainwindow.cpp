#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "calculation.h"

// グローバル変数定義
bool gIsPointClicked = false;									// 小数点チェック変数
bool gIsCalculated = false;										// 計算記録変数
int gLeftBracketCounter = 0;									// 左括弧カウンター変数
int gRightBracketCounter = 0;									// 右括弧カウンター変数
int gHistoryCounter = 1;										// 履歴数カウンター変数
extern QString gAnsString;										// 計算結果文字列
QString gDisplayText = "";										// 表示内容文字列
QString gCalculationFormula = "";								// 計算数式文字列
QString gHistoryText = "";										// 計算履歴文字列
QList<QString> gHistoryTextList;								// 表示内容履歴配列
QList<QString> gHistoryFormulaList;								// 数式履歴配列

const QString VERSION = "1.5.2 beta";

//----------------------------------------------------------------------------
//------------------------------コンストラクタ--------------------------------
//----------------------------------------------------------------------------

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , calculation(std::make_unique<Calculation>(this))
    , timer(std::make_unique<QTimer>(this))
    , themeGroup(std::make_unique<QActionGroup>(this))
{
	setupWindowSizeAndPosition();
	initializeUIComponents();
	connectSignalsAndSlots();

	// EventFilterをインストール
	installEventFilter(this);

	// 1秒1回、updateLabelを呼び出す
	// timer = new QTimer(this);
	timer->start(1000);

	// Backspaceを起動するため、QShortcutを作る(QLabelに変更するので廃棄)
	// QShortcut* shortcut = new QShortcut(QKeySequence("Backspace"), this);
	// connect(shortcut, &QShortcut::activated, this, &MainWindow::animate_backspace);
}

//----------------------------------------------------------------------------
//------------------------------デストラクタ----------------------------------
//----------------------------------------------------------------------------

MainWindow::~MainWindow()
{
	// qDebug() << "Destructor called";
	
	// delete UI
	if (ui) {
		// qDebug() << "Deleting UI";
		delete ui;
		ui = nullptr;
	}
	
	// delete QList
	gHistoryTextList.clear();
	gHistoryFormulaList.clear();

	// qDebug() << "Destructor finished";
}


//----------------------------------------------------------------------------
//------------------------------機能実装関数----------------------------------
//----------------------------------------------------------------------------

// Setup the window size and position
void MainWindow::setupWindowSizeAndPosition() {
	this->setMinimumSize(500, 420);
	this->setMaximumSize(550, 500);
	
	// 前回情報の読み込み
	QString settingPath = QCoreApplication::applicationDirPath() + "/config.ini";
	QSettings setting(settingPath, QSettings::IniFormat);	// ファイルの特定
	QPoint posMainWindow = setting.value("pos").toPoint();		// 場所情報を獲得
	int widthMainWindow = setting.value("width").toInt();		// 幅情報を獲得
	int heightMainWindow = setting.value("height").toInt();		// 高さ情報を獲得

	// もしposは(0,0)なら，場所が設定されていないと判断する
	if (posMainWindow == QPoint(0, 0)) {
		this->move(200, 200);									// デフォルトの場所に移動
	} else {
		this->move(posMainWindow);								// 前回の場所へ移動
	}

	// もし幅、高さが設定されていない
	if (widthMainWindow == 0 && heightMainWindow == 0) {
		this->resize(500, 500);									// デフォルトの大きさに設定
	} else {
		this->resize(widthMainWindow, heightMainWindow);		// 前回の大きさに設定
	}
}

// Initialize UI Components
void MainWindow::initializeUIComponents() {
	ui->setupUi(this);
	QString title = QString("ShibaCalculator - v%1").arg(VERSION);
	this->setWindowTitle(title);

	QString styleSheet = "border-radius: 10px; border:1px solid rgb(211,211,211)";
	ui->frmDisplay->setStyleSheet(styleSheet);

	// 動画を再生する
	QString moviePath = QCoreApplication::applicationDirPath() + "/shiba.gif";
	movieShiba = std::make_unique<QMovie>(moviePath, QByteArray(), this);
	ui->lblMovie->setMovie(movieShiba.get());
	movieShiba->start();

	// labelDisplay垂直位置を中央に設定、水平位置は右
	ui->lblDisplay->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
	

	// TextEditを設置
	ui->txedtHistory->setReadOnly(true);
	ui->txedtHistory->setAlignment(Qt::AlignLeft);
	ui->txedtHistory->setStyleSheet("QTextEdit {"
									"border-radius: 10px;"
									"}");
	
	// テーマのグループ化
	themeGroup->addAction(ui->actionManjaro);
	themeGroup->addAction(ui->actionAMOLED);
	themeGroup->addAction(ui->actionAqua);
	themeGroup->addAction(ui->actionConsole);
	themeGroup->addAction(ui->actionMacOS);
	themeGroup->addAction(ui->actionUbuntu);
	themeGroup->addAction(ui->actionDiplaytap);

	// チェックできるように設定
	ui->actionManjaro->setCheckable(true);
	ui->actionAMOLED->setCheckable(true);
	ui->actionAqua->setCheckable(true);
	ui->actionConsole->setCheckable(true);
	ui->actionMacOS->setCheckable(true);
	ui->actionUbuntu->setCheckable(true);
	ui->actionDiplaytap->setCheckable(true);

	// デフォルトでManjaroに設定
	ui->actionManjaro->setChecked(true);
	connect(themeGroup.get(), SIGNAL(triggered(QAction*)), this, SLOT(changeTheme(QAction*)));
	//Qt5スタイル
	//connect(timer, &QTimer::timeout, this, &MainWindow::updateLabel);
	changeTheme(ui->actionManjaro);
}	

void MainWindow::connectSignalsAndSlots() {
	// formula発送
//	connect(this, SIGNAL(sendFormula(QString)), calculation, SLOT(formulaCalculator(QString)));
//	// answer回収
//	connect(calculation, SIGNAL(sendAnswer(QString)), this, SLOT(receiveAnswer(QString)));
	//Qt5スタイル
//	connect(this, &MainWindow::sendFormula, calculation, &Calculation::formulaCalculator);
//	connect(calculation, &Calculation::sendAnswer, this, &MainWindow::receiveAnswer);
	
//	connect(timer, SIGNAL(timeout()), this, SLOT(updateLabel()));
//	//Qt5スタイル
//	connect(themeGroup, &QActionGroup::triggered, this, &MainWindow::changeTheme);
	
	connect(this, &MainWindow::sendFormula, calculation.get(), &Calculation::formulaCalculator);
	connect(calculation.get(), &Calculation::sendAnswer, this, &MainWindow::receiveAnswer);
	connect(timer.get(), &QTimer::timeout, this, &MainWindow::updateLabel);
	
	// メニュー
	connect(ui->actionAboutMe, SIGNAL(triggered()), this, SLOT(about()));
	connect(ui->actionAboutQt, SIGNAL(triggered()), this, SLOT(aboutQt()));
	connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(exit()));
	// Qt5スタイル
	// connect(ui->actionAboutMe, &QAction::triggered, this, &MainWindow::about);
	// connect(ui->actionAboutQt, &QAction::triggered, this, &MainWindow::aboutQt);
	// connect(ui->actionExit, &QAction::triggered, this, &MainWindow::exit);

	// 入力ボタンをクリック
	connect(ui->btnNum7, SIGNAL(clicked()), this, SLOT(onInputButtonClicked()));
	connect(ui->btnNum8, SIGNAL(clicked()), this, SLOT(onInputButtonClicked()));
	connect(ui->btnNum9, SIGNAL(clicked()), this, SLOT(onInputButtonClicked()));
	connect(ui->btnSignPlus, SIGNAL(clicked()), this, SLOT(onInputButtonClicked()));
	connect(ui->btnSignLeft, SIGNAL(clicked()), this, SLOT(onInputButtonClicked()));
	connect(ui->btnNum4, SIGNAL(clicked()), this, SLOT(onInputButtonClicked()));
	connect(ui->btnNum5, SIGNAL(clicked()), this, SLOT(onInputButtonClicked()));
	connect(ui->btnNum6, SIGNAL(clicked()), this, SLOT(onInputButtonClicked()));
	connect(ui->btnSignMinus, SIGNAL(clicked()), this, SLOT(onInputButtonClicked()));
	connect(ui->btnSignRight, SIGNAL(clicked()), this, SLOT(onInputButtonClicked()));
	connect(ui->btnNum1, SIGNAL(clicked()), this, SLOT(onInputButtonClicked()));
	connect(ui->btnNum2, SIGNAL(clicked()), this, SLOT(onInputButtonClicked()));
	connect(ui->btnNum3, SIGNAL(clicked()), this, SLOT(onInputButtonClicked()));
	connect(ui->btnSignMulti, SIGNAL(clicked()), this, SLOT(onInputButtonClicked()));
	connect(ui->btnSignPoint, SIGNAL(clicked()), this, SLOT(onInputButtonClicked()));
	connect(ui->btnNum0, SIGNAL(clicked()), this, SLOT(onInputButtonClicked()));
	connect(ui->btnSignDivided, SIGNAL(clicked()), this, SLOT(onInputButtonClicked()));

	// Qt5スタイル
	// connect(ui->pushButton_7, &QPushButton::clicked, this, &MainWindow::onInputButtonClicked);
	// connect(ui->pushButton_8, &QPushButton::clicked, this, &MainWindow::onInputButtonClicked);
	// connect(ui->pushButton_9, &QPushButton::clicked, this, &MainWindow::onInputButtonClicked);
	// connect(ui->pushButton_plus, &QPushButton::clicked, this, &MainWindow::onInputButtonClicked);
	// connect(ui->pushButton_left, &QPushButton::clicked, this, &MainWindow::onInputButtonClicked);
	// connect(ui->pushButton_4, &QPushButton::clicked, this, &MainWindow::onInputButtonClicked);
	// connect(ui->pushButton_5, &QPushButton::clicked, this, &MainWindow::onInputButtonClicked);
	// connect(ui->pushButton_6, &QPushButton::clicked, this, &MainWindow::onInputButtonClicked);
	// connect(ui->pushButton_minus, &QPushButton::clicked, this, &MainWindow::onInputButtonClicked);
	// connect(ui->pushButton_right, &QPushButton::clicked, this, &MainWindow::onInputButtonClicked);
	// connect(ui->pushButton_1, &QPushButton::clicked, this, &MainWindow::onInputButtonClicked);
	// connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::onInputButtonClicked);
	// connect(ui->pushButton_3, &QPushButton::clicked, this, &MainWindow::onInputButtonClicked);
	// connect(ui->pushButton_multi, &QPushButton::clicked, this, &MainWindow::onInputButtonClicked);
	// connect(ui->pushButton_point, &QPushButton::clicked, this, &MainWindow::onInputButtonClicked);
	// connect(ui->pushButton_0, &QPushButton::clicked, this, &MainWindow::onInputButtonClicked);
	// connect(ui->pushButton_divided, &QPushButton::clicked, this, &MainWindow::onInputButtonClicked);	
}

// 開発者情報
void MainWindow::about() 
{
	QMessageBox::about(this, "About Me",
					"<center><h2>ShibaCalculator</h2></center><br>"
					"A cuter calculator based on Qt 5.7.1<br>"
					"Version " + VERSION + "<br>"
					"Designed by HikaruHoshino");
}

// Qtに関する情報
void MainWindow::aboutQt() 
{
	QMessageBox::aboutQt(this, "About Qt");
}

// テーマ変更
void MainWindow::changeTheme(QAction* action)
{
	QString mainWindowTheme = QCoreApplication::applicationDirPath() + "/styles/" + action->objectName().remove("action") + ".qss";
	QFile file(mainWindowTheme);
	
	if (!file.exists()) {
		QMessageBox::warning(this, "Warning", "The theme file is not found: " + mainWindowTheme);
		return;
	}
	if (!file.open(QIODevice::ReadOnly)) {
		QMessageBox::warning(this, "Warning", "Failed to open the theme file: " + mainWindowTheme);
		return;
	}
	
	setStyleSheet(file.readAll());
}

// 時間をラベルに更新
void MainWindow::updateLabel() 
{
	QDateTime currentDateTime = QDateTime::currentDateTime();
	QString dateTimeString = " " + currentDateTime.toString(u8"yyyy年MM月dd日 HH:mm:ss");
	ui->lblTime->setText(dateTimeString);
}


// 終了
void MainWindow::exit() 
{
	this->close();
}

// アプリ終了する時の動作
void MainWindow::closeEvent(QCloseEvent* event)
{
	// event参照警告対策
	//Q_UNUSED(event);

	// 前回の場所、大きさ情報をconfig.iniに記入
	QString settingPath = QCoreApplication::applicationDirPath() + "/config.ini";
	QSettings setting(settingPath, QSettings::IniFormat);
	setting.setValue("pos", this->pos());
	setting.setValue("width", this->width());
	setting.setValue("height", this->height());
	//
	QMainWindow::closeEvent(event);
}

// バックスペースのクリック動画表示(QLabelに変更するので廃棄)
//void MainWindow::animate_backspace()
//{
//	ui->pushButton_back->animateClick();
//}

// 結果回収関数
void MainWindow::receiveAnswer(const QString& ans)
{
	// 計算式と表示結果にansを記録
	gCalculationFormula = ans;
	gDisplayText = ans;

	// 計算結果の小数点判定
	if (gCalculationFormula.contains('.')) {
		gIsPointClicked = true;
	}
	else {
		gIsPointClicked = false;
	}

	// 表示結果をQLabelに示す
	ui->lblDisplay->setText(gDisplayText);

	// 履歴関連
	gHistoryText += ans;							// 履歴作成
	ui->txedtHistory->append(gHistoryText);			// 履歴に追加
}

// イベントフィルター
bool MainWindow::eventFilter(QObject* obj, QEvent* event)
{
    // イベントの種類はキーボード入力
    if (event->type() == QEvent::KeyPress) {
        auto keyEvent = std::unique_ptr<QKeyEvent>(static_cast<QKeyEvent*>(event));

        // キーが押し続けられた時
        if (keyEvent->isAutoRepeat()) {
            event->ignore();  // 重複する事件を無視する
        }
        // キーが一回だけ押した場合
        else {
            // 特殊状況対応
            if (keyEvent->key() == Qt::Key_Backspace) {
                ui->btnFuncBack->animateClick();  // backspace対応
            }
            else if (keyEvent->key() == Qt::Key_A) {
                ui->btnFuncAns->animateClick();  // ans対応
            }
            else if (keyEvent->key() == Qt::Key_Asterisk) {
                ui->btnSignMulti->animateClick();  // 乗算対応
            }
            else if (keyEvent->key() == Qt::Key_Slash) {
                ui->btnSignDivided->animateClick();  // 割り算対応
            }
            else {
                QKeySequence keySeq(keyEvent->key());  // キー入力イベント
                QString keyText = keySeq.toString();  // キー内容を文字列に変換
                auto allButtons = this->findChildren<QPushButton*>();  // 全てのQPushButtonをリストに入る
                for (auto& button : allButtons) {  // 対応ボタンを見つかるまでループ
                    if (button->text() == keyText) {
                        button->animateClick();
                        break;
                    }
                }
            }
            return true;  // イベントは既に処理しました、他の処理は要らない
        }
    }
    return QMainWindow::eventFilter(obj, event);  // ほかのイベント、親クラスに渡す
}

//----------------------------------------------------------------------------
//------------------------------ボタン反応関数--------------------------------
//----------------------------------------------------------------------------

// 機能ボタン以外のボタン反応関数
void MainWindow::onInputButtonClicked()
{
	// 無限対策
	if (gCalculationFormula.contains("inf")) {
		QMessageBox::warning(this, u8"操作エラー",
							 u8"無限は計算できないです。'C'や'←'を押してください。",
							 u8"了解");
		return;
	}

	// 未定義対策
	if (gCalculationFormula.contains("nan")) {
		QMessageBox::warning(this, u8"操作エラー",
							 u8"この結果は数学の定義がありません。'C'や'←'を押してください。",
							 u8"了解");
		return;
	}

	QPushButton* button = qobject_cast<QPushButton*>(sender());
	if (button) {
		QString buttonText = button->text();								// ボタンのテキストを獲得

		// 計算した直後、削除ではなく、数字を入力する時、全ての内容を消す
		if (gIsCalculated && buttonText.at(0).isDigit()) {
			gCalculationFormula.clear();
			gDisplayText.clear();
			
			gIsCalculated = false;											// 計算記録変数をFalseになる
		}
		else {
			gIsCalculated = false;											// 計算記録変数をFalseになる
		}

		// 最初に数学記号を入力する際に、数式の最初の数字を0として扱う
		if (gCalculationFormula.isEmpty() && (buttonText == u8"×" ||
											 buttonText == u8"÷" ||
											 buttonText == u8".")) {
			gCalculationFormula += "0";
			gDisplayText += "0";
		}
		// "+","-"の場合、表示する必要がない
		else if (gCalculationFormula.isEmpty() && (buttonText == "+" ||
				 buttonText == "-")) {
			gCalculationFormula += "0";
		}

		// 記号の後、小数点の入力を拒否
		if (! gCalculationFormula.isEmpty() && buttonText == "." && 
				(gCalculationFormula.endsWith("(") ||
				gCalculationFormula.endsWith("+") ||
				gCalculationFormula.endsWith("-") ||
				gCalculationFormula.endsWith("*") ||
				gCalculationFormula.endsWith("/") ||
				gCalculationFormula.endsWith("."))) {
			return;
		}

		// カッコの後ろに数字を入力すると、乗算として扱う
		if (! gCalculationFormula.isEmpty() && gCalculationFormula.endsWith(")") && buttonText.at(0).isDigit()) {
			gCalculationFormula += "*";
			gDisplayText += u8"×";
		}

		// もし formula の最後は"."、そして buttonTextは "(" や ")"
		if (! gCalculationFormula.isEmpty() && gCalculationFormula.endsWith(".") && (buttonText == "(" || buttonText == ")")) {
			// "."を削除
			gCalculationFormula.chop(1);
			gDisplayText.chop(1);
			// "("の場合乗算として扱う
			if (buttonText == "(") {
				gCalculationFormula += "*";
				gDisplayText += u8"×";
			}
		}

		// もし formula の最後は "(" や ")" そして buttonText は"."
		if (! gCalculationFormula.isEmpty() && (gCalculationFormula.endsWith("(") || gCalculationFormula.endsWith(")")) && buttonText == ".") {
			return;
		}

		// もし formula の最後は ")" そして buttonText は"("
		if (! gCalculationFormula.isEmpty() && gCalculationFormula.endsWith(")") && buttonText == "(") {
			// 乗算として扱う
			gCalculationFormula += "*";
			gDisplayText += u8"×";
		}

		// ansを計算に入る時、状況により補正
		if (gDisplayText.endsWith("s") && (buttonText == "(" || buttonText.at(0).isDigit())) {
			gCalculationFormula += "*";
			gDisplayText += u8"×";
		}

		// 想定連続及び一つの数字の中で、二回"."を押す時、第二回以上の入力を拒否する
		if (buttonText == ".") {
			if (gIsPointClicked == true) {
				return;
			}
			else {
				gIsPointClicked = true;
			}
		}
//         else if (buttonText == "C"){
//            pointClicked = false;
//        }
		// 数学記号を押すと、リミット解除
		else if (buttonText == "+" ||
				 buttonText == "-" ||
				 buttonText == u8"×" ||
				 buttonText == u8"÷") {
			gIsPointClicked = false;

			// formulaの最後の文字は+-*/.であれば拒否
			if (gCalculationFormula.endsWith("+") ||
					gCalculationFormula.endsWith("-") ||
					gCalculationFormula.endsWith("*") ||
					gCalculationFormula.endsWith("/") ||
					gCalculationFormula.endsWith(".")) {
				return;
			}
			// formulaの最後の文字は（
			else if (gCalculationFormula.endsWith("(")) {
				gCalculationFormula += "0";
				gDisplayText += "0";
			}
		}
		// カッコを押すと、小数点の入力リミット解除の上に左カッコの数量を記録
		else if (buttonText == "(") {
			gIsPointClicked = false;
			gLeftBracketCounter++;

			// もし数の後でカッコを押したら、乗算として処理する
			if (gCalculationFormula.length() > 0 && gCalculationFormula.at(gCalculationFormula.length() - 1).isDigit()) {
				gCalculationFormula += "*";
				gDisplayText += u8"×";
			}
		}
		else if (buttonText == ")") {
			gIsPointClicked = false;

			// 右カッコの数量が左カッコを超える場合、拒否
			if (gRightBracketCounter >= gLeftBracketCounter) {
				return;
			}
			else {
				gRightBracketCounter++;

				// もし左右カッコ連続押したら、0として処理
				if (gCalculationFormula.endsWith("(")) {
					gCalculationFormula += "0";
					gDisplayText += "0";
				}
			}
		}

		// 入力する内容は「×」の時
		if (buttonText == u8"×") {
			// 内部処理は"*"と扱う
			gCalculationFormula += "*";

			// 表示内容は"×"
			gDisplayText += u8"×";
		}

		// 入力する内容は「÷」の時
		if (buttonText == u8"÷") {
			// 内部処理は"/"と扱う
			gCalculationFormula += "/";

			// 表示内容は"÷"
			gDisplayText += u8"÷";
		}

		// "×"、"÷"以外、直接的にbuttonTextを加える
		else if (buttonText != u8"×" && buttonText != u8"÷") {
			gCalculationFormula += buttonText;
			gDisplayText += buttonText;
		}

		// QLabelで表示
		ui->lblDisplay->setText(gDisplayText);
	}
}

// 「C」ボタン反応関数
void MainWindow::on_btnFuncClear_clicked() 
{
	// formulaの内容を消す
	gCalculationFormula.clear();
	gDisplayText.clear();
	ui->lblDisplay->setText(gDisplayText);

	// カウントを元に戻す
	gIsCalculated = false;
	gIsPointClicked = false;
	gLeftBracketCounter = 0;
	gRightBracketCounter = 0;
}

// 「＝」ボタン反応関数
void MainWindow::on_btnFuncEqual_clicked()
{
	// 無限対策
	if (gCalculationFormula.contains("inf")) {
		QMessageBox::warning(this, u8"操作エラー",
							 u8"無限は計算できないです。'C'や'←'を押してください。",
							 u8"了解");
		return;
	}

	// 未定義対策
	if (gCalculationFormula.contains("nan")) {
		QMessageBox::warning(this, u8"操作エラー",
							 u8"この結果は数学の定義がありません。'C'や'←'を押してください。",
							 u8"了解");
		return;
	}

	// "="を押したら、余計な"."、"+"、"-"、"*"、"/"があれば、自動的に削除
	if (! gCalculationFormula.isEmpty() && (gCalculationFormula.at(gCalculationFormula.length() - 1) == "." ||
										   gCalculationFormula.at(gCalculationFormula.length() - 1) == "+" ||
										   gCalculationFormula.at(gCalculationFormula.length() - 1) == "-" ||
										   gCalculationFormula.at(gCalculationFormula.length() - 1) == "*" ||
										   gCalculationFormula.at(gCalculationFormula.length() - 1) == "/")) {
		gCalculationFormula.chop(1);
		gDisplayText.chop(1);

	}
// 廃棄、下の処理はもっと優秀
//if (!formula.isEmpty() && formula.at(formula.length() - 1) == "("){
//	formula.chop(2);
//}

	// "="を押したら、"("がある")"がない時、自動的に補完
	while (! gCalculationFormula.isEmpty() && gLeftBracketCounter > gRightBracketCounter) {
		if (gCalculationFormula.at(gCalculationFormula.length() - 1) == "(") {
			// "()"中は何もない，0を加える
			gCalculationFormula += "0";
		}
		gCalculationFormula += ")";
		gDisplayText += ")";
		gRightBracketCounter++;
	}

	// 過去結果を空にする
	gHistoryText.clear();

	// 表示している内容を追加
	gHistoryText += gDisplayText;

	// 計算履歴を記録
	gHistoryFormulaList.append(gCalculationFormula);
	gHistoryTextList.append(gDisplayText);

	// comboBoxに項目を追加
	ui->cmbHistorySelect->addItem(QString::number(gHistoryCounter));

	// comboBoxのシグナルをブロック
	QSignalBlocker blocker(ui->cmbHistorySelect);
	// comboBoxの選定を外す
	ui->cmbHistorySelect->setCurrentIndex(-1);
	// comboBoxのシグナルをアンブロック
	blocker.unblock();

	// 計算履歴のカウンターで記録
	gHistoryCounter++;

	// " = "を追加
	gHistoryText += " = ";

	// 計算記録変数をTrueになる
	gIsCalculated = true;

	// formulaをcalculationに発送
	emit sendFormula(gCalculationFormula);

	// formulaを確認できるデバッグスイッチ
	// qInfo() <<__FUNCTION__<< formula;

	// カウントを元に戻す
	gLeftBracketCounter = 0;
	gRightBracketCounter = 0;
}

// 「ans」ボタン反応関数
void MainWindow::on_btnFuncAns_clicked()
{
	// 無限対策
	if (gCalculationFormula.contains("inf")) {
		QMessageBox::warning(this, u8"提示",
							 u8"計算結果は無限です。'C'や'←'を押してください。",
							 u8"了解");
		return;
	}

	// 未定義対策
	if (gCalculationFormula.contains("nan")) {
		QMessageBox::warning(this, u8"提示",
							 u8"この結果は数学の定義がありません。'C'や'←'を押してください。",
							 u8"了解");
		return;
	}

	// ansのカンマ判定
	if (gAnsString.contains('.')) {
		gIsPointClicked = true;
	}
	else {
		gIsPointClicked = false;
	}

	// ansの前に数字がある時、"*"補正
	if (! gCalculationFormula.isEmpty() && (gCalculationFormula.at(gCalculationFormula.length() - 1).isDigit())) {
		gCalculationFormula += "*";
		gDisplayText += u8"×";
	}

	// 数式と表示内容を設置
	gCalculationFormula += gAnsString;
	gDisplayText += "ans";

	// QLabelに表示
	ui->lblDisplay->setText(gDisplayText);
}

// 「←」ボタン反応関数
void MainWindow::on_btnFuncBack_clicked()
{
	// 無限大がある場合、backspaceで3桁の内容を消す
	if (gCalculationFormula.endsWith("f")) {
		gCalculationFormula.chop(3);
		gDisplayText.chop(3);

	}
	// 未定義がある場合、backspaceで3桁の内容を消す
	else if (gCalculationFormula.endsWith("n")) {
		gCalculationFormula.chop(3);
		gDisplayText.chop(3);

	}
	// ansを発見する場合
	else if (gDisplayText.endsWith("s")) {
		gCalculationFormula.chop(gAnsString.length());							// formulaはQString「ans」の長さの文字を削除
		gDisplayText.chop(3);													// displayTextは"ans"だけを削除
	}
	else {
		if (! gCalculationFormula.isEmpty()) {
			// formula の最後の文字は "."であることを確認
			if (gCalculationFormula.at(gCalculationFormula.length() - 1) == ".") {
				gIsPointClicked = false;
			}
			// formula の最後の文字は "("であることを確認
			if (gCalculationFormula.at(gCalculationFormula.length() - 1) == "(") {
				gLeftBracketCounter--;
			}
			// formula の最後の文字は ")"であることを確認
			else if (gCalculationFormula.at(gCalculationFormula.length() - 1) == ")") {
				gRightBracketCounter--;
			}
			// 1桁を消す
			gCalculationFormula.chop(1);
			gDisplayText.chop(1);
		}
	}

	// formulaの最後の文字は数字の時
	if (! gDisplayText.isEmpty() && gDisplayText.at(gDisplayText.length() - 1).isDigit()) {
		// 数字入力制限
		gIsCalculated = true;
	}
	else {
		// 制限解除
		gIsCalculated = false;
	}

	// QLabelで表す
	ui->lblDisplay->setText(gDisplayText);
}

// 履歴探す、復元機能
void MainWindow::on_cmbHistorySelect_currentIndexChanged(int index) 
{
	gDisplayText = gHistoryTextList.at(index);								// 過去の表示数式を探す
	gCalculationFormula = gHistoryFormulaList.at(index);					// 過去の実際の数式を探す
	ui->lblDisplay->setText(gDisplayText);									// 表示数式を表す
}

// 履歴コピー機能
void MainWindow::on_btnFuncCopy_clicked() 
{
	// 履歴には何もない状態
	if (ui->txedtHistory->toPlainText().isEmpty()) {
		QMessageBox::warning(this, u8"提示",
							 u8"まだ計算履歴がありません。",
							 u8"了解");
		return;
	}
	// コピー、1秒の提示を表す
	else {
		QClipboard* clipboard = QApplication::clipboard();
		clipboard->setText(ui->txedtHistory->toPlainText());
		QToolTip::showText(QCursor::pos(), u8"コピー完了", nullptr, QRect(), 1000);
	}
}

// 履歴削除機能
void MainWindow::on_btnFuncDelete_clicked() 
{
	// 履歴には何もない状態
	if (ui->txedtHistory->toPlainText().isEmpty()) {
		QMessageBox::warning(this, u8"提示",
							 u8"まだ計算履歴がありません。",
							 u8"了解");
		return;
	}
	else {
		// on_comboBox_currentIndexChangedのシグナルをブロック
		QSignalBlocker blocker(ui->cmbHistorySelect);
		// comboBoxの内容を削除
		ui->cmbHistorySelect->clear();
		// シグナルをアンブロック
		blocker.unblock();

		// 全ての履歴を削除
		gHistoryTextList.clear();
		gHistoryFormulaList.clear();
		gHistoryText.clear();
		ui->txedtHistory->clear();
		// カウンターを戻す
		gHistoryCounter = 1;
		// 削除完了のお知らせ
		QToolTip::showText(QCursor::pos(), u8"削除完了", this, QRect(), 1000);
	}
}



//古いバージョン（eventFilterを使いません）
//void MainWindow::keyPressEvent(QKeyEvent *event)
//{
//    if (event->isAutoRepeat()) {
//        //キーが押し続けられた時、重複する事件を無視する
//        event->ignore();

//    //他の状況
//    } else {
//        if(event->key() == Qt::Key_Backspace)
//        {
//            ui->pushButton_back->animateClick();
//        }

//        if(event->key() == Qt::Key_A)
//        {
//            ui->pushButton_ans->animateClick();
//        }

//        else{
//            QKeySequence keySeq(event->key());
//            QString keyText = keySeq.toString();

//            QList<QPushButton *> allButtons = this->findChildren<QPushButton *>();

//            for(QPushButton *button : allButtons)
//            {
//                if(button->text() == keyText)
//                {
//                    button->animateClick();
//                    break;
//                }
//            }
//        }
//    }
//}
