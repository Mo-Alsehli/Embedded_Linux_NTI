# Yocto Variables

* [Yocto Variable Glossary](https://docs.yoctoproject.org/ref-manual/variables.html)

* **There are two types of variables in yocto**
* **Global Variable** (under `.conf` files )
    - It's global accross the whole build.
    - global accross all recipes.
* **Local Variables** (under `.bb`, `.bbclass`, `bbappedn` files)


## Variable Assignation
* `=` Equal(Simple Assignation) Assignation.
    - It's usally a bad practice with global variable.
    - It overwrites the whole variale value with a new one.

* `+=` Append Assignation (**Not Recommended**).
* `=+` Prepend Assignation (**Not Recommended**).
    - It appends or prepends a value to the variable.
    - Adds space between the two valuse.
    - This assignation method is not recommended cause if you made the append assignation before defining the variable with an initial value it will be completely ignored.

* `.=` `=.` Appends and Prepends (**Not Recommended**).
    - It appends and prepends but **without adding a space**.

* `?=` Week Assignation (**Default Value**).
    - It defines or assigns the variable with a default value.
    - If the variable was assigned again this value will be overriden.
    - If there are multiple weak assignations The assigned value is only the first one.


* `??=` Weak Weak Assignation
    - It defines also a default value for the default value.
    - If there are multipe weak weak assignations it takes the value of the last assigned one.

* `:=` Immediate Variable Expansion.
    - It expands and assigns the value of a variable without further parsing all possible assignations for it.

### Recommended ways to edit variables.
* `:append`
    - It appends a value to the variable even before the assignation.
    - **But you have to be sure to add space as it doesn't add it automatically**.
* `:prepend`

* `:remove`
    - It removes a value from a variable that has be previously added.

**NOTE: just be sure not to assign global variables inside recipes as the assigned value won't be reflected on the whole build**.


**Difference between `DEPEND` and  `RDEPENDS`**.
    - for ex. QT depends on qt library in compile time so the `DEPEND` variable is assigned with qt library, and qt depends on the gui so the `RDEPEND` should be assigned with any gui server like `x11`, `wayland`.


### License in Recipes