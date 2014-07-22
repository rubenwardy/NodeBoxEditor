NBE File Format
===============

.nbe files are a uni

The first five bytes tell the parser that it is an NBE file.
"NBEFP"

The next byte is a number between 1 and 255 which indicates how many files there are.

Next up is the file table. It is in this format:

| Length   | Description                             |
| 50 bytes | Name of the file                        |
|  4 bytes | Start position of file data, x bytes in |
|  4 bytes | Size of the file data, x bytes          |

Then comes the file data, one after another.
