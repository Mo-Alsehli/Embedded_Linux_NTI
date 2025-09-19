# Init Process


## Run Levels
- 
```
<device>:<runlevel>:<action>:command
```
- The new part here which is run levels which represents initializing the user environment in different means.
- There are multiple[7] options for run levels in linux.
- To know the current run level we use the command `runlevel /`.
- To change the run level I'm currently in we use the command `init <runlevel>`
- Run level is a feature and it's not supported in busybox.
- run levels is introduced in the systemV.


### SystemV 
- It's used and introduced by Unix.
- It has simple Init Process.
- Configuration exectuion is faster.

### Configuring System level.
- First of all create the `inittab` file inside the `/etc`.
- Create 3 folders, each folder represent a specific run level script.
    - `rc<level>.d`, this command means  **run command <level> . directory**
- Create Generic folder that contains cpp application script.
    - `mkdir init.d`
    -  Create a script to start & stop the application.
    - It might be a bash or it can be another script language.
