import QtQuick 2.9

MouseArea {
    id: root

    QtObject {
        id: __

        property var line: []
        property var thinking: []

        readonly property var letters: [
            'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O'
        ]

        function pos2coord(x) {
            var k = width / 15
            return Math.round((x - k / 2) / k)
        }
    }

    function putCoord(x, y) {
        var coord = Qt.point(x, y)
        for (var i = 0; i < __.line.length; ++i) {
            if (__.line[i] === coord) return Qt.point(-1, -1)
        }
        __.line.push(coord)
        canvas.requestPaint()
        return coord
    }

    function putPos(x, y) {
        return putCoord(__.pos2coord(x), __.pos2coord(y))
    }

    function clear() {
        __.line = []
        __.thinking = []
        canvas.requestPaint()
    }

    function setThinking(list) {
        __.thinking = list
        canvas.requestPaint()
    }

    Canvas {
        id: canvas
        anchors.fill: parent

        onPaint: {
            var ctx = getContext("2d")
            var k = width / 15, w = width - k

            /* fill background */

            ctx.beginPath()
            ctx.rect(0, 0, width, width)
            ctx.closePath()

            ctx.fillStyle = "white"
            ctx.fill()

            /* draw border */

            ctx.translate(0.5, 0.5)
            ctx.beginPath()
            ctx.rect(k / 2, k / 2, w, w)
            ctx.closePath()

            /* draw lines */

            for (var i = 1; i < 14; ++i) {
                ctx.moveTo(k / 2, k / 2 + i * k)
                ctx.lineTo(k / 2 + w, k / 2 + i * k)
            }
            for (i = 1; i < 14; ++i) {
                ctx.moveTo(k / 2 + i * k, k / 2)
                ctx.lineTo(k / 2 + i * k, k / 2 + w)
            }

            ctx.strokeStyle = 'black'
            ctx.lineWidth = 1
            ctx.fill()
            ctx.stroke()
            ctx.translate(-0.5, -0.5)

            /* draw stars */

            ctx.beginPath()
            ctx.arc((k + w) / 2, (k + w) / 2, 5, 0, Math.PI * 2)
            ctx.closePath()

            ctx.fillStyle = "black"
            ctx.fill()

            ctx.beginPath()
            ctx.arc( 3.5 * k,  3.5 * k, 4, 0, Math.PI * 2)
            ctx.arc(11.5 * k,  3.5 * k, 4, 0, Math.PI * 2)
            ctx.closePath()
            ctx.fill()

            ctx.beginPath()
            ctx.arc( 3.5 * k, 11.5 * k, 4, 0, Math.PI * 2)
            ctx.arc(11.5 * k, 11.5 * k, 4, 0, Math.PI * 2)
            ctx.closePath()
            ctx.fill()

            /* draw coordinate texts */

            ctx.font="14px Arial"
            ctx.textAlign = "center"
            ctx.textBaseline = "middle"

            for (i = 0; i < 15; ++i) {
                ctx.fillText(__.letters[i], k / 2 + i * k, k / 4)
            }
            for (i = 0; i < 15; ++i) {
                ctx.fillText(i + 1, k / 4, k / 2 + i * k)
            }

            /* draw pieces */

            for (i = 0; i < __.line.length; i += 2) {
                ctx.beginPath()
                ctx.arc(k / 2 + __.line[i].x * k, k / 2 + __.line[i].y * k, k / 2 - 1, 0, Math.PI * 2)
                ctx.closePath()
                ctx.fill()
            }

            ctx.fillStyle = "white"
            for (i = 1; i < __.line.length; i += 2) {
                ctx.beginPath()
                ctx.arc(k / 2 + __.line[i].x * k, k / 2 + __.line[i].y * k, k / 2 - 1, 0, Math.PI * 2)
                ctx.closePath()
                ctx.fill()
                ctx.stroke()
            }

            if (__.line.length > 0) {
                var last = __.line[__.line.length - 1]
                ctx.beginPath()
                ctx.arc(k / 2 + last.x * k, k / 2 + last.y * k, 4, 0, Math.PI * 2)
                ctx.closePath()
                if ((__.line.length % 2) === 0) {
                    ctx.fillStyle = "black"
                }
                ctx.fill()
            }

            /* thinking... */

            for (i = 0; i < __.thinking.length; ++i) {
                ctx.fillStyle = Qt.rgba(1, 0, 0, __.thinking[i].visits / __.thinking[0].visits)
                ctx.beginPath()
                ctx.arc(k / 2 + __.thinking[i].x * k, k / 2 + __.thinking[i].y * k, 3, 0, Math.PI * 2)
                ctx.closePath()
                ctx.fill()
            }
        }
    }
}
