# Bash Basics.

## Terminal
**Terminal is a text place or just a console but with special connection to standard input/output.**
- **Terminal Lifecycle:**:
1. Init/open
2. Operations & commands 
3. Close.

### 1. Terminal Init / Open:
> In this stage the terminal searchs for variables in a set of files.
- Loading: `/ect/environment` Contains global Environment Variables for the whole system -> all users.
- Loading: `~/.bashrc` Contains Globla Environment stricted to user only.

- **`SHELL` Global Variable.**: It takes the path for applications executer (bash).
- **`PATH` Global Variable.**: It takes the path of the directories that the terminal search for applications in it.

### 2. Operations / write:
- Passes the application path to the executer.


## Shell Language.
- It's a scripting language that's compiled line by line.
- there are multiple languages like shell, zsh and bash.



# BASH Scripting.
**Bash is like a file which has some fixed syntax.**

- **shebang** -> `#!/user/bin/bash`: It has the path of the bash executer.
- **Inputs**: global variables, positional parameter, from another bash file, Interactive mode.
- **Operations**: Some operations on inputs.
- **Result**: More like output to inform the user (stdout, Redirection, Status).

- After writing the bash script we have to make it executable `chmod u+x <script_name>`.

- **Positional Parameters**: `./script_name <positional_parameters>`
    - We can catch the positional parameters using the `$numberOfParameter`.
    - the first positional parameter `$0` is always the name of the bash script.

### Variables in Bash
#### 1. Local Variable.
**Local Variable declaraion**
- `declare` variable_name=value -> declare here is optional.
- `declare -i` Creates an integer variable
- Naming convention is all letters are uppercase.
- The must be no spaces between var name and = or the = and value.
- **NOTE**: Local Variables are local only to the current script, even the process creates another process they didn't inherit local variables from eachothers.   

#### 2. Environment Variables:
**Environment Variable Declaraion**
- `export` varname=value
- Global Variable are global to the current script and it's children.
- we can delete a global variable using `unset`.


### Operations in Bash.
#### Command Substitution.
- It's used for executing a command and taking its output to a variable.
- `my_var=$(shell_command)`

#### Arithmatic Operations.
- `my_var=$((arithmatic_operations))

#### Comparisons.
if ((comparison)); then
    operation
fi
