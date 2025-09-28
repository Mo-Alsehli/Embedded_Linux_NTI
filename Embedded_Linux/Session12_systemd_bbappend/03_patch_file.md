# Patch File 


### Making a Patch File to modify a specific file in a specific recipe.
- We create a patch file ex `Makefile.patch` and copy it the `files` in the `bbappend` recipe.
- Inside the `recipe.bbappend` we add it to the `SRC_URI`.