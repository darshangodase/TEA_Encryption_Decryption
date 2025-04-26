# TEA File Encryption/Decryption Tool

A simple command-line utility for encrypting and decrypting files using the Tiny Encryption Algorithm (TEA).

## Overview

This program implements the TEA (Tiny Encryption Algorithm) for file encryption and decryption. TEA is a symmetric block cipher that operates on 64-bit blocks of data with a 128-bit key.


## Features

- File encryption using TEA algorithm
- File decryption with proper padding handling
- Command-line interface for easy integration into workflows
- Automatic handling of padding to ensure block alignment
- Support for files of any size
- Minimal memory footprint

## Requirements

- Windows operating system
- C++ runtime libraries (included in most Windows installations)

## Usage

### Basic Commands

To encrypt a file:
```
tea_crypt.exe encrypt <input_file> <output_file> <encryption_key>
```

To decrypt a file:
```
tea_crypt.exe decrypt <encrypted_file> <output_file> <encryption_key>
```

### Parameters

- `encrypt/decrypt`: Operation to perform
- `<input_file>`: Path to the file to encrypt/decrypt
- `<output_file>`: Path where the result will be saved
- `<encryption_key>`: Password or key for encryption/decryption (up to 16 characters)

### Examples

Encrypting a text file:
```
tea_crypt.exe encrypt "C:\Documents\confidential.txt" "C:\Documents\confidential.enc" "MySecretKey123"
```

Decrypting a previously encrypted file:
```
tea_crypt.exe decrypt "C:\Documents\confidential.enc" "C:\Documents\confidential_decrypted.txt" "MySecretKey123"
```

## Implementation Details

- Block Size: 8 bytes (64 bits)
- Key Size: 16 bytes (128 bits)
- Number of Rounds: 32
- Padding: PKCS#7-like padding scheme

## Security Considerations

Please note that while TEA provides basic encryption, it is not considered cryptographically secure by modern standards:

- The TEA algorithm has known vulnerabilities including related-key attacks
- This implementation doesn't provide authentication or integrity checking
- Keys are passed as command-line arguments, which may be visible in command history

For high-security applications, consider using more robust encryption tools like VeraCrypt, GnuPG, or similar professional encryption software.

## Building from Source

If you want to modify or compile the program yourself:

1. Navigate to the source code directory
2. Compile using your C++ compiler:
```
g++ -o tea_crypt.exe main.cpp
```

## Practical Uses

- Encrypting personal files and documents
- Simple file protection for non-critical data
- Educational purposes to understand encryption concepts
- Batch processing of multiple files for basic protection

## License

This software is provided as-is with no warranty. You are free to use and modify it for personal and educational purposes.

## Acknowledgments

Based on the Tiny Encryption Algorithm (TEA) developed by David Wheeler and Roger Needham at Cambridge Computer Laboratory.