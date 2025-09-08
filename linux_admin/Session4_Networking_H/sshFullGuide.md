# 🔐 SSH Key Authentication Practice - Full Guide

This README contains step-by-step instructions for practicing SSH key authentication between:

* **Scenario A:** PC (Ubuntu) as **client** and Phone (Termux) as **server**
* **Scenario B:** PC (Ubuntu) as **server** and Phone (Termux) as **client**
* **Scenario C:** PC-A (Ubuntu) as **client** and PC-B (Ubuntu) as **server**

Each scenario includes commands for generating keys, fixing permissions, and connecting.

---

## ✅ Scenario A: PC (Ubuntu) → Phone (Termux)

> Goal: Connect from Ubuntu PC to Termux using SSH **without password**

### 1. 🔑 Generate SSH Key on PC

```bash
ssh-keygen -t ed25519 -C "pc_to_termux"
```

* When prompted: enter a **custom name** if desired, e.g. `mmubuntu_ssh_key`
* It creates:

  * `~/.ssh/mmubuntu_ssh_key` (private key)
  * `~/.ssh/mmubuntu_ssh_key.pub` (public key)

### 2. 📂 Move Keys to .ssh (if needed)

```bash
mv ~/mmubuntu_ssh_key* ~/.ssh/
chmod 700 ~/.ssh
chmod 600 ~/.ssh/mmubuntu_ssh_key
chmod 644 ~/.ssh/mmubuntu_ssh_key.pub
```

### 3. 📲 On Termux (Phone) — Start SSH Server

```bash
pkg update && pkg upgrade -y
pkg install openssh -y
# It may be missing permissions. Try:
termux-setup-storage
sshd
passwd  # Set password for login (first time only)
```

### 4. 🔁 Copy Public Key from PC to Termux

```bash
ssh-copy-id -i ~/.ssh/mmubuntu_ssh_key.pub -p 8022 u0_aXXX@<phone_ip>
```

* Replace `u0_aXXX` with `whoami` on Termux
* Replace `<phone_ip>` with output of `ip addr show` on Termux

### 5. 🔐 Connect from PC to Termux

```bash
ssh -i ~/.ssh/mmubuntu_ssh_key -p 8022 u0_aXXX@<phone_ip>
```

### 6. (Optional) 🔒 Disable Password Login on Termux

```bash
mkdir -p ~/.ssh
printf "PasswordAuthentication no\n" >> ~/.ssh/sshd_config
pkill sshd
sshd -f ~/.ssh/sshd_config
```

---

## ✅ Scenario B: Phone (Termux) → PC (Ubuntu)

> Goal: Connect from Termux to Ubuntu PC via SSH key

### 1. 🔐 Generate Key on Termux

```bash
ssh-keygen -t ed25519 -C "termux_to_pc"
```

### 2. 🖥️ Enable SSH Server on Ubuntu (PC)

```bash
sudo apt update
sudo apt install openssh-server -y
sudo systemctl enable --now ssh
```

### 3. 🔁 Copy Public Key from Termux to PC

```bash
ssh-copy-id mmagdi@<pc_ip>
```

* If password login is disabled, do it manually:

#### On Termux:

```bash
cat ~/.ssh/id_ed25519.pub
```

* Copy full line

#### On Ubuntu PC:

```bash
nano ~/.ssh/authorized_keys
# Paste the public key
chmod 700 ~/.ssh
chmod 600 ~/.ssh/authorized_keys
```

### 4. 🔐 Connect from Termux to Ubuntu PC

```bash
ssh mmagdi@<pc_ip>
```

### 5. (Optional) 🔒 Disable Password Login on PC

```bash
sudo nano /etc/ssh/sshd_config
# Change or add:
PasswordAuthentication no

sudo systemctl restart ssh
```

---

## ✅ Scenario C: PC-A (Ubuntu) → PC-B (Ubuntu)

> Goal: Connect from one Ubuntu machine to another using SSH key

### 1. 🔑 Generate SSH Key on PC-A

```bash
ssh-keygen -t ed25519 -C "pcA_to_pcB"
```

* Store in default or custom file under `~/.ssh/`

### 2. 🖥️ Enable SSH Server on PC-B

```bash
sudo apt update
sudo apt install openssh-server -y
sudo systemctl enable --now ssh
```

### 3. 🔁 Copy Public Key from PC-A to PC-B

```bash
ssh-copy-id -i ~/.ssh/pcA_to_pcB.pub username_on_B@<pc_B_ip>
```

* Replace `username_on_B` and `<pc_B_ip>` accordingly

# 🔐 Manual SSH Key Installation (When Password Authentication is Disabled)

> Scenario: You want to connect from **PC-A** to **PC-B**, but **PC-B has password authentication disabled**, so `ssh-copy-id` won’t work.

---

## ✅ Step 1: On PC-A (the client)

### Show your public key:
```bash
cat ~/.ssh/pcA_to_pcB.pub
````

### Copy the entire output

It should look like:

```
ssh-ed25519 AAAAC3... user@pcA
```

---

## ✅ Step 2: On PC-B (the server)

> You must already have access to PC-B (via terminal, USB keyboard, or already working SSH key).

### Open the authorized\_keys file:

```bash
nano ~/.ssh/authorized_keys
```

### Paste the public key (copied from PC-A) into the file

Make sure it's on a single line.

### Save and exit:

* Press `Ctrl+O` → `Enter` to save
* Press `Ctrl+X` to exit

---

## ✅ Step 3: Fix Permissions

Run the following to secure the `.ssh` folder and file:

```bash
chmod 700 ~/.ssh
chmod 600 ~/.ssh/authorized_keys
```

---

## ✅ Step 4: Connect from PC-A

Now test SSH from PC-A:

```bash
ssh -i ~/.ssh/pcA_to_pcB username_on_B@<pc_B_ip>
```

✔️ If everything is correct, you'll connect without being asked for a password.

---



### 4. 🔐 Connect from PC-A to PC-B

```bash
ssh -i ~/.ssh/pcA_to_pcB username_on_B@<pc_B_ip>
```

### 5. (Optional) 🔒 Disable Password Login on PC-B

```bash
sudo nano /etc/ssh/sshd_config
PasswordAuthentication no
sudo systemctl restart ssh
```

---

## 🧠 Extra Tips

### View IP Address (on both PC & Phone)

```bash
ip -4 addr show | grep inet
```

### Check SSH status (Ubuntu)

```bash
sudo systemctl status ssh
```

### Verbose SSH debug (to troubleshoot keys)

```bash
ssh -v -i ~/.ssh/mmubuntu_ssh_key -p 8022 u0_aXXX@<phone_ip>
```

---