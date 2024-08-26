#include <iostream>
#include <fcntl.h>
#include <unistd.h>

using namespace std;

const size_t CHUNK_SIZE = 10240;

void reverseAndWrite(int fd, char* buffer, ssize_t size);
void copyChunk(int inputFd, int outputFd, off_t start, off_t end, size_t chunkSize);
void reverseRange(int inputFd, int outputFd, off_t start, off_t end, size_t chunkSize);
void displayProgress(off_t totalProcessed, off_t fileSize);

int main(int argc, char* argv[]) {
    if (argc < 3) {
        write(1, "Please give proper arguement format <input_file> <flag> [x] [y]\n", 65);
        return 1;
    }

    const char* inputFilename = argv[1];
    int flag = atoi(argv[2]);
    int x = 0, y = 0;

    if (flag == 1) {
        if (argc < 5) {
            write(1, "Flag 1 requires two additional parameters: x and y\n", 52);
            return 1;
        }
        x = atoi(argv[3]);
        y = atoi(argv[4]);

        if (x >= y) {
            write(1, "Invalid input format, X should be less than y\n", 47);
            return 1;
        }
    } else if (flag == 0 && argc > 3) {
        write(1, "Flag 0 does not require additional x and y arguements\n", 55);
        return 1;
    }

    string outputFilename = to_string(flag) + "_" + string(inputFilename);

    int inputFd = open(inputFilename, O_RDONLY);
    if (inputFd < 0) {
        write(1, "Unable to open input file\n", 27);
        return 1;
    }

    off_t fileSize = lseek(inputFd, 0, SEEK_END);
    if (fileSize == -1) {
        write(1, "Error determining the input file\n", 34);
        close(inputFd);
        return 1;
    }

    int outputFd = open(outputFilename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0777);
    if (outputFd < 0) {
        write(1, "Unable to open output file\n", 28);
        close(inputFd);
        return 1;
    }

    if (flag == 0) reverseRange(inputFd, outputFd, 0, fileSize, CHUNK_SIZE);
    
    else if (flag == 1) {
        reverseRange(inputFd, outputFd, 0, x, CHUNK_SIZE);
        copyChunk(inputFd, outputFd, x, y+1, CHUNK_SIZE);
        reverseRange(inputFd, outputFd, y+1, fileSize, CHUNK_SIZE);
    } 
    
    else {
        write(1, "Cannot use flag value other than 0 or 1\n", 41);
        close(inputFd);
        close(outputFd);
        return 1;
    }

    close(inputFd);
    close(outputFd);

    return 0;
}

void reverseAndWrite(int fd, char* buffer, ssize_t size) {
    for (int i = 0; i < size / 2; ++i) {
        char temp = buffer[i];
        buffer[i] = buffer[size - i - 1];
        buffer[size - i - 1] = temp;

    }
    if (write(fd, buffer, size) != size) write(1, "Error writing to output file\n", 30);
    
}

void copyChunk(int inputFd, int outputFd, off_t start, off_t end, size_t chunkSize) {
    char buffer[CHUNK_SIZE];
    ssize_t bytesRead;

    for (int offset = start; offset < end; offset += chunkSize) {
        size_t actualChunkSize = (offset + chunkSize <= end) ? chunkSize : end - offset;

        if (lseek(inputFd, offset, SEEK_SET) == -1) {
            write(1, "Error seeking in input file\n", 29);
            return;
        }

        bytesRead = read(inputFd, buffer, actualChunkSize);
        if (bytesRead != actualChunkSize) {
            write(1, "Error reading from input file\n", 31);
            return;
        }

        if (write(outputFd, buffer, actualChunkSize) != actualChunkSize) {
            write(1, "Error writing the output file\n", 31);
            return;
        }
    }
}

void reverseRange(int inputFd, int outputFd, off_t start, off_t end, size_t chunkSize) {
    char buffer[CHUNK_SIZE];
    ssize_t bytesRead;
    off_t totalBytesProcessed = 0;

    off_t remainingBytes = end - start;

    while (remainingBytes > 0) {
        size_t actualChunkSize = (remainingBytes < chunkSize)?remainingBytes: chunkSize;

        if (lseek(inputFd, start + remainingBytes - actualChunkSize, SEEK_SET) == -1) {
            write(1, "Error seeking input file\n", 26);
            return;
        }

        bytesRead = read(inputFd, buffer, actualChunkSize);
        if (bytesRead != actualChunkSize) {
            write(1, "Error reading from inpur file\n", 31);
            return;
        }

        reverseAndWrite(outputFd, buffer, actualChunkSize);

        remainingBytes -= actualChunkSize;
        totalBytesProcessed += actualChunkSize;

        displayProgress(totalBytesProcessed, end - start);
    }

    displayProgress(end - start, end - start);
}


void displayProgress(off_t totalProcessed, off_t fileSize) {
    int progress = static_cast<int>((totalProcessed * 100) / fileSize);
    printf("\rProgress: %d%%", progress);
    fflush(stdout);
}
