# PM Log: Payment Card Management

- **Agent**: PM Agent
- **Project**: bill-organizer
- **Project Type**: desktop-app (Qt 6.x, C++)
- **Ticket Number**: PLAN-001
- **Date**: 2026-03-13 14:41:16
- **Reference Ticket**: projects/bill-organizer/planning/tickets/PLAN-001-payment-card-management.md
- **Generated Files**:
  - projects/bill-organizer/planning/specs/test-cases/PLAN-001-unit.md
  - projects/bill-organizer/planning/specs/test-cases/PLAN-001-integration.md
  - projects/bill-organizer/planning/specs/test-cases/PLAN-001-e2e.md

---

## Request Interpretation

This task was to **complete missing test case deliverables** for PLAN-001 (Payment Card Management).

**Existing deliverables** (already created in previous session):
- Screen requirements (screens/PLAN-001-payment-card-management.md)
- Interactive wireframe (screens/PLAN-001-payment-card-management.html)
- State management specification (state/PLAN-001-payment-card-management.md)
- Database & encryption specification (storage/PLAN-001-payment-card-management.md)

**Missing deliverables** identified:
- Unit test cases
- Integration test cases
- End-to-end test cases

The user explicitly requested **"Tests ONLY"**, so I generated only the three missing test case files.

---

## Project Type-Specific Decisions

**Project Type**: desktop-app (Qt 6.x C++)

For desktop applications, the standard test deliverables are:
1. **Unit tests** (`test-cases/PLAN-001-unit.md`)
2. **Integration tests** (`test-cases/PLAN-001-integration.md`)
3. **E2E tests** (`test-cases/PLAN-001-e2e.md`)

All three were generated according to the desktop-app project type template.

---

## Test Case Coverage Decisions

### Unit Tests (PLAN-001-unit.md)
**Scope**: Individual components in isolation
- **PaymentCard model**: Getters/setters, derived properties (getMaskedCardNumber, isExpired, isExpiringSoon, getExpiryStatus)
- **CardEncryption**: Encryption/decryption operations, round-trip tests, key management
- **PaymentCardManager validation**: All validation rules for nickname, issuer, last4, expiry, CVC
- **PaymentCardManager duplicate detection**: isDuplicate() logic
- **CardDatabase**: Initialization, row mapping, edge cases
- **Total test cases**: ~100+ individual unit test cases

**Test framework**: Qt Test (QTest)
**Database for tests**: In-memory SQLite (`:memory:`)

### Integration Tests (PLAN-001-integration.md)
**Scope**: Component interactions (Manager + Database + Encryption)
- **Add card flows**: Normal cases, duplicate detection, encryption verification, error cases
- **Edit card flows**: Normal updates, CVC management (add/update/remove/keep), duplicate detection
- **Delete card flows**: With/without subscriptions, subscription count checks
- **Load and query flows**: Load all, load by ID, active/expired card queries
- **Encryption integration**: Key initialization, round-trip with database, decryption errors
- **Signal/slot integration**: cardAdded, cardUpdated, cardDeleted, operationFailed signals
- **PaymentCardListModel integration**: Model refresh on state changes, data integrity, filtering
- **Database transactions**: Rollback/commit, concurrent access
- **Total test cases**: ~85+ integration test cases

**Test framework**: Qt Test with real SQLite database (not in-memory)
**Monitoring**: QSignalSpy for signal verification

### E2E Tests (PLAN-001-e2e.md)
**Scope**: Complete user workflows from UI to database
- **Application launch**: Empty state, with existing cards, card display states (expired/expiring/active)
- **Add card workflows**: Success flow, validation errors, client-side validation, cancel, duplicates, loading state
- **Edit card workflows**: Success flow, CVC management (view/add/update/keep/remove), validation, cancel, delete from edit form
- **Delete card workflows**: Without subscriptions, with subscriptions, edge cases
- **Card selection**: From subscription forms, expired card handling, add new card from selection
- **Search and filter**: Search by nickname/issuer, no results, clear search
- **Keyboard navigation & accessibility**: Tab navigation, Enter/Escape keys, screen reader support, focus indicators
- **Error handling**: Database errors, encryption errors, system issues (low disk space, crashes)
- **Multi-window**: State synchronization, backgrounding/restore
- **Visual regression**: UI consistency with design reference (purple gradient, sidebar, rounded cards)
- **Total test cases**: ~110+ E2E test cases

**Test framework**: Qt Test with QtQuickTest for UI automation
**Platform**: Cross-platform (Windows/macOS/Linux)

---

## Key Testing Decisions

### 1. Test Pyramid Balance
- **Unit tests (~100)**: Largest layer, fast execution (<5 seconds)
- **Integration tests (~85)**: Middle layer, moderate execution (<30 seconds)
- **E2E tests (~110)**: Top layer but comprehensive due to desktop UI complexity (<5 minutes)

### 2. CVC Testing Strategy
- **Unit**: Encryption/decryption algorithms, validation rules
- **Integration**: Round-trip encryption with database, key management
- **E2E**: User workflows for adding/updating/removing CVC through UI

### 3. Duplicate Detection Testing
- **Unit**: isDuplicate() function logic with various combinations
- **Integration**: Database queries for duplicate detection
- **E2E**: User experience when attempting to add duplicate cards

