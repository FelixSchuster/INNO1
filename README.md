# CPP_Parser
A C++ parser for moodle-logfiles containing common HTML/XHTML entities\
Prepares data to be used for celonis process mining

## Usage
CLI:
```
cpp_parser.exe <logfile>
```

## Features
The following HTML/XHTML entities are translated to their actual values:

| Key | Value |
| :--- | :--- |
| &quot, | \ |
| &apos, | " |
| &amp, | & |
| &gt, | > |
| &lt, | < |
| &lt, | / |

Additionally the following key is replaced so celonis can properly interpret non-existing entries:

| Key | Value |
| :--- | :--- |
| -, | 0, |

The following entries will be removed from the filtered file:

| Key | Value |
| :--- | :--- |
| Vollständiger Name | User Admin |
| Ereigniskontext | \*(Kopie)\* |
| Komponente | Forum |
| Komponente | System |
| Komponente | Gruppenwahl |
| Komponente | Zoom Meeting |
| Komponente | Verzeichnis |
| Ereignisname | \*angezeigt\* |
| Ereignisname | \*heruntergeladen\* |

## Bugs
None that we are aware of.

## TODO
Check for valid number of rows in headline.