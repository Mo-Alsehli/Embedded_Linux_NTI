# Comprehensive Shell Scripting Guide

## Table of Contents
1. [Introduction](#introduction)
2. [Getting Started](#getting-started)
3. [Basic Syntax](#basic-syntax)
4. [Variables](#variables)
5. [Input/Output](#inputoutput)
6. [Control Structures](#control-structures)
7. [Functions](#functions)
8. [Arrays](#arrays)
9. [String Manipulation](#string-manipulation)
10. [File Operations](#file-operations)
11. [Process Management](#process-management)
12. [Error Handling](#error-handling)
13. [Advanced Topics](#advanced-topics)
14. [Best Practices](#best-practices)
15. [Common Patterns](#common-patterns)
16. [Debugging](#debugging)

## Introduction

Shell scripting is a powerful way to automate tasks, manage system operations, and create command-line tools. This guide focuses primarily on Bash (Bourne Again Shell), the most common shell on Linux and macOS systems.

### What is Shell Scripting?
- A shell script is a text file containing a series of commands
- Scripts can automate repetitive tasks
- They can combine multiple commands into complex workflows
- Shell scripts are interpreted, not compiled

### Common Use Cases
- System administration tasks
- File management and processing
- Backup and maintenance scripts
- Application deployment
- Data processing pipelines

## Getting Started

### Creating Your First Script

```bash
#!/bin/bash
# This is a comment
echo "Hello, World!"
```

### Making Scripts Executable
```bash
chmod +x myscript.sh
./myscript.sh
```

### The Shebang Line
The first line `#!/bin/bash` tells the system which interpreter to use:
- `#!/bin/bash` - Use Bash
- `#!/bin/sh` - Use standard shell (POSIX)
- `#!/usr/bin/env bash` - Use Bash from PATH

## Basic Syntax

### Comments
```bash
# Single line comment
echo "Hello"  # Inline comment

# Multi-line comments using here document
: '
This is a
multi-line comment
'
```

### Command Execution
```bash
# Direct command execution
ls -la

# Command substitution
current_date=$(date)
files=`ls`  # Older syntax, backticks
```

### Whitespace and Line Continuation
```bash
# Line continuation with backslash
echo "This is a very long line that \
continues on the next line"

# Whitespace matters around operators
var=value    # Correct
var = value  # Wrong - creates command
```

## Variables

### Variable Declaration and Assignment
```bash
# Simple assignment (no spaces around =)
name="John Doe"
age=30
readonly PI=3.14159

# Using variables
echo "Name: $name"
echo "Age: ${age}"  # Preferred syntax
```

### Variable Types
```bash
# String variables
greeting="Hello"
message='Single quotes preserve literal values'

# Numeric variables (bash treats all as strings)
number=42
result=$((number + 10))  # Arithmetic expansion

# Array variables
fruits=("apple" "banana" "orange")
```

### Environment Variables
```bash
# Setting environment variables
export PATH="/usr/local/bin:$PATH"
export MY_VAR="value"

# Common environment variables
echo $HOME     # User home directory
echo $USER     # Current username
echo $PWD      # Current directory
echo $PATH     # Executable search path
```

### Special Variables
```bash
# Script parameters
echo $0        # Script name
echo $1        # First argument
echo $2        # Second argument
echo $#        # Number of arguments
echo $@        # All arguments as separate strings
echo $*        # All arguments as single string
echo $$        # Process ID of script
echo $?        # Exit status of last command
echo $!        # Process ID of last background command
```

## Input/Output

### Reading Input
```bash
# Simple input
echo "Enter your name:"
read name
echo "Hello, $name"

# Silent input (for passwords)
read -s password

# Input with prompt
read -p "Enter your age: " age

# Reading multiple values
read first last <<< "John Doe"
```

### Output Formatting
```bash
# Basic output
echo "Simple text"
printf "Formatted output: %s is %d years old\n" "$name" "$age"

# Output redirection
echo "Log entry" > logfile.txt     # Overwrite
echo "Another entry" >> logfile.txt # Append
```

### Here Documents and Here Strings
```bash
# Here document
cat << EOF
This is a multi-line
text block that can contain
variables like $USER
EOF

# Here string
grep "pattern" <<< "$variable"
```

## Control Structures

### Conditional Statements

#### If Statements
```bash
# Basic if statement
if [ "$age" -gt 18 ]; then
    echo "You are an adult"
fi

# If-else
if [ "$score" -ge 90 ]; then
    echo "Excellent!"
elif [ "$score" -ge 70 ]; then
    echo "Good!"
else
    echo "Needs improvement"
fi
```

#### Test Conditions
```bash
# File tests
[ -f file.txt ]      # File exists and is regular file
[ -d directory ]     # Directory exists
[ -r file.txt ]      # File is readable
[ -w file.txt ]      # File is writable
[ -x script.sh ]     # File is executable

# String tests
[ -z "$string" ]     # String is empty
[ -n "$string" ]     # String is not empty
[ "$str1" = "$str2" ] # Strings are equal
[ "$str1" != "$str2" ] # Strings are not equal

# Numeric comparisons
[ "$a" -eq "$b" ]    # Equal
[ "$a" -ne "$b" ]    # Not equal
[ "$a" -lt "$b" ]    # Less than
[ "$a" -le "$b" ]    # Less than or equal
[ "$a" -gt "$b" ]    # Greater than
[ "$a" -ge "$b" ]    # Greater than or equal
```

#### Case Statements
```bash
case "$1" in
    start)
        echo "Starting service..."
        ;;
    stop)
        echo "Stopping service..."
        ;;
    restart)
        echo "Restarting service..."
        ;;
    *)
        echo "Usage: $0 {start|stop|restart}"
        exit 1
        ;;
esac
```

### Loops

#### For Loops
```bash
# Traditional for loop
for i in {1..10}; do
    echo "Number: $i"
done

# Loop over array
fruits=("apple" "banana" "orange")
for fruit in "${fruits[@]}"; do
    echo "Fruit: $fruit"
done

# Loop over files
for file in *.txt; do
    echo "Processing $file"
done

# C-style for loop
for ((i=0; i<10; i++)); do
    echo "Index: $i"
done
```

#### While Loops
```bash
# Basic while loop
count=1
while [ $count -le 5 ]; do
    echo "Count: $count"
    ((count++))
done

# Reading file line by line
while IFS= read -r line; do
    echo "Line: $line"
done < input.txt
```

#### Until Loops
```bash
# Until loop (opposite of while)
counter=1
until [ $counter -gt 5 ]; do
    echo "Counter: $counter"
    ((counter++))
done
```

### Loop Control
```bash
# Break and continue
for i in {1..10}; do
    if [ $i -eq 3 ]; then
        continue  # Skip iteration
    fi
    if [ $i -eq 8 ]; then
        break     # Exit loop
    fi
    echo $i
done
```

## Functions

### Function Definition and Calling
```bash
# Function definition
greet() {
    echo "Hello, $1!"
}

# Alternative syntax
function greet {
    echo "Hello, $1!"
}

# Calling function
greet "World"
```

### Function Parameters and Return Values
```bash
calculate_sum() {
    local num1=$1
    local num2=$2
    local result=$((num1 + num2))
    echo $result  # Return value via echo
}

# Using function return value
result=$(calculate_sum 5 3)
echo "Sum is: $result"

# Using return for exit status
is_even() {
    local number=$1
    if [ $((number % 2)) -eq 0 ]; then
        return 0  # Success (true)
    else
        return 1  # Failure (false)
    fi
}

if is_even 4; then
    echo "Number is even"
fi
```

### Local Variables
```bash
my_function() {
    local local_var="This is local"
    global_var="This is global"
    echo $local_var
}
```

## Arrays

### Array Declaration and Initialization
```bash
# Declaration
declare -a my_array

# Initialization
fruits=("apple" "banana" "orange")
numbers=(1 2 3 4 5)

# Associative arrays (Bash 4+)
declare -A colors
colors[red]="#FF0000"
colors[green]="#00FF00"
colors[blue]="#0000FF"
```

### Array Operations
```bash
# Access elements
echo ${fruits[0]}        # First element
echo ${fruits[-1]}       # Last element (Bash 4+)

# All elements
echo ${fruits[@]}        # All elements as separate words
echo ${fruits[*]}        # All elements as single word

# Array length
echo ${#fruits[@]}       # Number of elements
echo ${#fruits[0]}       # Length of first element

# Add elements
fruits+=("grape")        # Append element
fruits[10]="kiwi"       # Set specific index

# Remove elements
unset fruits[1]         # Remove element at index 1
```

### Iterating Over Arrays
```bash
# Iterate over values
for fruit in "${fruits[@]}"; do
    echo "Fruit: $fruit"
done

# Iterate over indices
for i in "${!fruits[@]}"; do
    echo "Index $i: ${fruits[i]}"
done

# Associative array iteration
for key in "${!colors[@]}"; do
    echo "Color $key: ${colors[$key]}"
done
```

## String Manipulation

### String Length and Substrings
```bash
text="Hello World"

# String length
echo ${#text}           # Output: 11

# Substrings
echo ${text:0:5}        # Output: "Hello" (start:length)
echo ${text:6}          # Output: "World" (from position 6)
echo ${text:(-5)}       # Output: "World" (last 5 characters)
```

### Pattern Matching and Replacement
```bash
filename="document.txt.backup"

# Remove from beginning (shortest match)
echo ${filename#*.}     # Output: "txt.backup"

# Remove from beginning (longest match)
echo ${filename##*.}    # Output: "backup"

# Remove from end (shortest match)
echo ${filename%.*}     # Output: "document.txt"

# Remove from end (longest match)
echo ${filename%%.*}    # Output: "document"

# Replace first occurrence
echo ${filename/txt/doc}    # Output: "document.doc.backup"

# Replace all occurrences
echo ${filename//./_}       # Output: "document_txt_backup"
```

### Case Conversion (Bash 4+)
```bash
text="Hello World"

# Convert to lowercase
echo ${text,,}          # Output: "hello world"
echo ${text,}           # Output: "hello World" (first char only)

# Convert to uppercase
echo ${text^^}          # Output: "HELLO WORLD"
echo ${text^}           # Output: "Hello World" (first char only)
```

## File Operations

### File Testing
```bash
file="/etc/passwd"

if [ -f "$file" ]; then
    echo "File exists"
    
    if [ -r "$file" ]; then
        echo "File is readable"
    fi
    
    if [ -w "$file" ]; then
        echo "File is writable"
    fi
    
    if [ -x "$file" ]; then
        echo "File is executable"
    fi
fi
```

### File Reading and Writing
```bash
# Reading files
while IFS= read -r line; do
    echo "Line: $line"
done < input.txt

# Writing files
echo "New content" > output.txt
echo "Appended content" >> output.txt

# Reading entire file into variable
content=$(cat file.txt)
```

### Directory Operations
```bash
# Create directory
mkdir -p path/to/directory

# Change directory
cd /path/to/directory || exit 1

# List directory contents
for item in *; do
    if [ -d "$item" ]; then
        echo "Directory: $item"
    elif [ -f "$item" ]; then
        echo "File: $item"
    fi
done
```

## Process Management

### Running Commands in Background
```bash
# Run in background
long_running_command &
pid=$!

# Wait for background process
wait $pid

# Check if process is running
if kill -0 $pid 2>/dev/null; then
    echo "Process is running"
else
    echo "Process is not running"
fi
```

### Process Substitution
```bash
# Compare output of two commands
diff <(ls dir1) <(ls dir2)

# Use command output as input file
while read -r line; do
    echo "Processing: $line"
done < <(find /path -name "*.txt")
```

### Job Control
```bash
# List jobs
jobs

# Bring job to foreground
fg %1

# Send job to background
bg %1

# Kill job
kill %1
```

## Error Handling

### Exit Codes
```bash
# Set exit codes
command
if [ $? -eq 0 ]; then
    echo "Command succeeded"
else
    echo "Command failed"
    exit 1
fi

# Shorter form
command && echo "Success" || echo "Failed"
```

### Error Trapping
```bash
# Set up error trap
trap 'echo "Error on line $LINENO"; exit 1' ERR

# Cleanup trap
cleanup() {
    echo "Cleaning up..."
    rm -f temp_file
}
trap cleanup EXIT

# Signal traps
trap 'echo "Interrupted"; exit 1' INT TERM
```

### Strict Mode
```bash
# Enable strict error handling
set -euo pipefail
# -e: exit on error
# -u: exit on undefined variable
# -o pipefail: pipe failure causes script failure
```

## Advanced Topics

### Parameter Expansion
```bash
# Default values
echo ${var:-default}        # Use default if var is unset
echo ${var:=default}        # Set var to default if unset
echo ${var:+alternative}    # Use alternative if var is set

# Required parameters
echo ${var:?error message}  # Exit with error if var is unset

# Indirect expansion
var_name="PATH"
echo ${!var_name}          # Expands to value of PATH
```

### Process Substitution Advanced
```bash
# Multiple inputs
paste <(cut -f1 file1) <(cut -f2 file2) > combined

# Output to multiple files
command | tee >(process1) >(process2) > final_output
```

### Coprocesses (Bash 4+)
```bash
# Start coprocess
coproc my_coproc { python3 -u script.py; }

# Write to coprocess
echo "input data" >&${my_coproc[1]}

# Read from coprocess
read -r output <&${my_coproc[0]}
```

### Regular Expressions
```bash
# Pattern matching with =~
if [[ "$email" =~ ^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$ ]]; then
    echo "Valid email"
fi

# Extract matches
if [[ "$text" =~ ([0-9]+) ]]; then
    number=${BASH_REMATCH[1]}
    echo "Found number: $number"
fi
```

## Best Practices

### Script Structure
```bash
#!/bin/bash
set -euo pipefail

# Global variables
readonly SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
readonly SCRIPT_NAME="$(basename "$0")"

# Functions
usage() {
    cat << EOF
Usage: $SCRIPT_NAME [OPTIONS]
Description of what the script does.

OPTIONS:
    -h, --help      Show this help message
    -v, --verbose   Enable verbose output
EOF
}

main() {
    # Main script logic here
    echo "Script executed successfully"
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            usage
            exit 0
            ;;
        -v|--verbose)
            set -x
            shift
            ;;
        *)
            echo "Unknown option: $1"
            usage
            exit 1
            ;;
    esac
done

# Run main function
main "$@"
```

### Coding Standards
```bash
# Use meaningful variable names
user_count=10  # Good
uc=10         # Bad

# Quote variables to prevent word splitting
echo "$var"    # Good
echo $var     # Bad (can break)

# Use readonly for constants
readonly MAX_RETRIES=3

# Use local variables in functions
my_function() {
    local temp_file="/tmp/myapp.$$"
    # function logic
}

# Check command existence
if ! command -v git >/dev/null 2>&1; then
    echo "Error: git is not installed"
    exit 1
fi
```

### Security Considerations
```bash
# Avoid eval
# eval "$user_input"  # Dangerous!

# Use arrays for command arguments
args=("--flag" "$user_input" "--output" "$output_file")
command "${args[@]}"

# Create secure temporary files
temp_file=$(mktemp)
trap 'rm -f "$temp_file"' EXIT

# Validate input
validate_input() {
    local input=$1
    if [[ ! "$input" =~ ^[a-zA-Z0-9_-]+$ ]]; then
        echo "Invalid input"
        exit 1
    fi
}
```

## Common Patterns

### Configuration File Parsing
```bash
# Simple key=value config
while IFS='=' read -r key value; do
    case $key in
        username) username=$value ;;
        password) password=$value ;;
        host) host=$value ;;
    esac
done < config.txt
```

### Log Rotation
```bash
rotate_log() {
    local logfile=$1
    local max_size=${2:-1000000}  # 1MB default
    
    if [ -f "$logfile" ] && [ $(stat -f%z "$logfile" 2>/dev/null || stat -c%s "$logfile" 2>/dev/null) -gt $max_size ]; then
        mv "$logfile" "${logfile}.old"
        touch "$logfile"
    fi
}
```

### Retry Logic
```bash
retry() {
    local max_attempts=$1
    shift
    local attempt=1
    
    until [ $attempt -gt $max_attempts ]; do
        if "$@"; then
            return 0
        fi
        echo "Attempt $attempt failed. Retrying..."
        ((attempt++))
        sleep 2
    done
    
    echo "All $max_attempts attempts failed."
    return 1
}

# Usage
retry 3 curl -f https://api.example.com/data
```

### Progress Indicators
```bash
show_progress() {
    local current=$1
    local total=$2
    local width=50
    local percentage=$((current * 100 / total))
    local completed=$((current * width / total))
    
    printf "\rProgress: ["
    printf "%*s" $completed | tr ' ' '='
    printf "%*s" $((width - completed)) | tr ' ' '-'
    printf "] %d%%" $percentage
}

# Usage in loop
total=100
for i in $(seq 1 $total); do
    # Do work here
    sleep 0.1
    show_progress $i $total
done
echo  # New line after progress
```

## Debugging

### Debug Options
```bash
# Enable debug mode
set -x          # Print commands and arguments
set +x          # Disable debug mode

# Debug specific sections
{
    set -x
    # Commands to debug
    command1
    command2
    set +x
} 2>/tmp/debug.log
```

### Debugging Functions
```bash
debug() {
    if [[ "${DEBUG:-}" == "true" ]]; then
        echo "DEBUG: $*" >&2
    fi
}

# Usage
DEBUG=true
debug "This is a debug message"
```

### Common Debugging Techniques
```bash
# Print variable values
echo "DEBUG: variable value is '$variable'" >&2

# Check if file operations succeeded
if ! cp source dest; then
    echo "Failed to copy file" >&2
    exit 1
fi

# Validate assumptions
if [[ ! -d "$directory" ]]; then
    echo "Error: $directory is not a directory" >&2
    exit 1
fi
```

### Linting and Testing
```bash
# Use shellcheck for static analysis
# Install: apt-get install shellcheck
# Usage: shellcheck script.sh

# Simple test framework
test_function() {
    local expected=$1
    local actual=$2
    if [[ "$expected" == "$actual" ]]; then
        echo "PASS: $expected"
    else
        echo "FAIL: expected '$expected', got '$actual'"
        return 1
    fi
}

# Example test
result=$(calculate_sum 2 3)
test_function "5" "$result"
```