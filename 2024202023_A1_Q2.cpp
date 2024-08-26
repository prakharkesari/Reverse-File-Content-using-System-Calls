#include<unistd.h>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
//#include <sys/types.h>
#include <sys/stat.h>
#include <cstring>
//#include<string>

using namespace std;

#define STDERR_FILENNO 2
const size_t CHUNK = 10240;

void printPer(const char* fileName, string fileType){
    struct stat fileDetails;

    stat(fileName, &fileDetails);

    string msg1 = "User has READ permissions on "+ fileType+" : "+((fileDetails.st_mode & 400)? "Yes\n":"No\n");
    write(1, &msg1[0], strlen(&msg1[0]));

    string msg2 = "User has WRITE permissions on "+ fileType+" : "+((fileDetails.st_mode & 200)? "Yes\n":"No\n");
    write(1, &msg2[0], strlen(&msg2[0]));

    string msg3 = "User has EXECUTE permissions on "+ fileType+" : "+((fileDetails.st_mode & 100)? "Yes\n":"No\n");
    write(1, &msg3[0], strlen(&msg3[0]));

    string msg4 = "Group has READ permissions on "+ fileType+" : "+((fileDetails.st_mode & 040)? "Yes\n":"No\n");
    write(1, &msg4[0], strlen(&msg4[0]));

    string msg5 = "Group has WRITE permissions on "+ fileType+" : "+((fileDetails.st_mode & 020)? "Yes\n":"No\n");
    write(1, &msg5[0], strlen(&msg5[0]));

    string msg6= "Group has EXECUTE permissions on "+ fileType+" : "+((fileDetails.st_mode & 010)? "Yes\n":"No\n");
    write(1, &msg6[0], strlen(&msg6[0]));

    string msg7 = "Others has READ permissions on "+ fileType+" : "+((fileDetails.st_mode & 004)? "Yes\n":"No\n");
    write(1, &msg7[0], strlen(&msg7[0]));

    string msg8 = "Others has WRITE permissions on "+ fileType+" : "+((fileDetails.st_mode & 002)? "Yes\n":"No\n");
    write(1, &msg8[0], strlen(&msg8[0]));

    string msg9 = "Others has EXECUTE permissions on "+ fileType+" : "+((fileDetails.st_mode & 001)? "Yes\n":"No\n");
    write(1, &msg9[0], strlen(&msg9[0]));

    write(1, "\n", 1);

}

bool checkDirectory(const char* path){
    struct stat dirStat;

    if(stat(path, &dirStat) == -1){
        if(errno == ENOENT) return false;
        else write(STDERR_FILENNO, "Error checking directory\n", 25); 
        return false;
    }

    if(S_ISDIR(dirStat.st_mode)) return true;
    else return false;
}

bool isFileReversed(const char* input, const char* output){
    int in = open(input, O_RDONLY);
    int op = open(output, O_RDONLY);

    struct stat stat1, stat2;
    if(in < 0 || op < 0){
        write(STDERR_FILENNO, "Error opening files\n", 20);
        return false;
    }

    if(fstat(in, &stat1) == -1 || fstat(op, &stat2) == -1){
        write(STDERR_FILENNO, "Error getting file stats\n", 25);
        close(in);
        close(op);

        return false;
    }

    if(stat1.st_size != stat2.st_size) return false;

    off_t size = stat1.st_size;
    off_t rem = size;
    char buf1[CHUNK];
    char buf2[CHUNK];

    while(rem > 0){
        size_t chunk = (rem < CHUNK) ? rem : CHUNK;

        if(pread(in, buf1, chunk, size - rem) != chunk){
            write(STDERR_FILENNO, "Error reading input file\n", 25);
            close(in);
            close(op);
            return false;
        }

        if(pread(op, buf2, chunk, rem - chunk) != chunk) {
            write(STDERR_FILENNO, "Error reading output file\n", 26);
            close(in);
            close(op);
            return false;
        }

        for(size_t i=0; i<chunk; i++){
            if(buf1[i] != buf2[chunk - i - 1]) {
                close(in);
                close(op);
                return false;
            }
        }
        rem -=chunk;
    }
    close(in);
    close(op);
    return true;
}

bool checkFileSize(const char* input, const char* output){
    struct stat stat1, stat2;

    if(stat(input, &stat1) == -1){ 
        write(STDERR_FILENNO, "Unable to access input file\n", 28);
        return false;
    }

    if(stat(output, &stat2) == -1){ 
        write(STDERR_FILENNO, "Unable to access output file\n", 29);
        return false;
    }

    if(stat1.st_size == stat2.st_size) return true;
    else return false;
}

int main(int argc, char* argv[]){
    if(argc != 4){
        write(STDERR_FILENO, "Please pass correct arguements <inputFile> <outputFile> <Directory>\n", 68);
        return 1;
    }

    const char* inputFile = argv[1];
    const char* outputFile = argv[2];
    const char* directory = argv[3];

    write(1, "Both file size same: ", 21);
    if(checkFileSize(inputFile, outputFile)) write(1, "Yes\n", 4);
    else write(1, "No\n", 3);

    write(1, "Whether file contents are reversed in new file: ", 48);
    if(isFileReversed(inputFile, outputFile)) write(1, "Yes\n", 4);
    else write(1, "No\n", 3);

    write(1, "Directory is created :", 22);
    if(!checkDirectory(directory)) write(1, "No\n", 3);
    else write(1, "Yes\n", 4);

    write(1, "\n", 1);

    // struct stat pathStat;
    // write(1, "User has read permissions on new file: ",39);

    printPer(inputFile, "Old File");
    printPer(outputFile, "New File");
    printPer(directory, "Directory");


    return 0;
}