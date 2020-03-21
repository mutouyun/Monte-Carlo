import QtQuick 2.9
import QtQuick.Window 2.2
import FlatWhite 1.0

Window {
    id: root
    visible: true
    width: 800
    height: 600
    title: qsTr("flat white")

    Connections {
        target: Engine
        onThinkingFinished: board.putCoord(x, y)
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.RightButton

        onClicked: {
            Engine.restart()
            board.clear()
        }
    }

    Board {
        id: board
        anchors.centerIn: parent
        width: Math.min(root.width, root.height)
        height: width
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        enabled: !Engine.processing

        onClicked: {
            if (mouse.button === Qt.LeftButton) {
                var coord = putPos(mouse.x, mouse.y)
                Engine.move(coord.x, coord.y)
            }
            else if (mouse.button === Qt.RightButton) {
                Engine.calcNext()
            }
        }
    }
}