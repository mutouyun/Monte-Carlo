import QtQuick 2.9

MouseArea {
    id: root

    QtObject {
        id: __

        property var line: []

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
        canvas.requestPaint()
    }

    Canvas {
        id: canvas
        anchors.fill: parent
        antialiasing: false

        onPaint: {
            var ctx = getContext("2d")
            var k = width / 15, w = width - k

            ctx.beginPath()
            ctx.rect(0, 0, width, width)
            ctx.closePath()

            ctx.fillStyle = "white"
            ctx.fill()

            ctx.beginPath()
            ctx.rect(k / 2, k / 2, w, w)
            ctx.closePath()

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

            ctx.beginPath()
            ctx.arc((k + w) / 2, (k + w) / 2, 5, 0, Math.PI * 2)
            ctx.closePath()

            ctx.fillStyle = "black"
            ctx.fill()

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
        }
    }
}
