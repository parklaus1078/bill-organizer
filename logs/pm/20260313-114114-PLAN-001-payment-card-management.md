# PM Log: Payment Card Management

- **Agent**: PM Agent
- **Project**: bill-organizer
- **Project Type**: desktop-app (Qt 6.x, C++)
- **Ticket Number**: PLAN-001
- **Date**: 2026-03-13 11:41:14
- **Reference Ticket**: projects/bill-organizer/planning/tickets/PLAN-001-payment-card-management.md
- **Generated Files**:
  - projects/bill-organizer/planning/specs/screens/PLAN-001-payment-card-management.md
  - projects/bill-organizer/planning/specs/screens/PLAN-001-payment-card-management.html
  - projects/bill-organizer/planning/specs/state/PLAN-001-payment-card-management.md
  - projects/bill-organizer/planning/specs/storage/PLAN-001-payment-card-management.md

---

## Request Interpretation

The ticket requests a **payment card wallet** feature that allows users to store, manage, and link payment cards to subscriptions. The core requirements are:

1. **CRUD Operations**: Add, edit, delete, and view payment cards
2. **Secure Storage**: Encrypt sensitive data (CVC) at rest using AES-256
3. **Card Details**: Store card nickname, issuer/bank, last 4 digits, expiry date, and optional CVC
4. **Card Masking**: Display only last 4 digits (never full card number)
5. **Linking**: Cards can be selected when creating/editing subscriptions
6. **Card Status**: Visual indicators for expired or expiring-soon cards

### Ambiguities Resolved:
- **Full card number storage**: Ticket mentions "last 4 digits" only, so full number is **not stored** (only last 4 digits). This is a secure-by-design decision.
- **CVC storage**: Explicitly mentioned as optional with encryption. Implemented with clear UI indication that it's optional.
- **Card validation**: Out of scope per ticket (no bank integration, no Luhn algorithm).
- **Card expiry notifications**: Out of scope (handled in PLAN-005), but visual warnings implemented in UI.

---

## Project Type-Specific Decisions

**Project Type**: `desktop-app` (Qt 6.x framework, C++)

Based on the desktop-app project type, the following deliverables were generated:

### 1. Screen Requirements (`screens/PLAN-001-payment-card-management.md`)
Comprehensive UI specification covering:
- **8 distinct screens/states**: Card list (default/empty), add/edit forms, delete confirmation, card selection mode
- **User flows**: Complete navigation paths for all CRUD operations
- **Component structure**: Detailed breakdown of UI elements, fields, buttons, validation
- **Visual states**: Hover, expired, expiring soon, loading, error states
- **Accessibility**: Keyboard navigation, screen reader support, ARIA attributes
- **Edge cases**: Duplicate detection, expired card warnings, database errors

### 2. Interactive Wireframe (`screens/PLAN-001-payment-card-management.html`)
**Type Selected**: Interactive HTML

**Rationale for Interactive HTML**:
- Form submission with validation feedback
- Multiple state transitions (list → add → save → list)
- Different visual states based on card status (expired, expiring soon)
- Modal dialogs (delete confirmation, duplicate warning)
- Card selection mode with radio buttons

**Implemented States**:
1. Card List View (with cards, including expired/expiring badges)
2. Card List View (empty state)
3. Add Card Form (with validation)
4. Edit Card Form (pre-populated, CVC shown as "•••")
5. Delete Confirmation Dialog (with subscription warning if applicable)
6. Card Selection Mode (for subscription forms)
7. Success Message/Toast
8. Duplicate Warning Dialog

**Interaction Simulations**:
- Form validation (inline error messages)
- Loading state during save
- Auto-format expiry date (MM/YY)
- CVC masking in edit mode
- Card selection with radio buttons (disabled for expired cards)

### 3. State Management Specification (`state/PLAN-001-payment-card-management.md`)
Desktop-app specific state management using **Qt's Model/View architecture**:

**Key Design Decisions**:
- **`PaymentCard`**: Value object (data model) with getters/setters
- **`PaymentCardManager`**: Singleton service class for business logic, emits Qt signals
- **`PaymentCardListModel`**: Extends `QAbstractListModel` for Qt views (QListView/QTableView)
- **`CardEncryption`**: Singleton for AES-256 encryption/decryption

**Signal/Slot Architecture**:
- Manager emits: `cardAdded()`, `cardUpdated()`, `cardDeleted()`
- Model listens and updates incrementally (no full refresh)
- UI connects to model via Qt's data binding

