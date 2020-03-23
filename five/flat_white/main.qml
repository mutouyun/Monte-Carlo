import QtQuick 2.9
import QtQuick.Window 2.2
import FlatWhite 1.0

Window {
    id: root

    visible: true
    title: qsTr("flat white")

    minimumWidth: 800
    minimumHeight: 600
    width: minimumWidth
    height: minimumHeight

    Connections {
        target: Engine

        onThinking: {
            board.setThinking(list)
        }

        onThinkingFinished: {
            board.setThinking([])
            board.putCoord(x, y)
        }
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
