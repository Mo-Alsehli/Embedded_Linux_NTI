# Advanced Bash Scripting

## Variable Handling

### **Unset a Local Variable**

```bash
unset var
```

👉 Removes a variable from the current shell environment.

---

### **Default Value Handling**

We can assign default values to variables using parameter expansion:

* `echo "${NAME:-Default}"`
  → If `NAME` is unset/empty, returns `Default` (but does not assign).

* `echo "${NAME:=Default}"`
  → If `NAME` is unset/empty, assigns `Default` to `NAME` and returns it.

* `echo "${NAME:+Value}"`
  → If `NAME` is set and not empty, expands to `Value`, otherwise empty.

* `echo "${NAME:?Error message}"`
  → If `NAME` is unset/empty, prints an error and exits.

---

## String Operations

### **Check if String is Empty**

```bash
str=""

if [[ -z "$str" ]]; then
    echo "String is empty"
fi

if [[ -n "$str" ]]; then
    echo "String is NOT empty"
fi
```

---

### **Substring Extraction**

```bash
string="Hello world"
echo "Substring from index 3: ${string:3}"    # lo world
echo "Substring of length 5 from index 0: ${string:0:5}"  # Hello
echo "Last 3 characters: ${string: -3}"       # rld
```

---

### **Pattern Matching**

```bash
filename="report.txt"

if [[ $filename == *.txt ]]; then
    echo "This is a text file"
fi
```

👉 Wildcards (`*`, `?`) can be used inside `[[ ]]`.

---

### **Pattern Search**

Using regex inside `[[ ]]`:

```bash
str="abc123"

if [[ $str =~ [0-9]+ ]]; then
    echo "String contains numbers"
fi
```

---

### **Trim Whitespaces**

```bash
str="   hello world   "

# Trim leading/trailing spaces
trimmed="$(echo -e "$str" | sed -e 's/^[[:space:]]*//' -e 's/[[:space:]]*$//')"
echo ">$trimmed<"
```

Or using Bash parameter expansion (trims only fixed characters):

```bash
str="   hello world   "
echo ">${str##*( )}<"   # requires extglob enabled
```

---

## Flow Control

### **For Loop**

```bash
echo "Iterating over files in current directory:"
for file in *; do
    echo "File: $file"
done
```

```bash
for i in {1..5}; do
    echo "Number: $i"
done
```

---

## Exit Status (Status Code)

* **Successful execution → `0`**.
* **Errors → `1–127`**, each has different meaning.
* **>128 → Process terminated by a signal (value = 128 + signal\_number).**

Check last exit status:

```bash
echo "Status code: $?"
```

👉 `$?` always stores the exit code of the **last executed command**.

---

## Functions in Bash Scripting

### **Define and Call Functions**

```bash
my_function() {
    local param1="$1"
    local param2="$2"

    echo "Parameters: $param1, $param2"
}

my_function "Hello" "World"
```

---

### **Function Best Practices**

* Always **validate parameters**.
* Use `local` for function-scoped variables to avoid polluting global namespace.
* Return values with `echo` or via exit codes.
* Keep functions small and focused.

---

## Writing a Bash Script (Generic Workflow)

1. **Identify Requirements** (inputs, outputs, processing).
2. **Analysis Framework (A–B–C–D–E–F–G)**

   * **A** → Analyze Requirements (Read – Sort).
   * **B** → Bold/Box Nouns (these become data types/variables).
   * **C** → Circle Verbs (these become functions).
   * **D–G** → Define, Group, Expand into code.
3. Convert requirements into Bash.

---

### **Bash Script File Structure**

* Add a **shebang** and top-level comment:

  ```bash
  #!/bin/bash
  # This script demonstrates advanced Bash scripting
  ```
* Document usage info (`--help` flag if needed).
* Use a `main()` function as the entry point:

  ```bash
  main() {
      echo "Main program logic here"
  }

  main "$@"
  ```

👉 `"$@"` passes all script arguments into `main`.