**Memory Management**:
- Qt object lifecycle: Singletons for managers, view-owned models
- Sensitive data: Clear plain-text CVC from memory immediately after use

### 4. Database & Encryption Specification (`storage/PLAN-001-payment-card-management.md`)
**Database**: SQLite 3 with `QtSql` module

**Schema**:
- Table: `payment_cards` with columns for all card fields
- Indexes: `(last4_digits, expiry_month, expiry_year)` for duplicate detection, `created_at` for sorting
- Future compatibility: Foreign key support for subscriptions table

**Encryption Implementation**:
- **Algorithm**: AES-256-CBC
- **Library Choice**: QCA (Qt Cryptographic Architecture) **OR** OpenSSL
  - Recommendation: QCA for simpler API
- **Key Storage**: QtKeychain (platform-specific secure storage)
  - macOS: Keychain
  - Windows: Credential Manager
  - Linux: libsecret/KWallet
- **IV Management**: Random 16-byte IV per encryption, prepended to ciphertext
- **Data Format**: `[IV (16 bytes)][Ciphertext (variable)]` stored as BLOB

**Data Access Layer**:
- `CardDatabase` class with CRUD methods
- Prepared statements (SQL injection prevention)
- Row-to-object mapping

---

## HTML Type Decision

**Selected**: Interactive HTML

**Reasoning**:
- **State Transitions**: 8 different states/screens require dynamic showing/hiding
- **Form Validation**: Real-time validation feedback with inline error messages
- **User Interactions**: Multiple user actions (add, edit, delete, select) with different outcomes
- **Modal Dialogs**: Delete confirmation and duplicate warning overlays
- **Card Selection**: Radio button selection with enable/disable logic
- **Loading States**: Asynchronous save operation simulation

**Static HTML would be insufficient for**:
- Simulating state changes after form submission
- Showing/hiding validation errors dynamically
- Modal overlay behavior
- Card selection workflow

**Implemented Interactive Features**:
1. Form submission handlers with client-side validation
2. Auto-formatting for expiry date (MM/YY)
3. Dynamic error message display/hide
4. State transitions between list, add, edit, delete
5. Loading spinner during save operation
6. Duplicate detection dialog
7. Card selection with radio buttons
8. Success toast message
9. CVC masking in edit mode ("•••")

---

## Security Decisions

### What's Encrypted:
- **CVC only**: Encrypted with AES-256-CBC
- **Rationale**: CVC is the most sensitive data; last 4 digits and expiry are commonly displayed publicly (e.g., on receipts)

### What's NOT Encrypted:
- Card nickname, issuer, last 4 digits, expiry date
- **Rationale**: These are identifying information, not sensitive enough to warrant encryption overhead

### Key Management:
- **QtKeychain** for platform-specific secure storage
- **Fallback**: If QtKeychain unavailable, warn user and store key in config with restricted permissions (chmod 600)

### Threat Model:
- **Protected Against**: Local file system access by unprivileged users
- **NOT Protected Against**: Memory dumping, malware with elevated privileges, physical access to unlocked device
- **Recommendation**: Document limitations in user guide

---

## Reviewer Notes

### Items Requiring Confirmation:

1. **Full Card Number Storage**:
   - **Decision**: Store only last 4 digits (not full number)
   - **Assumption**: Based on ticket wording "last 4 digits" and security-by-design principle
   - **Question**: Should full card number be stored (encrypted)? This would require additional changes.

2. **Encryption Library Choice**:
   - **Provided Options**: QCA (Qt Cryptographic Architecture) **OR** OpenSSL
   - **Recommendation**: QCA for easier API, but OpenSSL is more widely available
   - **Question**: Coding agent to decide based on dependencies/build environment?

3. **Duplicate Card Handling**:
   - **Decision**: Allow duplicates with user confirmation
   - **Rationale**: Users may have multiple cards with same last 4 digits/expiry (e.g., family members)
   - **Alternative**: Prevent duplicates entirely (stricter approach)

4. **Card Deletion with Subscription Links**:
   - **Decision**: Set `payment_card_id` to NULL in subscriptions (not cascade delete)
   - **Rationale**: Preserve subscription history even if card is deleted
   - **Alternative**: Prevent deletion if card has subscriptions (require user to unlink first)

5. **CVC Update Behavior in Edit Form**:
   - **Decision**: Show existing CVC as "•••" (masked), allow update or clearing
   - **Behavior**:
     - User leaves as "•••" → Keep existing encrypted value
     - User enters new value → Re-encrypt and replace
     - User clears field → Remove CVC from storage
   - **Alternative**: Always require re-entry of CVC during edit (never show "•••")

