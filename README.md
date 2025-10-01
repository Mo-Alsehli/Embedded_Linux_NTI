# Embedded Linux @ NTI — Repository Map & Folder Guide

A clear, non-technical guide to what’s inside this repository. Use it to quickly find what you need, understand the purpose of each area, and keep new additions consistent.

---

## Solid Steps to Navigate the Repo

1. Start at the **top-level folders** shown below.
2. Read the **“What it contains”** and **“When to use it”** under each folder.
3. Drop new content into the **closest matching folder** and follow the naming hints.

---

## Top-Level Structure (Bird’s-Eye View)

```
Embedded_Linux_NTI/
├── Cpp/
├── Embedded_Linux/
├── linux_admin/
└── README.md
```

**What this means**

* **Cpp/**: Small, focused C/C++ practice projects and examples.
* **Embedded_Linux/**: Session-based learning notes and labs for embedded Linux topics.
* **linux_admin/**: Handy day-to-day Linux administration notes (commands, workflows, quick references).
* **README.md**: The page you’re reading now.

---

## Folder-by-Folder Details

### 1) `Cpp/`

**Purpose:** A home for bite-sized programming exercises and examples.

**What it contains (typical layout):**

```
Cpp/
└── <topic-name>/
    ├── main.c | main.cpp        # The example’s main entry file
    ├── (support files)          # Any small helper files used by this example
    ├── README.md                # Short text describing what this example shows
    └── (build helpers)          # Files that help build/run the example (if present)
```

**When to use it:**

* You’re experimenting with a single idea (e.g., input/output, simple math, basic file use).
* You want quick, isolated examples that don’t depend on the rest of the repo.
* You’re adding a new practice exercise for yourself or others.

**Naming hints:**

* Use short, descriptive folder names like `strings_basics/`, `files_intro/`, `simple_timer/`.
* Keep one concept per folder to make browsing easy.

---

### 2) `Embedded_Linux/`

**Purpose:** Session-style learning material for embedded Linux, grouped by topic.

**What it contains (typical layout):**

```
Embedded_Linux/
└── SessionX_<short-topic-title>/
    ├── notes.md    # Read-through guide for the session (concepts, terms, context)
    ├── lab.md      # Step-by-step activity for this session (do-this-then-that flow)
    └── refs/       # Images, diagrams, and any “look-at-this” reference items
```

**When to use it:**

* You want a structured “lesson” experience: read, then do, then review.
* You’re exploring a new topic and want a single place that explains the idea and walks you through it.
* You’re contributing a new session to teach a focused topic.

**Naming hints:**

* Keep the `SessionX_` prefix followed by a short topic (e.g., `Session1_Introduction_Embedded_Linux/`).
* Put screenshots/diagrams in `refs/` so the session stays tidy.

---

### 3) `linux_admin/`

**Purpose:** Quick, practical notes for everyday Linux tasks.

**What it contains (typical layout):**

```
linux_admin/
├── commands.md     # Frequently used one-liners and short notes
├── networking.md   # Basics on viewing network info and connection checks
└── storage.md      # Notes about disks, space usage, and mounting drives
```

**When to use it:**

* You need a quick reminder of a common task (e.g., “Where do I check disk usage?”).
* You’re adding a short “how-I-do-this” note you’ll likely reuse.
* You want a central place to keep small, practical references without turning them into full sessions.

**Naming hints:**

* Keep files grouped by theme (`processes.md`, `users_groups.md`, `logs.md`) rather than mixing many topics together.
* Keep entries short and scannable.

---

---
## At a Glance (Cheat Sheet)

* **Cpp/** → Small practice examples (one idea each).
* **Embedded_Linux/** → Session-style learning (notes + lab + refs).
* **linux_admin/** → Practical notes for everyday tasks.


