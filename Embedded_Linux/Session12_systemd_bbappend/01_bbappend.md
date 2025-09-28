# bbappend (Editing others recipes)

**It's usually not good to edit previously written recipe**
**We create bbappend to in order to append specific feature on the original reicpe**


### Creating the bbappend.
- create your own layer for this bbappend.
- **The Recipe Inside This Layer Must be Exactly With The Same Name As The Image You Want To Add Feature To**.
- The package You want to modify might have a version with it's name and you have to assign this version also to the recipe you will append to through the `PV` variable.
    - `bitbake -e <recipe-name> | grep ^PV=`
- Or we can just add the `%` at the end of the name `recipe%.bbappend` which represents the last version.
- Inside it we appedn to the functions like that 
```
do_func:append(){

}
// Ex
do_install:append(){

}
```

## EX. modifing a specific recipe to use systemd