### 4. Expiry Date Testing
- **Unit**: isExpired(), isExpiringSoon(), getExpiryStatus() logic
- **Integration**: Query active/expired cards from database
- **E2E**: Visual display of expired/expiring cards with warning badges

### 5. Encryption Security Testing
- **Unit**: Basic encrypt/decrypt correctness
- **Integration**: Verify CVC never stored plain-text in database, different IVs per encryption
- **E2E**: Verify CVC never appears in logs, UI always masks CVC

### 6. Signal/Slot Testing (Qt-specific)
- **Integration layer only**: Use QSignalSpy to verify cardAdded, cardUpdated, cardDeleted signals
- Connected to PaymentCardListModel synchronization tests

### 7. Accessibility Testing
- **E2E layer**: Keyboard navigation (Tab, Enter, Escape), screen reader announcements (role="alert"), focus indicators
- Ensures compliance with desktop accessibility standards

---

## Reviewer Notes

### Assumptions Made

1. **Test Framework**: Assumed Qt Test (QTest) as it's the standard for Qt/C++ projects. If the project uses a different framework (e.g., Google Test), test case descriptions remain valid but setup code would differ.

2. **UI Test Automation**: Assumed QtQuickTest or similar Qt-based UI automation. If using QWidgets instead of QML, test automation approach may need adjustment.

3. **Subscription Table**: Referenced `subscriptions` table for subscription link tests (TC-INT-110, TC-E2E-160), but this table is not yet implemented (part of future PLAN-002). These test cases can be deferred until subscription feature is implemented.

4. **Keychain Library**: Test cases assume qtkeychain is used for encryption key storage. If fallback file storage is used instead, some integration tests (TC-INT-200-202) may need modification.

5. **Current Date in Tests**: Assumed test helpers use injectable clock/mock date for date-dependent tests (expiry validation). Implementation should use dependency injection for QDate::currentDate() to enable deterministic testing.

6. **Search Feature**: Search tests (TC-E2E-250-254) assume search functionality is implemented. The screen spec mentions search as optional; if not implemented, these test cases can be removed.

### Items Requiring Confirmation

1. **Test Coverage Target**: Document specifies 80% line coverage for unit tests. Confirm if this aligns with project standards or if different coverage threshold is required.

2. **Cross-Platform Testing**: E2E tests specify testing on Windows/macOS/Linux. Confirm which platforms are priority for initial release.

3. **Visual Regression Testing**: E2E tests reference "bill-organizer UI reference.jpg" for design consistency. Confirm if visual regression testing tools (e.g., screenshot comparison) should be integrated.

4. **Performance Tests**: No dedicated performance test cases included. Consider adding if performance is critical (e.g., loading 10,000+ cards, encryption speed benchmarks).

5. **Concurrency Tests**: Only basic concurrent access test included (TC-INT-360-361). If multi-threaded card operations are expected, expand concurrency test coverage.

6. **Database Migration Tests**: Storage spec mentions schema versioning. If migrations are needed, add migration test cases to integration suite.

### Test Data Management

**Recommendation**: Create test data builders/fixtures for common scenarios:
```cpp
// Suggested test helpers
PaymentCard createValidCard();          // Future-dated, all fields valid
PaymentCard createExpiredCard();        // Past expiry date
PaymentCard createExpiringSoonCard();   // Expiry within 30 days
PaymentCard createCardWithCVC();        // Includes encrypted CVC
```

This reduces test code duplication and improves maintainability.

### Known Test Gaps

1. **Boundary Testing**: Limited boundary tests for date edge cases (e.g., leap years, year 2099, month/year rollovers). Consider expanding if strict date validation is critical.

2. **Localization**: No tests for internationalization (e.g., date format in different locales). Add if app supports multiple languages/regions.

3. **Memory Leak Tests**: Mentioned in success criteria but no specific test cases. Consider adding Valgrind-based memory leak detection to CI pipeline.

4. **Security Penetration Tests**: Basic encryption tests included, but no adversarial testing (e.g., SQL injection attempts, memory dumping). Consider security audit if handling real payment data.

---

## Next Steps for Coding Agent

When implementing based on these test cases:

1. **Start with Unit Tests**: Implement PaymentCard model and CardEncryption first, with corresponding unit tests (TDD approach).

2. **Database Layer**: Implement CardDatabase with integration tests.

3. **Manager Layer**: Implement PaymentCardManager with validation logic, tested via unit + integration tests.

4. **UI Layer**: Implement Qt UI components (QML or QWidgets), verified with E2E tests.

5. **Test Automation Setup**:
   - Configure Qt Test framework in CMake
   - Set up in-memory database for unit tests
   - Configure test database for integration tests
   - Set up UI test automation (QtQuickTest or QTest GUI testing)

6. **CI/CD Integration**: Add test runs to continuous integration pipeline with coverage reporting.

---

## Compliance with PM Agent Mandate

✅ **Rate limit check**: Performed at start (4/5 executions remaining)
✅ **Project config check**: Read `.project-config.json` and `.project-meta.json`
✅ **Project type verification**: Confirmed desktop-app type
✅ **Existing deliverables check**: Identified existing screen/state/storage specs
✅ **Deliverable generation**: Created all three test case files per desktop-app template
✅ **Log writing**: This log file created immediately after deliverable generation
✅ **User guidance**: Next steps documented above

**Work completed successfully**: All missing test case deliverables for PLAN-001 generated.
