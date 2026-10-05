import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    visible: true
    width: 360
    height: 400
    title: "Counter Demo"

    ColumnLayout {
        anchors.centerIn: parent
        spacing: 16

        RowLayout {
            spacing: 20

            Text {
                text: "当前值: " + counter.value
                font.pixelSize: 24
                Layout.alignment: Qt.AlignHCenter
            }

            Text {
                text: "双倍值: " + counter.doubled
                font.pixelSize: 24
                Layout.alignment: Qt.AlignHCenter
            }
        }

        RowLayout {
            spacing: 12

            Button {
                text: "+" + counter.step
                onClicked: counter.increment()
            }

            Button {
                text: "重置"
                onClicked: counter.reset()
            }

            Button {
                text: "设为 100"
                onClicked: counter.value = 100
            }
        }

        RowLayout {
            spacing: 8

            Text {
                text: "步长"
            }

            SpinBox {
                id: stepSpinBox
                from: 1
                to: 100
                value: counter.step
                onValueModified: counter.step = value
            }
        }

        ColumnLayout {
            Layout.preferredWidth: 320
            spacing: 8

            Text {
                text: "操作历史"
            }

            ListView {
                id: historyView
                Layout.fillWidth: true
                Layout.preferredHeight: 120
                clip: true
                model: counter.history
                delegate: Text {
                    text: modelData
                }
                onCountChanged: positionViewAtEnd()
            }
        }

        // 监听 C++ 信号
        Connections {
            target: counter
            function onValueChanged(newValue) {
                console.log("C++ 通知：值变为", newValue);
            }
        }
    }
}
