// SPDX-License-Identifier: MIT
#include "StyleCatalog.h"

#include <QTest>
#include <algorithm>

using namespace qttutorial::quick_controls_styling;

class TestStyleCatalog : public QObject {
    Q_OBJECT
private slots:
    void includesBasicStyle()
    {
        const auto& styles = styleCatalogue();
        const auto it = std::ranges::find(styles, "Basic", &StyleInfo::name);
        QVERIFY(it != styles.end());
    }

    void everyEntryHasNonEmptyFields()
    {
        for (const auto& style : styleCatalogue()) {
            QVERIFY(!style.name.empty());
            QVERIFY(!style.minimumQtVersion.empty());
            QVERIFY(!style.note.empty());
        }
    }
};

QTEST_APPLESS_MAIN(TestStyleCatalog)
#include "test_style_catalog.moc"
