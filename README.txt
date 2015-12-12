Archi

Archi is file archiver (Huffman`s algorithm). It can compress and decompress different files: .txt, .pdf, .out, .exe etc.

Many thanks for the code of Github`s user ordian (https://github.com/ordian/huffman). His Huffman`s algorithm helped me to make this small program and to undestand algorithm. I learned a lot of new from his code :) 
Maybe my code will help someone also.

The main differences:
-new main.cpp (new interface)
-Huffman`s algorithm moved to the special class and became more safety
-small refactoring
-different small changes


Program usage:
------------------------------------------------------
Usage: archi [OPTION] [input-file] [output-file]
  -c, --compress
    compressing the file
  -d, --decompress
    decompressing the file
  -h, --help
    display this help and exit
------------------------------------------------------
For example:
	./archi.out -c test.txt output1.decoded
	./archi.out -d output1.decoded test2.txt
	Now you can open test2.txt! :)


Compilation for GNU/Linux:
g++ main.cpp huffman.cpp -o bin/archi.out


Tested on .txt, .pdf, .out files.
In folder /tested files you can find files which were tested (source files and archives).


Some issues:
-not working with files such as .mp4 and .zip


Comments:
There isn` t BYTE type in C++, so BYTE is "typedef unsigned char BYTE;" (see node.h).


My tiny TODO:
-add exception for files in main.cpp
-maybe to do default name for decoded file


Some good links for help:
http://www.codeproject.com/Articles/25088/Huffman-compression-class-in-C
https://github.com/fzakaria/Huffman-Compression/blob/master/compressor.cpp
http://code.activestate.com/recipes/577480-huffman-data-compression/
http://stackoverflow.com/questions/5570975/file-compression-with-c

Russian:
http://habrahabr.ru/post/144200/
https://www.youtube.com/watch?v=KNVPFVG49Oc
http://compression.ru/download/articles/huff/simakov_2002_huffcode.html
http://rain.ifmo.ru/cat/view.php/theory/data-compression/adaptive-huffman-2006
