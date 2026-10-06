import QtQuick
// 直接导入具体样式：只需要部署 Basic 这一套控件，不用把 Fusion/Material/
// Universal/Imagine/Windows 等样式一起带上（省下十几 MB 依赖）。
import QtQuick.Controls.Basic
import QtQuick.Layouts

ApplicationWindow {
    id: window

    width: 620
    height: 700
    minimumWidth: 420
    minimumHeight: 620
    visible: true
    title: "简单计算器"
    color: "#eef2ee"

    // 状态集中放在 QtObject 里：类型明确，QML 编译器才能把这些绑定
    // 提前编译成原生代码（放在 ApplicationWindow 上时编译器解析不了）。
    // 注意 id 不能叫 state —— 那是每个 Item 自带的属性，会被遮蔽掉。
    QtObject {
        id: calcState

        property string selectedOperation: "+"
        // 逐项比较而不是数组 indexOf：数组是 var 类型 + 未类型化的 JS 调用，
        // 编译器无法提前编译，这里全部走编译期常量比较。
        readonly property bool isUnary: calcState.selectedOperation === "sin"
                                        || calcState.selectedOperation === "cos"
                                        || calcState.selectedOperation === "tan"
                                        || calcState.selectedOperation === "log"
                                        || calcState.selectedOperation === "exp"
    }

    function calculate(): void {
        if (firstInput.text.trim() === "") {
            errorLabel.text = "请输入一个有效数字。"
            return
        }
        if (!calcState.isUnary && secondInput.text.trim() === "") {
            errorLabel.text = "请输入第二个数字。"
            return
        }

        const first = Number(firstInput.text)
        const second = calcState.isUnary ? 0 : Number(secondInput.text)
        if (!Number.isFinite(first) || !Number.isFinite(second)) {
            errorLabel.text = "请输入有效数字。"
            return
        }

        const response = calculator.calculate(calcState.selectedOperation, first, second)
        if (response.ok) {
            resultLabel.text = Number(response.value).toLocaleString(Qt.locale(), "g", 12)
            errorLabel.text = ""
        } else {
            errorLabel.text = response.error
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 28
        spacing: 20

        RowLayout {
            Layout.fillWidth: true
            spacing: 12

            Rectangle {
                Layout.preferredWidth: 42
                Layout.preferredHeight: 42
                radius: 12
                color: "#176b55"

                Text {
                    anchors.centerIn: parent
                    text: "∑"
                    color: "white"
                    font.pixelSize: 25
                    font.weight: Font.DemiBold
                }
            }

            ColumnLayout {
                spacing: 2
                Text {
                    text: "计算器"
                    color: "#192720"
                    font.pixelSize: 22
                    font.weight: Font.DemiBold
                }
                Text {
                    text: "基础与科学运算"
                    color: "#64746b"
                    font.pixelSize: 13
                }
            }

            Item { Layout.fillWidth: true }

            Label {
                text: "RAD"
                color: "#176b55"
                font.pixelSize: 12
                font.weight: Font.Bold
                leftPadding: 10
                rightPadding: 10
                topPadding: 7
                bottomPadding: 7
                background: Rectangle {
                    radius: 6
                    color: "#dcebe3"
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 150
            radius: 12
            color: "#192720"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 10

                Text {
                    text: "结果"
                    color: "#a9b8ad"
                    font.pixelSize: 13
                }
                Text {
                    id: resultLabel
                    Layout.fillWidth: true
                    text: "0"
                    color: "#f4f7f3"
                    font.pixelSize: 36
                    font.weight: Font.Medium
                    elide: Text.ElideLeft
                    horizontalAlignment: Text.AlignRight
                }
                Text {
                    id: errorLabel
                    Layout.fillWidth: true
                    text: ""
                    color: "#ffab91"
                    font.pixelSize: 13
                    horizontalAlignment: Text.AlignRight
                    elide: Text.ElideRight
                }
            }
        }

        GridLayout {
            Layout.fillWidth: true
            columns: 2
            columnSpacing: 12
            rowSpacing: 8

            Text {
                text: "第一个数"
                color: "#45584d"
                font.pixelSize: 13
            }
            Text {
                visible: !calcState.isUnary
                text: "第二个数"
                color: "#45584d"
                font.pixelSize: 13
            }

            TextField {
                id: firstInput
                Layout.fillWidth: true
                placeholderText: "例如 12.5"
                text: ""
                color: "#192720"
                selectionColor: "#176b55"
                selectedTextColor: "#ffffff"
                selectByMouse: true
                inputMethodHints: Qt.ImhFormattedNumbersOnly
                validator: DoubleValidator { notation: DoubleValidator.ScientificNotation }
                background: Rectangle {
                    radius: 7
                    color: "#ffffff"
                    border.color: firstInput.activeFocus ? "#176b55" : "#d4ddd5"
                }
            }

            TextField {
                id: secondInput
                visible: !calcState.isUnary
                Layout.fillWidth: true
                placeholderText: "例如 3"
                text: ""
                color: "#192720"
                selectionColor: "#176b55"
                selectedTextColor: "#ffffff"
                selectByMouse: true
                inputMethodHints: Qt.ImhFormattedNumbersOnly
                validator: DoubleValidator { notation: DoubleValidator.ScientificNotation }
                background: Rectangle {
                    radius: 7
                    color: "#ffffff"
                    border.color: secondInput.activeFocus ? "#176b55" : "#d4ddd5"
                }
            }
        }

        Text {
            text: "运算"
            color: "#45584d"
            font.pixelSize: 13
        }

        GridLayout {
            Layout.fillWidth: true
            columns: 5
            rowSpacing: 8
            columnSpacing: 8

            Repeater {
                // 用 ListModel 而不是 JS 数组：角色有明确类型，委托里的绑定
                // 才能被提前编译（JS 数组只能退化成运行时解析）
                model: ListModel {
                    ListElement { label: "+";   value: "+" }
                    ListElement { label: "−";   value: "-" }
                    ListElement { label: "×";   value: "*" }
                    ListElement { label: "÷";   value: "/" }
                    ListElement { label: "%";   value: "%" }
                    ListElement { label: "xʸ";  value: "^" }
                    ListElement { label: "sin"; value: "sin" }
                    ListElement { label: "cos"; value: "cos" }
                    ListElement { label: "tan"; value: "tan" }
                    ListElement { label: "ln";  value: "log" }
                    ListElement { label: "eˣ";  value: "exp" }
                }

                delegate: Button {
                    id: operationButton

                    required property string label
                    required property string value

                    Layout.fillWidth: true
                    Layout.preferredHeight: 46
                    text: operationButton.label
                    highlighted: calcState.selectedOperation === operationButton.value
                    font.pixelSize: 15
                    font.weight: Font.DemiBold
                    onClicked: {
                        calcState.selectedOperation = operationButton.value
                        errorLabel.text = ""
                    }
                    background: Rectangle {
                        radius: 7
                        color: operationButton.highlighted ? "#176b55" : (operationButton.hovered ? "#e1e9e2" : "#ffffff")
                        border.color: operationButton.highlighted ? "#176b55" : "#d4ddd5"
                    }
                    contentItem: Text {
                        text: operationButton.text
                        color: operationButton.highlighted ? "#ffffff" : "#263b2f"
                        font: operationButton.font
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }
        }

        Item { Layout.fillHeight: true }

        Button {
            id: calculateButton

            Layout.fillWidth: true
            Layout.preferredHeight: 50
            text: "计算"
            font.pixelSize: 16
            font.weight: Font.DemiBold
            onClicked: window.calculate()
            background: Rectangle {
                radius: 8
                color: calculateButton.down ? "#105540" : (calculateButton.hovered ? "#1c7a61" : "#176b55")
            }
            contentItem: Text {
                text: calculateButton.text
                color: "#ffffff"
                font: calculateButton.font
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
    }
}