6. **Card Expiry Warning Threshold**:
   - **Decision**: "Expiring Soon" badge shown when ≤30 days until expiry
   - **Question**: Should this be configurable, or is 30 days acceptable?

### Arbitrarily Decided (Due to Ambiguity):

1. **Card Nickname Max Length**: 50 characters (reasonable for display, not specified in ticket)
2. **Issuer/Bank Max Length**: 50 characters
3. **Database File Location**: Platform-specific user data directory (standard Qt practice)
4. **Backup Strategy**: Mentioned in storage spec, but not implemented in PLAN-001 (future enhancement)
5. **Search/Filter Functionality**: Mentioned in screen spec as "optional", not detailed
6. **Card Images/Logos**: Not included (could be future enhancement)

---

## Dependencies for Coding Agent

### Required Qt Modules:
- `Qt6::Core`
- `Qt6::Sql`
- `Qt6::Widgets` (for desktop UI)

### Required External Libraries:
- **Encryption**: `qca-qt6` (Qt Cryptographic Architecture) **OR** `OpenSSL`
- **Keychain**: `qt6keychain` (QtKeychain)

### Installation Commands (Ubuntu example):
```bash
sudo apt-get install libqca-qt6-2-dev
sudo apt-get install qt6keychain-dev
```

### CMake Configuration:
```cmake
find_package(Qt6 REQUIRED COMPONENTS Core Sql Widgets)
find_package(Qca-qt6 REQUIRED)
find_package(Qt6Keychain REQUIRED)

target_link_libraries(BillOrganizer
    Qt6::Core
    Qt6::Sql
    Qt6::Widgets
    qca-qt6
    qt6keychain
)
```

---

## Next Steps for User

1. **Review Deliverables**:
   - Open HTML wireframe in browser to interact with states
   - Review screen requirements for completeness
   - Verify state management matches Qt architecture expectations
   - Check database schema and encryption approach

2. **Provide Feedback**:
   - Confirm decisions in "Reviewer Notes" section
   - Clarify any ambiguous requirements
   - Approve or request changes to deliverables

3. **Proceed to Coding**:
   - Once approved, run Backend Coding Agent to implement database and encryption layer
   - Then run Frontend Coding Agent to implement Qt UI
   - Finally run QA Agent to generate and run tests

4. **Alternative**: If changes needed, re-run PM Agent with updated ticket

---

## Metrics

- **Screens Designed**: 8 (including states and dialogs)
- **Deliverable Files**: 4 (1 screen spec, 1 wireframe, 1 state spec, 1 storage spec)
- **Database Tables**: 1 (`payment_cards`)
- **API Endpoints**: N/A (desktop app, local database only)
- **Estimated Complexity**: Small (as per ticket)
- **Implementation Time Estimate**: Not provided (per PM agent guidelines)

---

---

## Updates

### Update 1: UI Design Reference Added (2026-03-13 11:42)

**Change**: Added UI design reference to screen requirements document

**File Modified**: `projects/bill-organizer/planning/specs/screens/PLAN-001-payment-card-management.md`

**Changes Made**:
1. Added "UI Design Reference" section at top of document
2. Referenced: `projects/bill-organizer/planning/specs/screens/bill-organizer UI reference.jpg`
3. Extracted design system guidelines from reference image:
   - **Color Scheme**: Deep purple gradient background (#4A148C → #6A1B9A)
   - **Sidebar**: Dark purple left navigation (~240px width)
   - **Main Content**: Centered on gradient background
   - **Typography**: Clean, modern sans-serif
   - **Components**: Rounded corners, subtle shadows, semi-transparent backgrounds
4. Updated "Layout Guidelines" section with:
   - Application structure (sidebar + main content area)
   - Specific color palette extracted from reference
   - Layout constraints matching design language
   - Sidebar navigation integration

**Rationale**: User requested that coding agent reference the existing UI design to maintain visual consistency across the application.

**Impact on Coding**:
- Coding agent must implement Qt widgets with custom styling to match purple gradient theme
- Custom QSS (Qt Style Sheets) will be required for colors, rounded corners, shadows
- Sidebar navigation should include new "Payment Cards" item
- Forms and dialogs must use semi-transparent backgrounds on gradient
- All colors and spacing should match reference design

---

**PM Agent Work Complete** ✅

All deliverables generated and logged. UI design reference integrated. Awaiting user review and approval before proceeding to coding phase.
