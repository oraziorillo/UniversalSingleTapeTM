# UniversalSingleTapeTM

This is a implementation in C of an universal single tape turing machine. The program takes as input a file that contains the description of a single tape Turing machine and some test strings. The program will simulate such a TM and will produce the corresponding output for each of the test strings furnished. 

A full description of the homework is provided in the slides "progetto-API-17-18.pdf", though the english version is not available. This project is the first real program I wrote during my bachelor studies in Milan and for this reason, I am very attached to it.  

To compile the program, use the command: ```gcc main.c -o main```.

Some input and (correct) output examples are given in the ```input_output_examples``` folder. To test the code on one of them use the command:

```cat input_output_examples/input_pub_*.txt | ./main > my_output.txt```

where you should substitute * with the number of the file to test. This command will write the output in a file called "my_output.txt". You can change the name of the output file in the command above. After that, you can check the correctness of the results comparing the produced output with the file "output_pub_\*.txt".

You can produce your own input description and test strings, but be careful to keep the right format.
