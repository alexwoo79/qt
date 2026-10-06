import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    visible: true
    width: 420
    height: 560
    title: "Mini Qt 数据流转示例"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        TextField {
            id: nameField
            placeholderText: "姓名"
            Layout.fillWidth: true
        }

        SpinBox {
            id: ageField
            from: 1
            to: 150
            value: 20
            Layout.fillWidth: true
        }

        RowLayout {
            Layout.fillWidth: true

            Button {
                text: "提交"
                Layout.fillWidth: true
                onClicked: {
                    backend.submit(nameField.text, ageField.value)
                    nameField.text = ""
                }
            }

            Button {
                text: "清空"
                Layout.fillWidth: true
                onClicked: backend.clear()
            }
        }

        Text {
            text: backend.statusText
            color: text.startsWith("✅") ? "green"
                 : text.startsWith("❌") ? "red"
                 : "gray"
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
        }

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: backend.messages

            delegate: Rectangle {
                width: ListView.view.width
                height: 40
                color: index % 2 ? "#f7f7f7" : "white"

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin: 8
                    text: (index + 1) + ". " + modelData
                }
            }
        }

        Connections {
            target: backend
            function onSubmitSucceeded(summary) {
                console.log("C++ 通知成功:", summary)
            }
            function onSubmitFailed(reason) {
                console.log("C++ 通知失败:", reason)
            }
        }
    }
}