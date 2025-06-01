# Synonym Dictionary CLI

A command-line application for managing words and their synonyms, built in C. This program allows users to add words, add synonyms, view synonyms, save the dictionary to a file, load it from a file, and view a history of actions.

## Table of Contents

* [Features](#features)
* [Technologies Used](#technologies-used)
* [File Structure](#file-structure)
* [Getting Started](#getting-started)
  * [1. Obtain the Source Code](#1-obtain-the-source-code)
  * [2. prerequisites](#2-prerequisites)
  * [3. Compilation](#3-compilation)
  * [4. Running the application](#4-running-the-application)
* [Usage](#usage)
* [Data Structures](#data-structures)

## Features

* **Add Words:** Introduce new words into the dictionary.
* **Add Synonyms:** Link synonyms to existing words.
* **View Synonyms:** Display all synonyms for a specific word.
* **View All Words:** List all unique words currently stored in the dictionary.
* **Persistent Storage:**
  * Save the current dictionary (words and their synonyms) to a file.
    * Load a previously saved dictionary from a file.
* **Action History:** Keeps a log of actions performed (though specific history functions need to be implemented in `history.c`).
* **Undo Functionality (Implied):** Data structure `UndoAction` suggests a planned or existing feature to revert actions.
* **Text Normalization:** Utility functions to trim whitespace and convert case (lowercase, capitalize).
* **Menu-Driven Interface:** Easy-to-use menu for interacting with the application.
* **Hash Table Implementation:** Uses a hash table (`HASH_SIZE 503`) for efficient word lookup and storage, with chaining for collision resolution.

## Technologies Used

* C Programming Language
* GCC (GNU Compiler Collection) for compilation (as per build instructions)

## File Structure

The project consists of the following main files:

* `main.c`: The main driver of the program, likely containing the `main()` function and menu logic.
* `synonym.c`: Contains functions related to managing words and synonyms, file operations, and hash table logic.
* `history.c`: (To be implemented) Will contain functions related to managing the action history.
* `functions.h`: The header file containing all structure definitions, global variable declarations, and function prototypes.

## Getting Started

This section outlines the steps to obtain the project's source code, set up the necessary prerequisites, compile the code, and run the application.

### 1. Obtain the Source Code

There are two primary methods to get the project files onto your local machine:

#### Option A: Clone the Repository (Recommended if using Git)

   If the project is hosted on a Git platform (such as GitHub, GitLab, or Bitbucket), cloning the repository is the preferred method. This allows you to easily pull updates in the future.

   1. Open your terminal or command prompt.
   2. Execute the `git clone` command followed by the repository's URL:

      ```bash
      git clone [URL_OF_YOUR_REPOSITORY]
      ```

      Replace `[URL_OF_YOUR_REPOSITORY]` with the actual HTTPS or SSH URL provided by the Git hosting service.
   3. Navigate into the newly created project directory:

      ```bash
      cd [PROJECT_DIRECTORY_NAME]
      ```

      Replace `[PROJECT_DIRECTORY_NAME]` with the name of the folder created by the clone operation (this is typically the repository's name).

#### Option B: Download the Source Code Archive (ZIP file)

   If the project provides a direct download option (often as a `.zip` file, common on GitHub by clicking "Code" then "Download ZIP"), or if you prefer not to use Git:

   1. Download the source code archive file to your computer.
   2. The contents of the ZIP archive should be extracted to an appropriate directory on the local file system.
   3. Open your terminal or command prompt and navigate into the extracted project directory.

### 2. Prerequisites

* A C compiler, preferably GCC (GNU Compiler Collection).
  * **Linux:** Usually pre-installed or can be installed via package manager (e.g., `sudo apt install gcc`).
  * **macOS:** Install Xcode Command Line Tools (`xcode-select --install`).
  * **Windows:** Install MinGW-w64 or use WSL (Windows Subsystem for Linux) with GCC.

### 3. Compilation

1. Open your terminal or command prompt.
2. Ensure you are in the project's root directory (where `main.c`, `synonym.c`, `history.c`, and `functions.h` are located).
3. Compile the source files using the command provided in your header file:

    ```bash
    gcc history.c synonym.c main.c -o program
    ```

    This command links all the C files and creates an executable named `program` (or `program.exe` on Windows).

### 4. Running the Application

Once compiled successfully, an executable file named `program` will be created.

* **On Linux/macOS:**

    ```bash
    ./program
    ```

* **On Windows:**

    ```bash
    program.exe
    ```

    or from PowerShell:

    ```bash
    .\program.exe
    ```

## Usage

Upon running the application, you will be presented with a menu. Follow the on-screen prompts to:

* Add new words.
* Add synonyms to existing words.
* Search for a word and display its synonyms.
* Display all words in the dictionary.
* Save the current dictionary to a file (e.g., `dictionary.txt`).
* Load an existing dictionary from a file.
* (Potentially) View history or undo actions, depending on implementation status. (ongoing)

## Data Structures

The program utilizes several custom data structures:

* **`SynonymNode`**: Represents a single synonym in a linked list.
  * `char* word`: Stores the synonym string.
  * `struct SynonymNode* next`: Points to the next synonym in the list for a given word.

* **`WordEntry`**: Represents an entry in the hash table for a primary word.
  * `char* word`: Stores the primary word string.
  * `SynonymNode* synonyms`: A pointer to the head of a linked list of its synonyms.
  * `struct WordEntry* next`: Points to the next `WordEntry` in case of a hash collision (separate chaining).

* **`UndoAction`**: Designed for an undo stack, likely to store information needed to revert an action.
  * `char* word`: The primary word involved in the action.
  * `char* synonym`: The synonym involved, if applicable.
  * `struct UndoAction* next`: Points to the next action in the undo stack.

* **`HistoryNode`**: Represents an entry in the action history log.
  * `char* action`: Describes the action performed (e.g., "Added word", "Added synonym").
  * `char* time`: Stores the timestamp of when the action occurred. *(Note: The `getCurrentTimestamp` function from previous discussions would be relevant here if `history.c` uses it).*
  * `struct HistoryNode* next`: Points to the next history entry in a linked list.

* **Hash Table**: An array of `WordEntry*` pointers (implicitly, not directly defined as a struct but managed via `HASH_SIZE`). The `hash()` function determines the index for a given word.

---
