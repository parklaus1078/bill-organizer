# Test Suite for PLAN-001: Payment Card Management

This directory contains comprehensive test coverage for the Payment Card Management feature.

## Test Structure

```
tests/
├── unit/                                    # Unit tests (isolated component tests)
│   ├── tst_payment_card.cpp                # PaymentCard model tests (~30 tests)
│   ├── tst_card_encryption.cpp             # Encryption service tests (~25 tests)
│   └── tst_payment_card_manager_validation.cpp  # Validation logic tests (~45 tests)
├── integration/                             # Integration tests (component interactions)
│   └── tst_card_manager_integration.cpp    # Manager + DB + Encryption tests (~60 tests)
├── ui/                                      # UI/E2E tests (user workflows)
│   └── tst_card_workflows.cpp              # Complete user workflows (~15 tests)
├── test_helpers.h                           # Shared test utilities and fixtures
└── CMakeLists.txt                           # Test build configuration
```

## Running Tests

### Prerequisites

- Qt 6.x installed
- CMake 3.16 or higher
- C++ compiler (GCC, Clang, or MSVC)

### Build Tests

```bash
# From project root
cd build
cmake ..
make

# Or build tests specifically
cd tests
cmake --build . --target all
```

### Run All Tests

```bash
# From build directory
ctest --output-on-failure

# Verbose output
ctest --output-on-failure --verbose

# Run specific test
./tests/unit/tst_payment_card
./tests/integration/tst_card_manager_integration
```

### Run Tests Headless (for CI/CD)

```bash
# Set offscreen platform
export QT_QPA_PLATFORM=offscreen

# Then run tests
ctest --output-on-failure
```

### Run with Valgrind (Memory Leak Detection)

```bash
valgrind --leak-check=full ./tests/unit/tst_payment_card
valgrind --leak-check=full ./tests/integration/tst_card_manager_integration
```

## Test Coverage

### Unit Tests (~100 test cases)

**tst_payment_card.cpp** - PaymentCard Model
- Getters and setters (TC-UNIT-001 to TC-UNIT-006)
- Masked card number formatting (TC-UNIT-010 to TC-UNIT-012)
- Expiry status checking (TC-UNIT-020 to TC-UNIT-042)
- Copy constructor and assignment operator (TC-UNIT-301 to TC-UNIT-302)
- Null and empty handling (TC-UNIT-320 to TC-UNIT-322)

**tst_card_encryption.cpp** - CardEncryption Service
- Encryption operations (TC-UNIT-050 to TC-UNIT-054)
- Decryption operations (TC-UNIT-060 to TC-UNIT-063)
- Round-trip encryption/decryption (TC-UNIT-070 to TC-UNIT-074)
- Key management (TC-UNIT-080 to TC-UNIT-082)

**tst_payment_card_manager_validation.cpp** - Validation Logic
- Nickname validation (TC-UNIT-100 to TC-UNIT-105)
- Issuer validation (TC-UNIT-110 to TC-UNIT-114)
- Last 4 digits validation (TC-UNIT-120 to TC-UNIT-128)
- Expiry date validation (TC-UNIT-130 to TC-UNIT-136)
- CVC validation (TC-UNIT-140 to TC-UNIT-146)
- Complete card validation (TC-UNIT-150 to TC-UNIT-153)

### Integration Tests (~60 test cases)

**tst_card_manager_integration.cpp** - Full Stack Integration
- Add card workflows (TC-INT-001 to TC-INT-022)
- Edit card workflows (TC-INT-050 to TC-INT-064)
- Delete card workflows (TC-INT-100 to TC-INT-103)
- Load and query operations (TC-INT-150 to TC-INT-154)
- Signal/slot integration (TC-INT-250 to TC-INT-282)

### UI/E2E Tests (~15 test cases)

**tst_card_workflows.cpp** - User Workflows
- Add card flows (TC-E2E-020 to TC-E2E-036)
- Edit card flows (TC-E2E-100 to TC-E2E-105)
- Delete card flows (TC-E2E-150 to TC-E2E-154)

## Test Naming Convention

Test functions follow the pattern: `test{Feature}_{scenario}`

Examples:
- `testValidateNickname_valid()` - Tests valid nickname validation
- `testAddCard_withCVC()` - Tests adding a card with CVC
- `testSignal_cardAdded()` - Tests cardAdded signal emission

## Coverage Goals

- **Unit Tests**: 90% code coverage
- **Integration Tests**: 85% coverage
- **Overall Target**: 80%+ coverage

## Generating Coverage Reports

```bash
# Build with coverage flags
cmake -DCMAKE_BUILD_TYPE=Debug ..
make

# Run tests
ctest

# Generate coverage report (requires lcov)
lcov --capture --directory . --output-file coverage.info
lcov --remove coverage.info '/usr/*' '*/Qt6/*' '*/tests/*' --output-file coverage.info
genhtml coverage.info --output-directory coverage_report

# View report
open coverage_report/index.html
```

## Test Data

Tests use the `TestHelpers` namespace for creating consistent test data:

- `createValidCard()` - Valid card with future expiry
- `createExpiredCard()` - Card with past expiry date
- `createExpiringSoonCard()` - Card expiring within 30 days
- `createCardWithCVC()` - Card with encrypted CVC

## Troubleshooting

### Qt Test not found

```bash
# Ensure Qt6 Test module is installed
sudo apt-get install qt6-base-dev  # Ubuntu/Debian
brew install qt6                    # macOS
```

### Tests fail to build

```bash
# Clean build
rm -rf build
mkdir build
cd build
cmake ..
make
```

### Database errors in integration tests

Integration tests use a temporary database. Ensure:
- Write permissions in temp directory
- SQLite support in Qt installation

### UI tests fail in headless environment

```bash
# Set offscreen platform
export QT_QPA_PLATFORM=offscreen
./tests/ui/tst_card_workflows
```

## CI/CD Integration

Example GitHub Actions workflow:

```yaml
name: Run Tests

on: [push, pull_request]

jobs:
  test:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      - name: Install Qt
        run: sudo apt-get install qt6-base-dev
      - name: Build
        run: |
          mkdir build && cd build
          cmake ..
          make
      - name: Run Tests
        run: |
          export QT_QPA_PLATFORM=offscreen
          cd build
          ctest --output-on-failure
```

## Contributing

When adding new tests:
1. Follow existing naming conventions
2. Add test to appropriate category (unit/integration/ui)
3. Update CMakeLists.txt if adding new test file
4. Document test coverage in this README
5. Ensure tests are independent and can run in any order

## References

- [Qt Test Documentation](https://doc.qt.io/qt-6/qtest-overview.html)
- [Qt Test Best Practices](https://doc.qt.io/qt-6/qttest-best-practices-qdoc.html)
- Test case specifications: `../planning/specs/test-cases/PLAN-001-*.md`
