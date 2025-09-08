# User Management Stack Notes

**Linux is a multi user enviornment**
**Every thing is a file**

### Handling Permissions
- When a User creates a process and he have some permissions, the process he created inherits the same permissions.

### File Permissions (ls -la):
**Each file may have three permissions `read`, `write`, `execute`.**
- If the file Permissions starts with `-` this means it's a normal file.
    - If it starts with `d` it represents a directory.
- `rw` read write permissions.



## Users.
**There are two types of users**
- Normal User.
- System User.
- There are groups for sharing permissions.
- In the context of users ID if the id is greater than 999 then it's a normal user but if it's less than or equal to 999 it's a system user.
### User Commands:
- To create a user `add user`.
- To delete a user `deluser`.
- To modify a user `usermod`.
- Add user to a group `usermod -G <group_name> <user_name>`.
- change user password `passwd`.
- Get information about logged in user `id`.

## Groups.
- To create a group `addgroup`.
- To delete a group `delgroup`.
- To change a group `chgrp`.

## Resources.
- To change the owner of a resource `chown`.
- To change permissions `chmod u(+/- to add or remove a specific permission)permission file`.
- 
