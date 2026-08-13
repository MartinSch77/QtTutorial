// SPDX-License-Identifier: MIT
import QtQuick

import "Icons.js" as Icons

// Radar-plot-style sensor track display - a passive visualization of
// simulated tracked objects (position, heading, a generic classification
// label). There is no weapons data, no aim/intercept solution and no
// launch/fire control anywhere in this panel: it is exactly the kind of
// screen an air-defence or maritime sensor operator watches, showing what
// is out there, not how to engage it. Alongside the platform's own
// ("organic") sensor tracks, it also plots simulated tactical-data-link
// tracks received from other platforms; those go stale/intermittent when
// the Comms subsystem degrades, purely as a data-quality effect.
Rectangle {
    id: root
    property var sim
    color: "#0a0f14"

    Canvas {
        id: canvas
        anchors.fill: parent
        anchors.margins: 16

        readonly property double areaRadiusKm: root.sim ? root.sim.trackAreaRadiusKm : 40.0

        Timer {
            interval: 100
            running: true
            repeat: true
            onTriggered: canvas.requestPaint()
        }

        onPaint: {
            const ctx = getContext("2d");
            ctx.reset();
            const cx = width / 2;
            const cy = height / 2;
            const radius = Math.min(width, height) / 2 - 8;
            const scale = radius / canvas.areaRadiusKm;

            ctx.strokeStyle = "#1f2a33";
            ctx.lineWidth = 1;
            ctx.font = "10px sans-serif";
            for (let ring = 1; ring <= 4; ++ring) {
                const ringRadius = radius * ring / 4;
                ctx.beginPath();
                ctx.arc(cx, cy, ringRadius, 0, 2 * Math.PI);
                ctx.stroke();
                ctx.fillStyle = "#4c5b68";
                ctx.fillText(Math.round(canvas.areaRadiusKm * ring / 4) + " km", cx + 4, cy - ringRadius + 11);
            }
            ctx.beginPath();
            ctx.moveTo(cx - radius, cy);
            ctx.lineTo(cx + radius, cy);
            ctx.moveTo(cx, cy - radius);
            ctx.lineTo(cx, cy + radius);
            ctx.stroke();

            Icons.drawCompassRose(ctx, cx, cy, radius, "#33424f");

            if (!root.sim) {
                return;
            }

            const tracks = root.sim.tracks;
            for (let i = 0; i < tracks.length; ++i) {
                const track = tracks[i];
                const px = cx + track.xKm * scale;
                const py = cy + track.yKm * scale;
                const headingRad = track.headingDeg * Math.PI / 180;

                Icons.drawTrackGlyph(ctx, px, py, track.domain, headingRad, "#3ddc6f", 6);

                ctx.fillStyle = "#f2f4f8";
                ctx.font = "12px sans-serif";
                ctx.fillText(track.classification + " #" + track.id, px + 10, py - 8);
            }

            const dataLinkTracks = root.sim.dataLinkTracks;
            for (let j = 0; j < dataLinkTracks.length; ++j) {
                const dlTrack = dataLinkTracks[j];
                const dpx = cx + dlTrack.xKm * scale;
                const dpy = cy + dlTrack.yKm * scale;
                const dHeadingRad = dlTrack.headingDeg * Math.PI / 180;
                const dlColor = dlTrack.stale ? "#7a8a99" : "#39c0ff";

                ctx.globalAlpha = dlTrack.stale ? 0.55 : 1.0;
                Icons.drawTrackGlyph(ctx, dpx, dpy, dlTrack.domain, dHeadingRad, dlColor, 6);
                ctx.globalAlpha = 1.0;

                ctx.fillStyle = dlColor;
                ctx.font = "11px sans-serif";
                let label = dlTrack.classification + " #" + dlTrack.id + " (data link)";
                if (dlTrack.stale) {
                    label += " - STALE " + dlTrack.dataAgeSeconds.toFixed(0) + "s";
                    Icons.drawWarningTriangle(ctx, dpx - 14, dpy - 14, 6, "#e0a300");
                }
                ctx.fillText(label, dpx + 10, dpy + 14);
            }

            Icons.drawAntennaBars(ctx, width - 26, 26, 12, root.sim.commsQualityPercent, "#39c0ff");
            ctx.fillStyle = "#7a8a99";
            ctx.font = "10px sans-serif";
            ctx.fillText("DATA LINK", width - 74, 46);
        }
    }
}
