//lab4
//File archiver: "archi"

#include <iostream>
#include <cstdlib>
#include <cstring> //strcmp();

#include "huffman.h"
using namespace std;

Size_t filesize(const char* filename)
{
    ifstream in(filename, ifstream::in | ifstream::binary);
    in.seekg(0, ifstream::end);
    return in.tellg(); //tellg() -> return number of elements in this case
}

int main(int argc, char **argv)
{
    Huffman MyHuffman;
    const char *input = NULL, *output = NULL;
    ifstream  in;
    ofstream out;

	//А как правильно распарсить параметры?
    //или сделать, чтобы можно было так задавать параметры: -cd ???
    //Как тогда? Завести отдельный файл?
    //Как вообще? Нормальный файл?

    //ОПТИМИЗИРОВАТЬ:
    if ( argv[1]!=NULL && strcmp(argv[1],"-h")==0 ) {
        cout << "Usage: archi [OPTION] [input-file] [output-file]" << endl;
        cout << "  -c, --compress" << endl;
        cout << "    compressing the file" << endl;
        cout << "  -d, --decompress" << endl;
        cout << "    decompressing the file" << endl;
        cout << "  -h, --help" << endl;
        cout << "    display this help and exit" << endl;

        cout << "\nThe program uses Huffman`s algorithm for compressing and decompressing." << endl;
        //cout << "A lot of thanks for help of ... code." << endl;
        cout << "Report archi bugs to AlexanderDydychkin@yandex.ru" << endl;
        //cout << "Author: Alexander Dydychkin" << endl;

        return 0;
    }
    if (argv[1]==NULL || argv[2]==NULL || argv[3]==NULL) {
		//cout << "archi: missing operand" << endl;
		cout << "archi: invalid input" << endl;
    	cout << "Try 'archi -h' for more information." << endl;
    	return 0;
	}

    //FILES work
    //EXCEPTION???  
    if ( argv[2]!=NULL )
        input=argv[2];
    if ( argv[3]!=NULL )
        output=argv[3];
    
    Size_t size = filesize(input);

    if (input) {
        in.open(input, ios::in | ios::binary);
      
        if (!in) {
            cout << "Can't open input file " 
            << input 
            << endl;
            return 1;
        }
    }
    if (output) {
        out.open(output, ios::out | ios::binary);
      
        if (!out) {
            cout << "Can't open output file " 
            << output 
            << endl;
            return 1;
        }
    }
    //End

	if ( strcmp(argv[1],"-c")==0 ) {
    //NEW if ( strcmp(argv[1],"-c" || strcmp(argv[1],"--compress")==0 ) {
    	cout << "Compressing..." << endl;
        MyHuffman.huffmanEncodeFile(in, out, size);
        cout << "Compressed." << endl;
    	//что делать с output??? Приравнивать к input, но расширение другое?
    }
    if ( strcmp(argv[1],"-d")==0 ) {
    //NEW if ( strcmp(argv[1],"-c" || strcmp(argv[1],"--decompress")==0 ) {
        cout << "Decompressing..." << endl;
        MyHuffman.huffmanDecodeFile(in, out);
        cout << "Decompressed." << endl;
        //что делать с output??? Приравнивать к input, но расширение другое?
    }

    return 0;
}
