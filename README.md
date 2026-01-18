<p align="center">
  <img src="assets/Brew.png" width="250" height="250" alt="Brew Logo">
</p>

<p align="center">
  <strong>The Ultimate CLI Snippet Manager for Developers</strong>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/Version-1.0.0-blue.svg" alt="Version">
  <img src="https://img.shields.io/badge/License-MIT-green.svg" alt="License">
  <img src="https://img.shields.io/badge/Platform-Windows-lightgrey.svg" alt="Platform">
  <img src="https://img.shields.io/badge/Language-C++17-orange.svg" alt="Language">
</p>

---

##What is Brew?

**Brew** is a lightweight, blazing-fast command-line tool designed to help developers manage, visualize, and sync code snippets across multiple languages.

Unlike traditional snippet managers, Brew allows you to:

* Pull documentation directly from GitHub
* Render Markdown with terminal syntax highlighting
* Copy clean, extracted code directly to your clipboard

---

##Key Features

* **Smart Markdown Parsing**
  Automatically extracts pure code from `.md` files, ignoring headers and explanations when copying to the clipboard.

* **Rich Terminal Rendering**
  Built-in Markdown viewer that displays documentation with colors and formatting directly in your console.

* **Alias System**
  Create personalized shortcuts for your most-used snippets
  Example: `brew get jsd` instead of `brew get js-debounce`

* **Cloud Sync**
  Sync your local database with a remote GitHub repository in a single command.

* **Cross-Language Support**
  Organized inventory for C++, Python, JavaScript, Rust, SQL, and more.

---

##Installation

### Prerequisites

* Windows 10 / 11
* OpenSSL DLLs (for HTTPS support)

### Setup

1. Download the latest `brew.exe` from the **Releases** page.
2. Add the folder containing `brew.exe` to your system `PATH`.
3. Initialize Brew:

```bash
brew sync
```

---

##Usage

### Syncing & Discovery

```bash
# Sync your local index with the GitHub repository
brew sync

# List all available snippets grouped by language
brew list

# Search for a snippet by name or description
brew search debounce
```

### Fetching & Viewing

```bash
# Preview a snippet with Markdown rendering
brew view js-debounce

# Copy only the code block to your clipboard
brew get js-debounce

# Create an alias for faster access
brew alias js-debounce jsd
brew get jsd
```

### Utilities

```bash
# Show usage statistics
brew stats

# Open the snippets repository in your browser
brew browse

# Clear all local cache
brew purge
```

---

##Repository Structure

Brew expects an `index.json` file in the repository root with the following format:

```json
{
  "js-debounce": {
    "path": "javascript/debounce.md",
    "description": "High-performance debounce function",
    "language": "javascript"
  }
}
```

---

##Built With

* **CLI11** – Robust command-line parsing
* **nlohmann/json** – Seamless JSON manipulation
* **httplib** – High-performance HTTP/HTTPS requests
* **Rang** – Terminal colors
* **Clip** – Cross-platform clipboard support

---

##License

This project is licensed under the **MIT License**.
See the `LICENSE` file for details.
