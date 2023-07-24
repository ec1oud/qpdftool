Not sure what this will turn into, but I'm trying to make some sort of utility
using [Qt PDF](https://doc-snapshots.qt.io/qt6-dev/qtpdf-index.html) in Qt 6,
as a dogfooding exercise and/or as the need arises for something I can't find
other tools to do.

Currently it looks for PDF files recursively in the current directory with
certain features:

- links
- non-numeric page labels (Roman numerals, prefixed numbers like A1, and such)
- page labels that begin or end with whitespace (untrimmed)
