#include "calculation.h"

// グローバル変数定義
QString gAnsString = "0";													// 計算結果文字列
double gAnsDouble = 0;														// 計算結果

Calculation::Calculation(QObject* parent)
	: QObject{parent}
{

}

//----------------------------------------------------------------------------
//------------------------------計算メイン関数--------------------------------
//----------------------------------------------------------------------------

HRESULT Calculation::formulaCalculator(const QString& formula)
{
    // ローカル変数定義
    QString receivedFormula;                                                // 計算式格納変数
    QStringList postfix;                                                    // 逆ポーランド記法格納配列
    QStringList formulaList;                                                // 中置記法格納配列
    QRegExp rx;                                                             // 正規表現パターン

    // 計算式を格納
    receivedFormula = formula;

//    qDebug() << "receivedFormula" << formula;

    // 無意味な入力を正規化
    if (receivedFormula == ""
            || receivedFormula == "("
            || receivedFormula == ")"
            || receivedFormula == "()"
            || receivedFormula == ")(") {
        receivedFormula = "0";
    }

    // 計算機能関数の実行
    HRESULT hr = S_OK;
    hr = devideFormula(receivedFormula, formulaList);                       // 数式の分割
    if (FAILED(hr)) return hr;

    hr = infixToPostfix(formulaList, postfix);                              // 数式を逆ポーランド式に変換
    if (FAILED(hr)) return hr;

    hr = postfixCalculator(postfix, gAnsDouble);                            // 計算
    if (FAILED(hr)) return hr;

//    qDebug() << __FUNCTION__ << "result(double)" << gAnsDouble;

    // 結果処理
    gAnsString = QString::number(gAnsDouble, 'f', 4);                       // 小数点以下4位残す、文字列に変換
    double temp = gAnsString.toDouble();                                    // doubleに変換
    rx.setPattern("(\\.){0,1}0+$");                                         // 小数点以下最後の「0」を消すパターン
    gAnsString = QString("%1").arg(temp, 0, 'f', -1).replace(rx, "");       // パターンを実行、Stringに変換

//    qDebug() << __FUNCTION__ << "result(QString)" << gAnsString;

    // 結果をmainwindowに返す
    emit sendAnswer(gAnsString);

    return S_OK;
}


//----------------------------------------------------------------------------
//------------------------------計算機能関数----------------------------------
//----------------------------------------------------------------------------

// 文字列分割関数
HRESULT Calculation::devideFormula(const QString& formula, QStringList& formulaList)
{
    // 必要の変数を定義/初期化
    QRegExp rx("(\\d+(\\.\\d+)?|\\+|-|\\*|/|\\(|\\))");                // 正規表現を定義
    int posFormula = 0;                                                // 位置記録変数を初期化
    bool lastWasOperator = true;

    // 正規表現にマッチ
    while ((posFormula = rx.indexIn(formula, posFormula)) != -1) {
        QString token = rx.cap(1);
        if (token.isEmpty()) {
            return E_FAIL; // 正規表現キャプチャが失敗した場合
        }

        if (token == "-" && lastWasOperator) {
            posFormula = rx.indexIn(formula, posFormula + 1);
            if (posFormula == -1) {
                return E_FAIL; // マイナス記号の後にトークンが見つからない場合
            }
            token += rx.cap(1);
            if (token.isEmpty()) {
                return E_FAIL; // 正規表現キャプチャが失敗した場合
            }
        }

        formulaList << token;
        lastWasOperator = (token == "+" || token == "-" || token == "*" || token == "/" || token == "(");
        posFormula += rx.matchedLength();                                // 次回の検査位置を確定
    }

    // formula_listを返す
    return S_OK;
}

