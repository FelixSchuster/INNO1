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

The following Entries will be removed:

| Key | Value |
| :--- | :--- |
| Vollständiger Name | User Admin |
| Komponente | Forum |
| Komponente | System |
| Komponente | Gruppenwahl |
| Komponente | Zoom Meeting |
| Ereignisname | \*angezeigt\* |
| Ereignisname | \*heruntergeladen\* |