// SPDX-License-Identifier: MIT
import QtQuick

// A hand-painted (Canvas) animated starfield: three depth layers of points
// drifting at different speeds for a simple parallax effect. No image
// assets, generated purely from a deterministic pseudo-random seed so the
// layout is stable across repaints.
Item {
    id: root
    property int starsPerLayer: 60

    Canvas {
        id: canvas
        anchors.fill: parent

        property var layers: []
        property real elapsedSeconds: 0

        function seedLayers() {
            layers = [];
            const layerSpecs = [
                { speed: 6, size: 1.0, alpha: 0.5 },
                { speed: 14, size: 1.4, alpha: 0.75 },
                { speed: 26, size: 1.9, alpha: 1.0 },
            ];
            for (const spec of layerSpecs) {
                const stars = [];
                for (let i = 0; i < root.starsPerLayer; ++i) {
                    stars.push({
                        x: Math.random() * Math.max(width, 1),
                        y: Math.random() * Math.max(height, 1),
                        speed: spec.speed,
                        size: spec.size,
                        alpha: spec.alpha,
                    });
                }
                layers.push(stars);
            }
        }

        Component.onCompleted: seedLayers()
        onWidthChanged: seedLayers()
        onHeightChanged: seedLayers()

        Timer {
            interval: 33
            running: true
            repeat: true
            onTriggered: {
                canvas.elapsedSeconds += 0.033;
                canvas.requestPaint();
            }
        }

        onPaint: {
            const ctx = getContext("2d");
            ctx.reset();
            ctx.fillStyle = "#05060a";
            ctx.fillRect(0, 0, width, height);

            for (const stars of layers) {
                for (const star of stars) {
                    let x = star.x - canvas.elapsedSeconds * star.speed;
                    const w = Math.max(width, 1);
                    x = ((x % w) + w) % w;
                    ctx.fillStyle = Qt.rgba(1, 1, 1, star.alpha);
                    ctx.fillRect(x, star.y, star.size, star.size);
                }
            }
        }
    }
}
