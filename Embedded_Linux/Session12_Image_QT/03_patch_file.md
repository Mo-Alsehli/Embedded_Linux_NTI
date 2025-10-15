# Patch File 


## Creating a patch file
- to create a patch file we usually need a project as a git repo.
- After we make each modification in files in the git rebo (app directory) we hit the command:
    - `git diff > filename.patch`.
    - The we move this file inside the files inside the recipe append directory.
    - The patch file is supposed to be as an extend to the recipe.
    - We add its scheme to the `SRC_URI:append`.
    - After adding the file to the `SRC_URI` we might need to assign the following
        - `S = "${WORKDIR}/git"`

### Making a Patch File to modify a specific file in a specific recipe.
- We create a patch file ex `Makefile.patch` and copy it the `files` in the `bbappend` recipe.
- Inside the `recipe.bbappend` we add it to the `SRC_URI`.