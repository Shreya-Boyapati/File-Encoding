Huffman Encoding
====

The Huffman Encoding application is a program that can compress and decompress files and I/O streams for data compression. The process uses bitstream operations to convert raw text into binary format and vice-versa.

Commands
---
- 1:  Build character frequency map
- 2:  Build encoding tree
- 3:  Build encoding map
- 4:  Encode data
- 5:  Decode data
- C:  Compress file
- D:  Decompress file
- B: View binary file
- T: View text file
  
Installing, Compiling and Running
---
- Clone and open the project directory from the command line or chosen IDE
- Build and run the project using the commands ```rm -f program.exe
	g++ -g -std=c++11 -Wall main.cpp hashmap.cpp -I '.guides/secure/' -o program.exe```
- Run with ```./program.exe```
- Enter chosen command and filename if relevant. Contents of file process will be printed on the terminal.

The output will look like the following.

Text file:

```
Enter choice: T
Enter filename: example.txt
example.txt
ab ab cab
```

Binary file:

```
Enter choice: B
Enter filename: example.txt.huf
example.txt.huf
11011110 10011100 11101100 01011100 11001100 00110100 00000100 10011100 
00011100 01011100 11001100 00110100 00000100 01001100 10101100 01101100 
01011100 10001100 00110100 00000100 11001100 01001100 01011100 01001100 
00110100 00000100 10011100 10011100 01011100 10001100 10111110 11100011 
10000111 11001000
```