HRESULT Calculation::infixToPostfix(const QStringList& infix, QStringList& postfix)
{
    // 変数定義
    QStack<QString> workStack;  // 処理用スタック

    // 入力为空时返回E_FAIL
    if (infix.isEmpty()) {
        return E_FAIL;
    }

    // infixを一度スキャン
    for (const QString& element : infix) {
        // 今スキャンした内容は"+"や"-"
        if (element == "+" || element == "-") {
            // スタックは空ではない、スタックの先頭は四則演算記号の時
            while (!workStack.isEmpty() && 
                    (workStack.top() == "+" 
                    || workStack.top() == "-" 
                    || workStack.top() == "*" 
                    || workStack.top() == "/")) {
                // スタックの先頭をpostfixにポップ
                postfix.append(workStack.pop());
            }

            // elementをスタックに入れる
            workStack.push(element);

        }
        // 今スキャンした内容は"*"や"/"
        else if (element == "*" || element == "/") {
            // スタックは空ではない、スタックの先頭は"*"や"/"
            while (!workStack.isEmpty() 
                    && (workStack.top() == "*" 
                    || workStack.top() == "/")) {
                // スタックの先頭をpostfixにポップ
                postfix.append(workStack.pop());
            }

            // elementをスタックに入れる
            workStack.push(element);

        }
        // 今スキャンした内容は"("
        else if (element == "(") {
            // elementをスタックに入れる
            workStack.push(element);
        }
        // 今スキャンした内容は")"
        else if (element == ")") {

            // スタックは空ではない、そして、スタックの先頭は"("ではない
            while (! workStack.isEmpty() && workStack.top() != "(") {
                // スタックの先頭をpostfixにポップ
                postfix.append(workStack.pop());
            }

            // スタックは空ではない(先頭は"(")
            if (! workStack.isEmpty()) {
                // スタックの先頭をポップ
                workStack.pop();
            } else {
                // 不匹配的括号
                return E_FAIL;
            }
        }
        else {
            // 数字であれば、その数字を直接的にpostfixに追加
            postfix.append(element);
        }
    }
    // スタックに空ではない（残すことをポップ）
    while (! workStack.isEmpty()) {
        if (workStack.top() == "(" || workStack.top() == ")") {
            // 不匹配的括号
            return E_FAIL;
        }
        // スタックの先頭をpostfixにポップ
        postfix.append(workStack.pop());
    }

    // postfixを返す
    return S_OK;
}

// 逆ポーランド式計算関数
HRESULT Calculation::postfixCalculator(const QStringList& postfix, double& result)
{
	// 変数定義
    QStack<QString> workStack;

    // 数式の長さは1の時に、Doubleに変更し、計算しなくて、値を返す
    if (postfix.size() == 1) {
        bool ok;
        result = postfix[0].toDouble(&ok);
        if (!ok) return E_FAIL;
        return S_OK;
    }

    // postfixをスキャン
    for (int i = 0; i < postfix.size(); ++i) {
        // elementでpostfix[i]を格納
        QString element = postfix[i];

        // elementは四則演算記号の時
        if (element.size() == 1 &&
            (element.at(0) == "+" 
            || element.at(0) == "-" 
            || element.at(0) == "*" 
            || element.at(0) == "/")) {
            // スタックの一番上の数字をrにポップ
            if (workStack.isEmpty()) return E_FAIL;
            bool ok;
            double r = workStack.pop().toDouble(&ok);
            if (!ok) return E_FAIL;

            // スタックの二番上の数字をlにポップ
            if (workStack.isEmpty()) return E_FAIL;
            double l = workStack.pop().toDouble(&ok);
            if (!ok) return E_FAIL;

            // 四則演算記号のタイプにより、計算方法を選択
            switch (element.at(0).toLatin1()) {
            case '+':
                result = l + r;
                break;
            case '-':
                result = l - r;
                break;
            case '*':
                result = l * r;
                break;
            case '/':
                if (r == 0) return E_FAIL; // 避免除以零
                result = l / r;
                break;
            default:
                return E_FAIL; // 未知の演算子の場合
            }

            // 結果をstackに入れる
            workStack.push_back(QString::number(result));
        }
        else {
            // 数字は直接的にstackに入れる
            workStack.push_back(element);
        }
    }

    // 結果を返す
    if (workStack.isEmpty()) return E_FAIL;
    bool ok;
    result = workStack.pop().toDouble(&ok);
    if (!ok) return E_FAIL;
    return S_OK;
}


