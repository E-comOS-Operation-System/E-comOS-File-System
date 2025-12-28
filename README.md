# E-comOS File Systems

**A Modern, Object-Oriented File System Family for E-comOS**
![License: AGPL v3](https://img.shields.io/badge/License-AGPL%20v3-blue.svg)
![Version](https://img.shields.io/badge/version-1.0.0-blue.svg)

## Overview

E-comOS File Systems is a family of modern, object-oriented file systems designed specifically for the E-comOS operating system. This project provides three distinct file system implementations tailored to different storage scenarios, all built on a unified object-oriented foundation.

## File System Family

### 1. NEFS (New! E-comOS File System)

**Target**: High-capacity storage devices (256GiB and above)

**Use Case**: Modern systems with large storage requirements

**Key Features**:



* No practical storage limits

* Advanced performance optimizations

* Comprehensive data integrity features

* Scalable architecture for future expansion

### 2. EFS (E-comOS File System)

**Target**: Smaller storage devices (256GiB and below)

**Use Case**: Legacy systems, embedded devices, and resource-constrained environments

**Key Features**:



* Lightweight design with minimal resource footprint

* Optimized for small file storage

* Fast mount and boot times

* Simplified maintenance

**Limitations**:



* Maximum folder size: 20GiB

* Path length limit: 1000 characters (excluding separator '>')

* File size: No practical limit

### 3. WM2UD (When Memory Unsupport Use Disk)

**Target**: Swap partitions

**Use Case**: Memory extension for systems with limited RAM

**Key Features**:



* Optimized for fast memory-disk data exchange

* Low-latency design

* Efficient memory page management

* Optional data compression

## Core Design Principles

### Object-Oriented Foundation

All file systems are built on a simple yet powerful object model:



* **File Object**: Represents files with properties and methods

* **Folder Object**: Represents directories that can contain other objects

### Unified Interface

Consistent API across all file systems:



```
// Core object operations

FileObject\* create\_file(const char\* path, const void\* data, size\_t size);

FolderObject\* create\_folder(const char\* path);

int delete\_object(const char\* path);

int rename\_object(const char\* old\_path, const char\* new\_path);

// Object access

FileObject\* get\_file(const char\* path);

FolderObject\* get\_folder(const char\* path);

List\* list\_objects(const char\* path);
```

### Modern Features



* **Metadata Integrity**: Built-in checksums and validation

* **Extensible Attributes**: Custom metadata support

* **Unified Path Syntax**: Using '>' as path separator (e.g., root>home>user)

* **Cross-Compatibility**: Data migration tools between file systems

## Technical Specifications

### NEFS Technical Details



* **Block Size**: Configurable (4KB, 8KB, 16KB, 32KB, 64KB)

* **Addressing**: Three-level indirect addressing

* **Maximum File Size**: Effectively unlimited (theoretical EB scale)

* **Journaling**: Yes (for data integrity)

* **Checksums**: SHA-256 for metadata, CRC32 for data blocks

### EFS Technical Details



* **Block Size**: Fixed at 4KB

* **Addressing**: Single-level indirect addressing

* **Maximum Storage**: 256GiB

* **Journaling**: No (to reduce overhead)

* **Checksums**: CRC32 for metadata and data

### WM2UD Technical Details



* **Block Size**: Matches system page size (typically 4KB)

* **Page Management**: LRU-based page replacement

* **Lookup Mechanism**: Hash table for fast page 查找

* **Compression**: Optional LZ4 compression

* **Encryption**: Optional AES-256 encryption

## Implementation Status

### Current Progress

Core object model design

NEFS format specification

EFS format specification

WM2UD format specification

Reference implementation (in progress)

Test suite development

### Roadmap



1. Complete reference implementation (Q1 2025)

2. Performance optimization (Q2 2025)

3. Data migration tools (Q3 2025)

4. File system utilities (Q4 2025)

## Building and Installation

### Prerequisites



* E-comOS development environment

* ECC compiler (Version 1.0 or later)

* Standard build tools

### Building from Source



```
\# Clone the repository

git clone https://github.com/E-comOS-Operation-System/file-systems.git

cd file-systems

\# Build the file system drivers

make all

\# Install the drivers

sudo make install
```

### Usage Examples

#### Creating a NEFS File System



```
\# Format a device with NEFS

mkfs.nefs /dev/sda1

\# Mount the file system

mount -t nefs /dev/sda1 /mnt
```

#### Creating an EFS File System



```
\# Format a device with EFS

mkfs.efs /dev/sdb1

\# Mount the file system

mount -t efs /dev/sdb1 /home
```

#### Configuring WM2UD



```
\# Format a swap partition with WM2UD

mkswap.wm2ud /dev/sdc1

\# Activate the swap partition

swapon /dev/sdc1
```

## Contributing

We welcome contributions to the E-comOS File Systems project! Please read our [Contributing Guidelines](CONTRIBUTING.md) before submitting pull requests.

### Ways to Contribute



* Code development

* Documentation improvement

* Testing and bug reporting

* Feature suggestions

### Development Guidelines



* Follow the existing code style

* Write comprehensive tests

* Document all public APIs

* Create detailed commit messages

## License

This project is licensed under the GNU Affero General Public License v3.0 - see the [LICENSE](LICENSE) file for details.

## Acknowledgments



* Inspired by modern file systems like ZFS, Btrfs, and ext4

* Built for the E-comOS operating system

* Designed with simplicity and performance in mind

## Contact

For questions, suggestions, or bug reports, please:



* Open an issue on GitHub

* Join our [Discord community](https://discord.gg/ecomos)

* Email us at [dev@e-comos.org](mailto:dev@e-comos.org)



***

*E-comOS File Systems - Making storage simple, efficient, and future-proof.*

