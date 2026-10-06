import QtQuick
import QtQuick.Controls
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

    property string selectedOperation: "+"
    property bool isUnary: ["sin", "cos", "tan", "log", "exp"].includes(selectedOperation)

    function calculate() {
        if (firstInput.text.trim() === "") {
            errorLabel.text = "请输入一个有效数字。"
            return
        }
        if (!isUnary && secondInput.text.trim() === "") {
            errorLabel.text = "请输入第二个数字。"
            return
        }

        const first = Number(firstInput.text)
        const second = isUnary ? 0 : Number(secondInput.text)
        if (!Number.isFinite(first) || !Number.isFinite(second)) {
            errorLabel.text = "请输入有效数字。"
            return
        }

        const response = calculator.calculate(selectedOperation, first, second)
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
                visible: !window.isUnary
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
                visible: !window.isUnary
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
                model: [
                    { label: "+", value: "+" },
                    { label: "−", value: "-" },
                    { label: "×", value: "*" },
                    { label: "÷", value: "/" },
                    { label: "%", value: "%" },
                    { label: "xʸ", value: "^" },
                    { label: "sin", value: "sin" },
                    { label: "cos", value: "cos" },
                    { label: "tan", value: "tan" },
                    { label: "ln", value: "log" },
                    { label: "eˣ", value: "exp" }
                ]

                delegate: Button {
                    required property var modelData
                    Layout.fillWidth: true
                    Layout.preferredHeight: 46
                    text: modelData.label
                    highlighted: window.selectedOperation === modelData.value
                    font.pixelSize: 15
                    font.weight: Font.DemiBold
                    onClicked: {
                        window.selectedOperation = modelData.value
                        errorLabel.text = ""
                    }
                    background: Rectangle {
                        radius: 7
                        color: parent.highlighted ? "#176b55" : (parent.hovered ? "#e1e9e2" : "#ffffff")
                        border.color: parent.highlighted ? "#176b55" : "#d4ddd5"
                    }
                    contentItem: Text {
                        text: parent.text
                        color: parent.highlighted ? "#ffffff" : "#263b2f"
                        font: parent.font
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }
        }

        Item { Layout.fillHeight: true }

        Button {
            Layout.fillWidth: true
            Layout.preferredHeight: 50
            text: "计算"
            font.pixelSize: 16
            font.weight: Font.DemiBold
            onClicked: window.calculate()
            background: Rectangle {
                radius: 8
                color: parent.down ? "#105540" : (parent.hovered ? "#1c7a61" : "#176b55")
            }
            contentItem: Text {
                text: parent.text
                color: "#ffffff"
                font: parent.font
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
    }
}