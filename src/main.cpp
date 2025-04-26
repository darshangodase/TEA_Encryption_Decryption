
#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <vector>
#include <windows.h>
#include <algorithm> 

#define KEY_SIZE 16
#define ROUNDS 32
#define TEA_BLOCK_SIZE 8

void tea_encrypt(unsigned char *data, const unsigned char *key) {
    unsigned int i;
    unsigned int delta = 0x9e3779b9;
    unsigned int sum = 0;
    unsigned int v0 = *(unsigned int *)data;
    unsigned int v1 = *(unsigned int *)(data + 4);

    for (i = 0; i < ROUNDS; i++) {
        v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + ((unsigned int *)key)[sum & 3]);
        sum += delta;
        v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + ((unsigned int *)key)[(sum >> 11) & 3]);
    }

    *(unsigned int *)data = v0;
    *(unsigned int *)(data + 4) = v1;
}

void tea_decrypt(unsigned char *data, const unsigned char *key) {
    unsigned int i;
    unsigned int delta = 0x9e3779b9;
    unsigned int sum = delta * ROUNDS;
    unsigned int v0 = *(unsigned int *)data;
    unsigned int v1 = *(unsigned int *)(data + 4);

    for (i = 0; i < ROUNDS; i++) {
        v1 -= (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + ((unsigned int *)key)[(sum >> 11) & 3]);
        sum -= delta;
        v0 -= (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + ((unsigned int *)key)[sum & 3]);
    }

    *(unsigned int *)data = v0;
    *(unsigned int *)(data + 4) = v1;
}

bool encryptFile(const std::string& inputFile, const std::string& outputFile, const std::string& teaKey) {
    std::ifstream inFile(inputFile, std::ios::binary);
    if (!inFile) {
        std::cerr << "Error opening input file: " << inputFile << std::endl;
        return false;
    }

    std::ofstream outFile(outputFile, std::ios::binary);
    if (!outFile) {
        std::cerr << "Error opening output file: " << outputFile << std::endl;
        inFile.close();
        return false;
    }

    inFile.seekg(0, std::ios::end);
    size_t fileSize = inFile.tellg();
    inFile.seekg(0, std::ios::beg);

    size_t paddingSize = TEA_BLOCK_SIZE - (fileSize % TEA_BLOCK_SIZE);
    if (paddingSize == TEA_BLOCK_SIZE) {
        paddingSize = 0;
    }

    size_t paddedSize = fileSize + paddingSize;
    std::vector<unsigned char> buffer(paddedSize);

    inFile.read(reinterpret_cast<char*>(buffer.data()), fileSize);
    
    if (paddingSize > 0) {
        for (size_t i = fileSize; i < paddedSize; i++) {
            buffer[i] = static_cast<unsigned char>(paddingSize);
        }
    }

    unsigned char key[KEY_SIZE];
    size_t keyLength = std::min(KEY_SIZE, static_cast<int>(teaKey.size()));
    memcpy(key, teaKey.c_str(), keyLength);
    
    if (teaKey.size() < KEY_SIZE) {
        memset(key + teaKey.size(), 0, KEY_SIZE - teaKey.size());
    }

    for (size_t i = 0; i < paddedSize; i += TEA_BLOCK_SIZE) {
        tea_encrypt(&buffer[i], key);
    }

    outFile.write(reinterpret_cast<char*>(buffer.data()), paddedSize);

    inFile.close();
    outFile.close();

    std::cout << "File encrypted successfully to " << outputFile << std::endl;
    std::cout << "Original size: " << fileSize << " bytes, Padded size: " << paddedSize << " bytes" << std::endl;
    
    return true;
}

bool decryptFile(const std::string& inputFile, const std::string& outputFile, const std::string& teaKey) {
    std::ifstream inFile(inputFile, std::ios::binary);
    if (!inFile) {
        std::cerr << "Error opening input file: " << inputFile << std::endl;
        return false;
    }

    std::ofstream outFile(outputFile, std::ios::binary);
    if (!outFile) {
        std::cerr << "Error opening output file: " << outputFile << std::endl;
        inFile.close();
        return false;
    }

    inFile.seekg(0, std::ios::end);
    size_t fileSize = inFile.tellg();
    inFile.seekg(0, std::ios::beg);

    if (fileSize % TEA_BLOCK_SIZE != 0 || fileSize == 0) {
        std::cerr << "Invalid encrypted file size: " << fileSize << " bytes" << std::endl;
        inFile.close();
        outFile.close();
        return false;
    }

    std::vector<unsigned char> buffer(fileSize);
    inFile.read(reinterpret_cast<char*>(buffer.data()), fileSize);

    unsigned char key[KEY_SIZE];
    size_t keyLength = std::min(KEY_SIZE, static_cast<int>(teaKey.size()));
    memcpy(key, teaKey.c_str(), keyLength);
    
    if (teaKey.size() < KEY_SIZE) {
        memset(key + teaKey.size(), 0, KEY_SIZE - teaKey.size());
    }

    for (size_t i = 0; i < fileSize; i += TEA_BLOCK_SIZE) {
        tea_decrypt(&buffer[i], key);
    }

    size_t paddingSize = buffer[fileSize - 1];
    
    if (paddingSize > TEA_BLOCK_SIZE) {
        std::cerr << "Invalid padding size: " << paddingSize << std::endl;
        inFile.close();
        outFile.close();
        return false;
    }
    
    for (size_t i = fileSize - paddingSize; i < fileSize; i++) {
        if (buffer[i] != paddingSize) {
            std::cerr << "Invalid padding detected" << std::endl;
            inFile.close();
            outFile.close();
            return false;
        }
    }

    outFile.write(reinterpret_cast<char*>(buffer.data()), fileSize - paddingSize);

    inFile.close();
    outFile.close();

    std::cout << "File decrypted successfully to " << outputFile << std::endl;
    std::cout << "Encrypted size: " << fileSize << " bytes, Original size: " << (fileSize - paddingSize) << " bytes" << std::endl;
    
    return true;
}

void displayUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [encrypt|decrypt] <input_file> <output_file> <key>" << std::endl;
    std::cout << "  encrypt - Encrypt the input file and save to output file" << std::endl;
    std::cout << "  decrypt - Decrypt the input file and save to output file" << std::endl;
    std::cout << "  input_file - Path to the input file" << std::endl;
    std::cout << "  output_file - Path to the output file" << std::endl;
    std::cout << "  key - Encryption/decryption key (up to 16 characters)" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 5) {
        displayUsage(argv[0]);
        
        return 1;
    }

    std::string operation = argv[1];
    std::string inputFile = argv[2];
    std::string outputFile = argv[3];
    std::string key = argv[4];

    if (operation == "encrypt") {
        if (encryptFile(inputFile, outputFile, key)) {
            return 0;
        } else {
            return 1;
        }
    } else if (operation == "decrypt") {
        if (decryptFile(inputFile, outputFile, key)) {
            return 0;
        } else {
            return 1;
        }
    } else {
        std::cerr << "Invalid operation: " << operation << std::endl;
        displayUsage(argv[0]);
        return 1;
    }
}