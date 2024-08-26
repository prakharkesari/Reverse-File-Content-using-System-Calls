This folder contains 2 .cpp file 
    1. 2024202023_A1_Q1.cpp - This program is used to reverse the content of the txt file.
    2. 2024202023_A1_Q2.cpp - This program is used to check if the new file is the reverse of old file. Also it checks all the permission for read, write and execute for Users, Groups and Others.


1. To execute the first program. First compile the program and execute it. Give the arguements as <file_name> <flag_value> <start index and end index if flag = 1>.

    1. When flag=0, the entire content of the file gets reversed and the reversed content is saved in the new file named "0_<file_name>".

    2. When flag=1, the content is reversed from start of the file to start index, and end index to end of the file. The string between the start index and end index remains unchanged. The new file created is named as 1_<file_name>.

    3. The above program can be executed for large file size(>1GB). The progress percentage is printed on screen to show the current percent of the file that is reversed. The output file will have the same size as input file.

2. To execute the second program. First compile the program and execute it. Give the arguements as <input_file_name> <output_file_name> <PATH>.     
    1. This will check if the reversed file that is created from first program is actually reversed of the input file.

    2. It will also check if the file size of reversed file is same as input file size or not.

    3. All the Read, Write and Execute permissions are also diaplayed for each : User, Groups and Others.