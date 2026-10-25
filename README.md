# TypeMaster – Typing Speed & Accuracy Trainer

TypeMaster is a desktop typing trainer written in **C++** with a graphical interface built using **SFML**. It lets you practice typing on short passages, take a 60-second timed challenge, see your speed and accuracy live, and save every result to a CSV file.

The project is also a demonstration of **Object-Oriented Programming** concepts: inheritance, polymorphism, abstraction, encapsulation, exception handling and file handling.

---

## Screenshot

<img width="1013" height="732" alt="image" src="https://github.com/user-attachments/assets/7a91db4f-c89e-4ed9-b724-15a6025a6955" />

## Features

- **Practice Mode** – type a passage at your own pace; the session ends automatically when you type the whole passage correctly.
- **60-Second Timed Challenge** – one continuous 60-second test. If you finish a passage, the next one loads automatically without restarting the timer.
- **Live statistics** – WPM, accuracy, correct characters and wrong characters update while you type.
- **Colour feedback** – correct characters turn green, wrong characters turn red.
- **Multiple passages** – passages are loaded from `prompts.txt`. Pressing **Next** on the last passage wraps back to the first.
- **Auto line wrapping** – the passage and your typed text wrap to the next line instead of going off screen.
- **Result saving** – results are appended to `user_stats.csv`.
- **Error handling** – the program shows a clear error if `prompts.txt` or the font file is missing, or if no passages are found.

---

## How to Use

1. Run the program.
2. Choose a mode with the buttons: **Practice** or **Timed (60 Sec Test)**.
3. Press **ENTER** to start. The timer begins at this moment.
4. Type the passage shown on screen. Use **Backspace** to correct mistakes.
5. Click **Next** to switch to another passage.
6. Click **Save Result** to store your current result in `user_stats.csv`.
   (Results are also saved automatically when a practice passage is completed or when the 60 seconds end.)

### Controls

| Input | Action |
|-------|--------|
| `Enter` | Start the session |
| Keyboard | Type the passage |
| `Backspace` | Delete the last character |
| Practice button | Switch to Practice Mode (resets session) |
| Timed / 60 Sec Test button | Switch to Timed Mode (resets session) |
| Next button | Load next passage (wraps to first after last) |
| Save Result button | Save current statistics to CSV |

---

## How the Statistics Are Calculated

- **Correct / Wrong** – each typed character is compared with the character at the same position in the passage. A match counts as correct, anything else (including extra characters) counts as wrong.
- **Accuracy (%)** = `correct characters / total typed characters × 100`
- **WPM** = `(typed characters / 5) / minutes`
  (the standard definition: 1 word = 5 characters)

---

## Project Structure

```
TypeMaster/
├── main.cpp            # SFML window, UI drawing, event handling, main loop
├── TypingSession.h     # Abstract base class for a typing session
├── PracticeMode.h      # Practice mode (derived from TypingSession)
├── TimedChallenge.h    # Timed mode (derived from TypingSession)
├── PassageManager.h    # Loads and provides passages from prompts.txt
├── Statistics.h        # Calculates WPM, accuracy, correct and wrong counts
├── FileManager.h       # Saves results to user_stats.csv
├── prompts.txt         # Typing passages (one passage per line)
└── user_stats.csv      # Saved results (created automatically)
```

### Class Overview

| Class | Responsibility |
|-------|----------------|
| `TypingSession` | Abstract base class. Stores target text, user input, correct/wrong counts. Declares the pure virtual function `startSession()`. |
| `PracticeMode` | Inherits `TypingSession`; overrides `startSession()` for untimed practice. |
| `TimedChallenge` | Inherits `TypingSession`; overrides `startSession()` and stores a time limit (default 60 s). |
| `PassageManager` | Reads `prompts.txt` into a `vector<string>`, returns a passage by index, returns the passage count. Throws exceptions on errors. |
| `Statistics` | Computes WPM, accuracy, correct and wrong characters. Uses private data with getter functions. |
| `FileManager` | Appends a result row to `user_stats.csv`. |

---

## OOP Concepts Used

- **Abstraction** – `TypingSession` is an abstract class with a pure virtual function.
- **Inheritance** – `PracticeMode` and `TimedChallenge` derive from `TypingSession`.
- **Polymorphism** – a `TypingSession*` pointer refers to either mode and calls `startSession()` through the virtual function.
- **Encapsulation** – class data is private/protected and accessed through getters and setters.
- **Constructors and virtual destructor** – proper initialization and safe cleanup in the base class.
- **Exception handling** – `try / catch` with `runtime_error` for missing files, empty passage list and invalid passage index.
- **File handling** – `ifstream` to read passages, `ofstream` (append mode) to save results.
- **Templates** – generic `toString()` function used to convert numbers to text for display.
- **STL** – `string`, `vector`, `stringstream`, `optional`.

---

## Data Files

### `prompts.txt`
One passage per line. Empty lines are ignored. Add as many passages as you like; the program uses all of them automatically.

### `user_stats.csv`
Each saved result is one row, with no header:

```
Mode,WPM,Accuracy,Correct,Wrong
```

Example:

```
Timed,30.89,100,98,0
Practice,42.5,96.3,260,10
```

---

## Requirements

- C++17 or newer compiler (g++ / MinGW recommended)
- **SFML 3.x** (the code uses the SFML 3 API, e.g. `sf::VideoMode({w, h})`, `font.openFromFile()`, `std::optional` events)
- Windows – the font is loaded from `C:/Windows/Fonts/segoeui.ttf`

> On Linux or macOS, change the font path in `main.cpp` to any `.ttf` font available on your system.

---

## Build and Run

Make sure `prompts.txt` is in the same folder as the executable.

**Using g++ (MinGW):**

```bash
g++ main.cpp -o main -std=c++17 -I<path-to-SFML>/include -L<path-to-SFML>/lib -lsfml-graphics -lsfml-window -lsfml-system
```

Then run:

```bash
./main
```

(On Windows, copy the required SFML `.dll` files next to `main.exe`.)

---

## Possible Future Improvements

- Per-user profiles and a stats/history screen that reads `user_stats.csv`
- High-score board and personal best tracking
- Difficulty levels and custom time limits (30 s / 120 s)
- Word-level wrap for the passage display
- Sound effects and themes
- Option to load custom passages from within the app

---

## Author

- **Name:** _your name_
- **Course / College:** _your course and college_
- **Year:** 2026
