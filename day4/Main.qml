import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: root

    visible: true
    width: 360
    height: 560
    minimumWidth: 360
    minimumHeight: 560
    title: "计数器"
    color: "#F1F5F2"

    readonly property color ink: "#173B34"
    readonly property color muted: "#71827B"
    readonly property color accent: "#C8EF72"
    readonly property color accentPressed: "#B6DE5F"
    readonly property color surface: "#FFFFFF"
    readonly property string bodyFont: "Noto Sans"

    ScrollView {
        id: scrollArea
        anchors.fill: parent
        clip: true
        contentWidth: availableWidth

        ColumnLayout {
            id: page
            x: 22
            width: Math.max(0, scrollArea.availableWidth - 44)
            spacing: 20

            Item { Layout.preferredHeight: 2 }

            RowLayout {
                Layout.fillWidth: true

                ColumnLayout {
                    spacing: 3

                    Text {
                        text: "DAY 04  /  TOOLKIT"
                        color: root.muted
                        font.family: root.bodyFont
                        font.pixelSize: 11
                        font.weight: Font.DemiBold
                    }

                    Text {
                        text: "计数器"
                        color: root.ink
                        font.family: root.bodyFont
                        font.pixelSize: 28
                        font.weight: Font.Bold
                    }
                }

                Item { Layout.fillWidth: true }

                RowLayout {
                    spacing: 7

                    Rectangle {
                        Layout.preferredWidth: 8
                        Layout.preferredHeight: 8
                        radius: 4
                        color: "#5EAA79"
                    }

                    Text {
                        text: "运行中"
                        color: root.muted
                        font.family: root.bodyFont
                        font.pixelSize: 12
                    }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 236
                radius: 22
                color: root.ink

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 22
                    spacing: 8

                    Text {
                        text: "当前值"
                        color: "#B5CAC1"
                        font.family: root.bodyFont
                        font.pixelSize: 13
                        font.weight: Font.Medium
                    }

                    Text {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        text: counter.value
                        color: "#F5FAF6"
                        font.family: "Noto Sans Mono"
                        font.pixelSize: 76
                        font.weight: Font.DemiBold
                        fontSizeMode: Text.Fit
                        minimumPixelSize: 38
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        elide: Text.ElideNone
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 1
                        color: "#42635A"
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 6

                        Text {
                            text: "双倍值"
                            color: "#B5CAC1"
                            font.family: root.bodyFont
                            font.pixelSize: 13
                        }

                        Item { Layout.fillWidth: true }

                        Text {
                            text: counter.doubled
                            color: root.accent
                            font.family: "Noto Sans Mono"
                            font.pixelSize: 19
                            font.weight: Font.DemiBold
                        }
                    }
                }
            }

            GridLayout {
                Layout.fillWidth: true
                columns: 2
                rowSpacing: 10
                columnSpacing: 10

                Button {
                    id: incrementButton
                    Layout.columnSpan: 2
                    Layout.fillWidth: true
                    Layout.preferredHeight: 58
                    onClicked: counter.increment()

                    background: Rectangle {
                        radius: 15
                        color: incrementButton.down ? root.accentPressed : root.accent
                    }

                    contentItem: Text {
                        text: "+   增加 " + counter.step
                        color: root.ink
                        font.family: root.bodyFont
                        font.pixelSize: 17
                        font.weight: Font.Bold
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }

                Button {
                    id: resetButton
                    Layout.fillWidth: true
                    Layout.preferredHeight: 48
                    onClicked: counter.reset()

                    background: Rectangle {
                        radius: 14
                        color: resetButton.down ? "#E6ECE8" : root.surface
                        border.color: "#DCE5DF"
                    }

                    contentItem: Text {
                        text: "重置"
                        color: root.ink
                        font.family: root.bodyFont
                        font.pixelSize: 14
                        font.weight: Font.DemiBold
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }

                Button {
                    id: setValueButton
                    Layout.fillWidth: true
                    Layout.preferredHeight: 48
                    onClicked: counter.value = 100

                    background: Rectangle {
                        radius: 14
                        color: setValueButton.down ? "#E6ECE8" : root.surface
                        border.color: "#DCE5DF"
                    }

                    contentItem: Text {
                        text: "设为 100"
                        color: root.ink
                        font.family: root.bodyFont
                        font.pixelSize: 14
                        font.weight: Font.DemiBold
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 12

                ColumnLayout {
                    spacing: 2

                    Text {
                        text: "步长"
                        color: root.ink
                        font.family: root.bodyFont
                        font.pixelSize: 15
                        font.weight: Font.DemiBold
                    }

                    Text {
                        text: "每次增加的数值"
                        color: root.muted
                        font.family: root.bodyFont
                        font.pixelSize: 11
                    }
                }

                Item { Layout.fillWidth: true }

                SpinBox {
                    id: stepSpinBox
                    from: 1
                    to: 100
                    value: counter.step
                    editable: true
                    Layout.preferredWidth: 124
                    Layout.preferredHeight: 46
                    onValueModified: counter.step = value

                    contentItem: TextInput {
                        z: 2
                        text: stepSpinBox.displayText
                        font: stepSpinBox.font
                        color: root.ink
                        selectionColor: root.accent
                        selectedTextColor: root.ink
                        horizontalAlignment: TextInput.AlignHCenter
                        verticalAlignment: TextInput.AlignVCenter
                        readOnly: !stepSpinBox.editable
                        validator: stepSpinBox.validator
                        inputMethodHints: Qt.ImhFormattedNumbersOnly
                    }

                    up.indicator: Text {
                        x: stepSpinBox.width - width - 7
                        y: 5
                        width: 30
                        height: (stepSpinBox.height - 10) / 2
                        text: "+"
                        color: root.muted
                        font.family: root.bodyFont
                        font.pixelSize: 15
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    down.indicator: Text {
                        x: stepSpinBox.width - width - 7
                        y: stepSpinBox.height / 2
                        width: 30
                        height: (stepSpinBox.height - 10) / 2
                        text: "−"
                        color: root.muted
                        font.family: root.bodyFont
                        font.pixelSize: 15
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    background: Rectangle {
                        radius: 13
                        color: root.surface
                        border.color: "#DCE5DF"
                    }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 1
                color: "#DCE5DF"
            }

            RowLayout {
                Layout.fillWidth: true

                Text {
                    text: "操作记录"
                    color: root.ink
                    font.family: root.bodyFont
                    font.pixelSize: 17
                    font.weight: Font.Bold
                }

                Item { Layout.fillWidth: true }

                Text {
                    text: counter.history.length
                    color: root.muted
                    font.family: "Noto Sans Mono"
                    font.pixelSize: 12
                }
            }

            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: 156

                ListView {
                    id: historyView
                    anchors.fill: parent
                    spacing: 7
                    clip: true
                    model: counter.history
                    delegate: Rectangle {
                        required property int index
                        required property var modelData

                        width: historyView.width
                        height: 42
                        radius: 12
                        color: index % 2 === 0 ? root.surface : "#E9F0EC"

                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 13
                            anchors.rightMargin: 13

                            Text {
                                text: "#" + (index + 1)
                                color: root.muted
                                font.family: "Noto Sans Mono"
                                font.pixelSize: 12
                            }

                            Item { Layout.fillWidth: true }

                            Text {
                                text: modelData
                                color: root.ink
                                font.family: "Noto Sans Mono"
                                font.pixelSize: 14
                                font.weight: Font.DemiBold
                            }
                        }
                    }
                    onCountChanged: positionViewAtEnd()
                }

                Text {
                    anchors.centerIn: parent
                    visible: historyView.count === 0
                    text: "暂无记录"
                    color: root.muted
                    font.family: root.bodyFont
                    font.pixelSize: 13
                }
            }
        }
    }

    Connections {
        target: counter
        function onValueChanged(newValue) {
            console.log("C++ 通知：值变为", newValue);
        }
    }
}
