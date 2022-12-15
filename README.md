# CPP_Parser
C++ Parser for Moodle-Logfiles containing common HTML/XHTML Entities\
Prepares Data to be used for Celonis Process Mining

## Usage
CLI:
```
cpp_parser.exe <logfile>
```

## Features
The following HTML/XHTML Entities are translated to their actual Values:

| Key | Value |
| :--- | :--- |
| &quot, | \ |
| &apos, | " |
| &amp, | & |
| &gt, | > |
| &lt, | < |
| &lt, | / |

Additionally the following Key is replaced so Celonis can properly interpret non-existing Entries:

| Key | Value |
| -, | 0, |

The following Entries will be removed:

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
The Parser currently crashes when parsing an already parsed File.

## TODO
Fix the Bugs.