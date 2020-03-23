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
        contextType: "2d"

        property real k: width / 15

        function fill(color, action) {
            context.fillStyle = color
            context.beginPath()
            action()
            context.fill()
        }

        function stroke(color, action) {
            context.strokeStyle = color
            context.beginPath()
            action()
            context.stroke()
        }

        function coord(c) {
            return ~~((c + 0.5) * k)
        }

        function circle(x, y, r) {
            context.ellipse(coord(x) - r, coord(y) - r, r * 2, r * 2)
        }

        onPaint: {
            context.clearRect(0, 0, width, height)
            context.translate(0.5, 0.5)

            /* draw lines */

            stroke('black', function() {
                context.lineWidth = 1
                for (var i = 0; i < 15; ++i) {
                    context.moveTo(coord(0) , coord(i) )
                    context.lineTo(coord(14), coord(i) )
                    context.moveTo(coord(i) , coord(0) )
                    context.lineTo(coord(i) , coord(14))
                }
            })

            /* draw stars */

            fill('black', function() {
                circle( 7,  7, 5)
                circle( 3,  3, 4)
                circle(11,  3, 4)
                circle( 3, 11, 4)
                circle(11, 11, 4)
            })

            /* draw coordinate texts */

            context.font="14px Arial"
            context.textAlign = "center"
            context.textBaseline = "middle"
            for (var i = 0; i < 15; ++i) {
                context.fillText(__.letters[i], coord(i), coord(0) / 2)
                context.fillText(i + 1        , coord(0) / 2, coord(i))
            }

            /* draw pieces */

            fill('black', function() {
                for (var i = 0; i < __.line.length; i += 2) {
                    circle(__.line[i].x, __.line[i].y, coord(0) - 1)
                }
            })
            fill('white', function() {
                for (var i = 1; i < __.line.length; i += 2) {
                    circle(__.line[i].x, __.line[i].y, coord(0) - 1)
                }
                context.lineWidth = 2
                context.stroke()
            })
            if (__.line.length > 0) {
                var last = __.line[__.line.length - 1]
                fill(((__.line.length % 2) === 0) ? Qt.rgba(0, 0, 0, 0.95) :
                                                    Qt.rgba(1, 1, 1, 0.95),
                     function() { circle(last.x, last.y, 4) })
            }

            /* thinking... */

            if (__.thinking.length > 0) {
                fill(Qt.rgba(1, 0, 0, 1), function() {
                    circle(__.thinking[0].x, __.thinking[0].y, 3)
                    context.stroke()
                })
                for (i = 1; i < __.thinking.length; ++i) {
                    fill(Qt.rgba(1, 0, 0, __.thinking[i].visits / (__.thinking[0].visits * 2)), function() {
                        circle(__.thinking[i].x, __.thinking[i].y, 3)
                    })
                }
            }

            context.translate(-0.5, -0.5)
        }
    }
}
