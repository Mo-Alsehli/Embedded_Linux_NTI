# C++ Smart Wallet Application

A comprehensive digital wallet application built in C++ that provides secure financial management capabilities with modern smart features.

## C++ Features Used in Smart Wallet Application

## Object-Oriented Programming
- **Classes** - Wallet, Account, Transaction classes
- **Inheritance** - Different account types inherit from base Account class
- **Polymorphism** - Virtual functions for different transaction types
- **Encapsulation** - Private data with public methods

## Modern C++ Features
- **Smart Pointers** - `std::unique_ptr`, `std::shared_ptr` for memory management
- **Auto Keyword** - Automatic type detection
- **Range-based Loops** - `for (auto& item : container)`
- **Lambda Functions** - For sorting and filtering data
- **Move Semantics** - Efficient object transfers

## STL Containers
- **std::vector** - For lists of transactions and accounts
- **std::map** - For account lookups by ID
- **std::string** - For text handling

## Templates
- **Template Classes** - Generic wallet for different currencies
- **Template Functions** - Reusable functions for different data types

## Exception Handling
- **try-catch blocks** - Handle errors safely
- **Custom Exceptions** - InsufficientFunds, InvalidAccount errors

## File I/O
- **std::ifstream/ofstream** - Read/write wallet data to files
- **File streams** - Save and load account information

## Memory Management
- **RAII** - Automatic cleanup when objects go out of scope
- **No manual new/delete** - Smart pointers handle memory

## Other Features
- **Operator Overloading** - Custom + and - for money operations
- **Const Correctness** - Proper use of const methods
- **Enums** - Transaction types (DEPOSIT, WITHDRAWAL, TRANSFER)

## 🛠️ Technology Stack

- **Language**: C++
- **Standard**: C++17 or higher
- **Build System**: CMake (recommended) or Makefile

## 📋 Prerequisites

Before building and running this application, ensure you have:

- C++ compiler (GCC 7.0+, Clang 5.0+, or MSVC 2017+)
- CMake 3.10 or higher
- Make utility
- Git for version control

## 🔧 Installation

### Clone the Repository

```bash
git clone https://github.com/Mo-Alsehli/Cpp_Smart_Wallet_Application.git
cd Cpp_Smart_Wallet_Application
```

### Build Instructions


#### Using Make

```bash
make build
```

### Run the Application

```bash
make run
```

## 📖 Usage

### Starting the Application

1. Launch the application from the command line
2. Create a new wallet or login to existing account
3. Set up security credentials (PIN/password)

### Basic Operations

#### Create Account
```
1. Select "Sign-up"
2. Enter account details
3. Set initial balance

```

#### Make Transaction
```
1. Select "Transfer Money"
2. Choose transaction type (Deposit/Withdrawal/Transfer)
3. Enter amount and description
4. Confirm transaction
```

#### View Balance and History
```
1. Select "View Balance"
```

## 🏗️ Project Structure

```
Cpp_Smart_Wallet_Application/
├── src/                    # Source code files
│   ├── main.cpp           # Main application entry point
│   ├── wallet.h/.cpp      # Core wallet functionality
│   ├── account.h/.cpp     # Account management
│   ├── transaction.h/.cpp # Transaction processing
│   ├── security.h/.cpp    # Security and encryption
│   └── utils.h/.cpp       # Utility functions
├── include/               # Header files
├── tests/                 # Unit tests
├── docs/                  # Documentation
├── data/                  # Data storage directory
├── CMakeLists.txt        # CMake configuration
├── Makefile              # Make configuration
└── README.md             # This file
```

## 👨‍💻 Author

**Mo Alsehli**
- GitHub: [@Mo-Alsehli](https://github.com/Mo-Alsehli)

