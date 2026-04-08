# Rev 2 -- Clean Single-File Program

## Where We Are

In Rev 1 you saw the example -- a bank account program crammed into one file
with old-style struct names and minimal error checking. It works, but it is hard to
read and harder to extend.

Rev 2 is the **same program, rewritten cleanly**, still in a single file. Nothing new
is added in terms of architecture -- this is about writing better C before we start
splitting things up.

## What Changed from Rev 1

| Area | Rev 1 | Rev 2 |
|------|-------|-------|
| Struct | `struct clientData` used everywhere | `typedef struct { ... } Account;` -- shorter, cleaner |
| Fields | `firstName[10]`, `lastName[15]` | `firstName[50]`, `lastName[50]` -- bigger buffers |
| Naming | `deleteRecord`, `newRecord` | `deleteAccount`, `createAccount` -- names match what they do |
| Input handling | No validation | Checks `scanf` return values, rejects out-of-range account numbers |
| File init | Crashes if file missing | Auto-creates the file with blank records on first run |
| Menu | Exits on choice 5 | Exits on choice 0 (more natural for a numbered menu) |

## What You Will Learn

1. **`typedef`** -- how to give a struct a short alias so you write `Account` instead
   of `struct clientData` everywhere.
2. **Input validation** -- checking `scanf` return values to catch non-numeric input.
3. **`perror()`** -- printing system error messages when file operations fail.
4. **`fgets` for safe string input** -- using `fgets(buf, MAX_NAME, stdin)` to read
   names safely, and `strcspn` to strip the trailing newline. This replaces
   `scanf("%s", ...)` which cannot limit input length or handle spaces.
5. **Code readability** -- meaningful function names, consistent formatting, and
   comments that explain *why*, not *what*.

## Files

```
rev2/
  trans.c          <-- the entire program (single file)
```

## How to Compile and Run

**Linux / macOS:**
```bash
gcc -o trans trans.c
./trans
```

**Windows (Developer Command Prompt):**
```cmd
cl /Fe:trans.exe trans.c
trans.exe
```

## Menu Options

```
1. Create Account
2. Update Account Balance
3. Delete Account
4. View Account
5. List All Accounts
0. Exit
```

## Try It

1. Run the program.
2. Create two accounts (option 1).
3. List them (option 5) -- notice the formatted table.
4. Update one balance (option 2).
5. Delete one (option 3) -- then list again to confirm it is gone.
6. Try entering an invalid account number (like 0 or 999) and see the error message.

## What Is Next

The code works, but everything is in **one file**. As programs grow, a single file
becomes impossible to manage. In Rev 3, we split this into multiple files with
headers -- the first step toward a real project structure.
