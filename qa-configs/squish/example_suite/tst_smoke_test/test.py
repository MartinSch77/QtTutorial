# Illustrative Squish for Qt test script -- not executed by this repo's CI
# (no Squish license here). See ../README.md for how to adapt this to a
# real AUT and object map. Written against Squish's documented Python API
# shape; not verified against an installed Squish, since none is available
# in this environment -- treat the exact object names below as placeholders
# to replace with real ones recorded via Squish's Object Spy.


def main():
    startApplication("widgets_basics")
    snooze(1)

    # Real object names come from Squish's Object Spy against the running
    # AUT; these are illustrative placeholders.
    clickButton(waitForObject(":widgets_basics.Add row_QPushButton"))
    test.compare(
        waitForObjectExists(":widgets_basics.tableView_QTableView").model().rowCount(),
        4,
    )

    sendEvent("QCloseEvent", waitForObject(":widgets_basics_MainWindow"))
