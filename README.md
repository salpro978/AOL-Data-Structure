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

* **Word Management:**
  * Add new words to dictionary.
  * Delete existing words from the dictionary.
* **Synonym Management:**
  * Add synonyms to existing words.
  * View synonyms for a specific word.
  * Delete specific synonyms from a word.
* **Viewing Options::**
  * View all words along with their synonyms.
  * View all word entries (sorted alphabetically using a Trie).
* **History Tracking:**
  * Automatically records actions like adding words, adding synonyms, and deleting entries.
  * View the action history with timestamps.
* **Data Persistence:**
  * Saves the dictionary (words and synonyms) to `synonymList.txt` upon exit.
  * Saves the action history to `history.txt` upon exit.
  * Loads existing dictionary and history data on startup.
* **Input Processing:**
  * Trims whitespace from user input.
  * Normalizes word casing for consistent storage and lookup (main words stored capitalized, Trie uses lowercase for sorting/display).
* **Memory Management:**
  * Properly frees all alocated memory for the hash table, trie, and history log on exit to prevent leaks.

## Technologies Used

* C Programming Language
* GCC (GNU Compiler Collection) for compilation (as per build instructions)

## File Structure

The project consists of the following main files:

* `main.c`: The main driver of the program. It initializes data structures, loads data from files, calls the main menu, and handles memory cleanup before exiting.
* `synonym.c`: Contains the core application logic, including management of the hash table and Trie, all word/synonym functions, the main menu loop, and file I/O for synonyms.
* `history.c`: Contains all functions related to managing the action history, including adding new entries, printing the history, and saving/loading the history log from a file.
* `dictionary.h`: The central header file containing all structure definitions (`WordEntry`, `SynonymNode`, `TrieNode`, `HistoryNode`), global variable declarations, function prototypes, and constants.

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

1. **View all words and synonyms:** Displays every word in the dictionary followed by its list of synonyms.
2. **View all words entry:** Displays all primary words, categorized and sorted alphabetically.
3. **Find synonyms words:** Search for a word and display its synonyms.
4. **Add new entry word:** Add a new primary word to the dictionary.
5. **Add synonyms to entry words:** Add a new synonym to an existing primary word.
6. **Delete entry word:** Delete a primary word and all of its associated synonyms.
7. **Delete synonym from entry words:** Delete a single synonym from a primary word.
8. **View history:** Display the log of all actions taken.
9. **Save and exit:** Saves all changes to synonymList.txt and history.txt, then closes the application.

## Data Structures

The program utilizes several custom data structures:

* **`SynonymNode`**: Represents a single synonym in a linked list.
  * `char* word`: Stores the synonym string.
  * `struct SynonymNode* next`: Points to the next synonym in the list for a given word.

* **`WordEntry`**: Represents an entry in the hash table for a primary word.
  * `char* word`: Stores the primary word string.
  * `SynonymNode* synonyms`: A pointer to the head of a linked list of its synonyms.
  * `struct WordEntry* next`: Points to the next `WordEntry` in case of a hash collision (separate chaining).

* **`TrieNode`**: Represents a single node in a Trie data structure, used for efficient prefix-based word searching and auto-completion.
  * `struct TrieNode *children[ALPHABET_SIZE]`: An array of pointers, where each index corresponds to a letter of the alphabet. A non-null pointer indicates that the character exists as a child of the current node, continuing a word path.
  * `int isEndOfWord`: A boolean flag (typically `1` for true) that marks the node as the final character of a valid, complete word.

* **`HistoryNode`**: Represents an entry in the action history log.
  * `char* action`: Describes the action performed (e.g., "Added word", "Added synonym").
  * `char* time`: Stores the timestamp of when the action occurred. *(Note: The `getCurrentTimestamp` function from previous discussions would be relevant here if `history.c` uses it).*
  * `struct HistoryNode* next`: Points to the next history entry in a linked list.

* **Hash Table**: An array of `WordEntry*` pointers (implicitly, not directly defined as a struct but managed via `HASH_SIZE`). The `hash()` function determines the index for a given word.

---
