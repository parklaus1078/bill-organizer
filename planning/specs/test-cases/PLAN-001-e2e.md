# Payment Card Management - E2E Test Cases

## Overview
End-to-end test cases for payment card management, testing complete user workflows from UI interaction through to database persistence.

---

## 1. Application Launch and Card List Tests

### 1.1 Launch Application - Empty State

| ID | Scenario | User Action | Expected Result |
|----|----------|-------------|-----------------|
| TC-E2E-001 | Launch app with no cards | Open Bill Organizer app → Navigate to Payment Cards section | Empty state displayed: "No payment cards saved" message, "Add Your First Card" button visible |
| TC-E2E-002 | Empty state UI elements | View empty state | Icon/illustration (💳), message, subtext "Add a card to link it to your subscriptions", button present |
| TC-E2E-003 | Click "Add Card" from empty state | Click "Add Your First Card" button | Navigate to Add Card Form |

### 1.2 Launch Application - With Existing Cards

| ID | Scenario | User Action | Expected Result |
|----|----------|-------------|-----------------|
| TC-E2E-010 | Launch app with 3 saved cards | Open app → Navigate to Payment Cards | Card list displays all 3 cards with masked numbers, issuers, expiry dates |
| TC-E2E-011 | Cards ordered by creation date | View card list with multiple cards | Cards ordered newest first (by created_at DESC) |
| TC-E2E-012 | Expired card displayed with warning | View list with expired card (expiry 01/25) | Card has red background, "Expired" badge visible |
| TC-E2E-013 | Expiring soon card displayed with warning | View list with card expiring in 20 days | Card has yellow "Expiring Soon" badge |
| TC-E2E-014 | Active card displayed normally | View list with future-dated card | Card displayed normally, no warning badges |

---

## 2. Add Card - Complete Workflows

### 2.1 Add Card - Success Flow

| ID | Scenario | User Action | Expected Result |
|----|----------|-------------|-----------------|
| TC-E2E-020 | Complete add card flow without CVC | Click "Add Card" → Enter nickname "My Visa", issuer "Chase", last4 "1234", expiry "12/28" → Click "Save" | Card saved, success message displayed, navigate back to card list, new card visible in list |
| TC-E2E-021 | Add card with CVC | Click "Add Card" → Fill all fields including CVC "123" → Click "Save" | Card saved with encrypted CVC, success toast shown, card appears in list |
| TC-E2E-022 | Add card and verify persistence | Add card → Close app → Reopen app → Navigate to Payment Cards | Previously added card still present in list |
| TC-E2E-023 | Add multiple cards sequentially | Add card 1 → Return to list → Add card 2 → Return to list → Add card 3 | All 3 cards displayed in list, ordered correctly |
| TC-E2E-024 | Add card with 4-digit CVC | Add card with CVC "1234" → Save | Card saved successfully, CVC masked as "•••" if viewed later |

### 2.2 Add Card - Validation Errors

| ID | Scenario | User Action | Expected Result |
|----|----------|-------------|-----------------|
| TC-E2E-030 | Submit form with empty required fields | Click "Add Card" → Click "Save" without filling fields | Inline error messages appear: "Card Nickname is required", "Issuer/Bank is required", etc. Form not submitted |
| TC-E2E-031 | Enter invalid last 4 digits (3 digits) | Enter nickname, issuer → Enter "123" in last4 field → Click Save | Error message: "Must be exactly 4 digits", form not submitted |
| TC-E2E-032 | Enter invalid last 4 digits (letters) | Enter "12ab" in last4 field → Click Save | Error message: "Must be exactly 4 digits", red border on field |
| TC-E2E-033 | Enter past expiry date | Enter expiry "01/25" (past date) → Click Save | Error message: "Expiry date must be in the future", form not submitted |
| TC-E2E-034 | Enter invalid expiry format | Enter expiry "13/28" (invalid month) → Click Save | Error message: "Please enter a valid expiry date", form not submitted |
| TC-E2E-035 | Enter invalid CVC (2 digits) | Enter CVC "12" → Click Save | Error message: "CVC must be 3-4 digits", form not submitted |
| TC-E2E-036 | Enter invalid CVC (letters) | Enter CVC "12a" → Click Save | Error message: "CVC must be 3-4 digits", form not submitted |

### 2.3 Add Card - Client-Side Validation

| ID | Scenario | User Action | Expected Result |
|----|----------|-------------|-----------------|
| TC-E2E-040 | Save button disabled when form empty | Open Add Card Form | "Save" button disabled/grayed out |
| TC-E2E-041 | Save button enabled when required fields filled | Enter nickname, issuer, last4, expiry (all valid) | "Save" button enabled |
| TC-E2E-042 | Real-time validation on last4 field | Type "12ab" in last4 field → Tab to next field | Error message appears immediately below field |
| TC-E2E-043 | Real-time validation on expiry field | Type "13/28" in expiry field → Tab away | Error message appears: "Please enter a valid expiry date" |
| TC-E2E-044 | Auto-format expiry date | Type "1228" in expiry field | Auto-formatted to "12/28" |

### 2.4 Add Card - Cancel Flow

| ID | Scenario | User Action | Expected Result |
|----|----------|-------------|-----------------|
| TC-E2E-050 | Cancel add card without entering data | Click "Add Card" → Immediately click "Cancel" | Return to card list, no card added |
| TC-E2E-051 | Cancel add card after entering data | Click "Add Card" → Fill form → Click "Cancel" | Return to card list, no card saved, entered data discarded |
| TC-E2E-052 | Cancel via Escape key | Open Add Card Form → Press Escape | Return to card list (if keyboard shortcut implemented) |

### 2.5 Add Card - Duplicate Detection

| ID | Scenario | User Action | Expected Result |
|----|----------|-------------|-----------------|
| TC-E2E-060 | Add duplicate card | Add card with last4="1234", expiry="12/28" → Add another with same details → Click Save | Warning dialog: "A card with these details already exists. Do you want to add it anyway?" with existing card details shown |
| TC-E2E-061 | Proceed with duplicate | Duplicate warning shown → Click "Add Anyway" | Card saved as separate entry, both cards appear in list |
| TC-E2E-062 | Cancel duplicate | Duplicate warning shown → Click "Cancel" | Return to form, no card added |
| TC-E2E-063 | No duplicate warning for different expiry | Add 1234/12/28 → Add 1234/11/28 | No warning shown, both cards saved (not considered duplicates) |

### 2.6 Add Card - Loading State

| ID | Scenario | User Action | Expected Result |
|----|----------|-------------|-----------------|
| TC-E2E-070 | View loading state during save | Fill form → Click "Save" | "Save" button shows loading spinner, button disabled during save operation |
| TC-E2E-071 | Loading state resolves to success | Click Save → Wait for completion | Loading spinner disappears, success message shown, navigate to list |

---

## 3. Edit Card - Complete Workflows

### 3.1 Edit Card - Success Flow

| ID | Scenario | User Action | Expected Result |
|----|----------|-------------|-----------------|
| TC-E2E-100 | Open edit form | Click on a card in the list → Or click "Edit" button | Edit Card Form opens, pre-populated with existing card data |
| TC-E2E-101 | Edit card nickname | Open edit form → Change nickname "My Visa" to "Personal Visa" → Click "Save" | Card updated, success message shown, return to list, nickname changed in list |
| TC-E2E-102 | Edit card issuer | Edit form → Change issuer → Save | Issuer updated in database and displayed in list |
| TC-E2E-103 | Edit card last 4 digits | Edit form → Change last4 "1234" to "5678" → Save | Last 4 digits updated, masked number shows "•••• 5678" in list |
| TC-E2E-104 | Edit card expiry date | Edit form → Change expiry "12/28" to "06/29" → Save | Expiry date updated in card list |
| TC-E2E-105 | Edit multiple fields simultaneously | Edit nickname, issuer, and expiry → Save | All fields updated correctly |

### 3.2 Edit Card - CVC Management

| ID | Scenario | User Action | Expected Result |
|----|----------|-------------|-----------------|
| TC-E2E-110 | View existing CVC (masked) | Open edit form for card with CVC | CVC field shows "•••" (masked), tooltip shows "Leave as ••• to keep current CVC" |
| TC-E2E-111 | Add CVC to card without CVC | Open edit form for card without CVC → Enter CVC "123" → Save | CVC encrypted and saved, card now has CVC stored |
| TC-E2E-112 | Update existing CVC | Edit card with CVC → Replace "•••" with "456" → Save | Old CVC replaced with new encrypted "456" |
| TC-E2E-113 | Keep existing CVC unchanged | Edit form with CVC "•••" → Edit nickname only, don't touch CVC field → Save | CVC preserved unchanged, other fields updated |
| TC-E2E-114 | Remove CVC from card | Edit form with CVC "•••" → Clear CVC field (delete "•••") → Save | CVC removed from database, hasCVC = false |

### 3.3 Edit Card - Validation Errors

| ID | Scenario | User Action | Expected Result |
|----|----------|-------------|-----------------|
| TC-E2E-120 | Clear required field | Edit form → Clear nickname field → Click Save | Error: "Card Nickname is required", form not submitted |
| TC-E2E-121 | Enter invalid last4 | Edit form → Change last4 to "12a" → Save | Error: "Must be exactly 4 digits", form not submitted |
| TC-E2E-122 | Enter past expiry date | Edit form → Change expiry to past date → Save | Error: "Expiry date must be in the future" |

### 3.4 Edit Card - Cancel Flow

| ID | Scenario | User Action | Expected Result |
|----|----------|-------------|-----------------|
| TC-E2E-130 | Cancel edit without changes | Open edit form → Immediately click "Cancel" | Return to card list, no changes saved |
| TC-E2E-131 | Cancel edit after making changes | Edit form → Change nickname → Click "Cancel" | Return to list, changes discarded, original data preserved |

### 3.5 Edit Card - Delete from Edit Form

| ID | Scenario | User Action | Expected Result |
|----|----------|-------------|-----------------|
| TC-E2E-140 | Click delete button in edit form | Open edit form → Click "Delete Card" button | Delete confirmation dialog appears |
| TC-E2E-141 | Complete delete from edit form | Edit form → Delete → Confirm | Card deleted, return to card list, card removed from list |

---

## 4. Delete Card - Complete Workflows

### 4.1 Delete Card - Without Subscriptions

| ID | Scenario | User Action | Expected Result |
|----|----------|-------------|-----------------|
| TC-E2E-150 | Initiate delete from list | Click delete icon on card in list | Delete confirmation dialog appears |
| TC-E2E-151 | View delete confirmation dialog | Delete dialog shown | Shows card nickname, last 4 digits "•••• 1234", warning "This action cannot be undone." |
| TC-E2E-152 | Confirm delete | Delete dialog → Click "Delete" button | Card deleted from database, success message shown, card removed from list |
| TC-E2E-153 | Delete and verify persistence | Delete card → Close app → Reopen → Check card list | Deleted card not present |
| TC-E2E-154 | Cancel delete | Delete dialog → Click "Cancel" | Dialog closes, no changes, card remains in list |

### 4.2 Delete Card - With Subscriptions

| ID | Scenario | User Action | Expected Result |
|----|----------|-------------|-----------------|
| TC-E2E-160 | Delete card linked to subscriptions | Card linked to 2 subscriptions → Click delete | Confirmation dialog shows additional warning: "⚠️ This card is used by 2 subscription(s). Deleting it will not affect existing subscriptions, but you cannot reuse this card." |
| TC-E2E-161 | Confirm delete with subscriptions | Delete dialog with subscription warning → Click "Delete" | Card deleted, subscriptions' payment_card_id set to NULL, subscriptions still exist |
| TC-E2E-162 | Cancel delete with subscriptions | Delete dialog with warning → Click "Cancel" | Dialog closes, card and subscription links preserved |

### 4.3 Delete Card - Edge Cases

| ID | Scenario | User Action | Expected Result |
|----|----------|-------------|-----------------|
| TC-E2E-170 | Delete last remaining card | Delete the only card in list | Card deleted, empty state appears: "No payment cards saved" |
| TC-E2E-171 | Delete middle card from list of 3 | Have 3 cards → Delete card #2 | Card #2 removed, cards #1 and #3 remain in list |

---

## 5. Card Selection - From Subscription Forms

### 5.1 Card Selection Flow

| ID | Scenario | User Action | Expected Result |
|----|----------|-------------|-----------------|
| TC-E2E-200 | Open card selection dialog | From subscription form → Click "Select Payment Card" | Card selection dialog appears with list of saved cards |
| TC-E2E-201 | View card selection list | Open selection dialog | Cards displayed with radio buttons, simplified view (nickname, issuer, last4, expiry) |
| TC-E2E-202 | Select a card | Click on card item in selection list | Radio button checked, "Select" button enabled |
| TC-E2E-203 | Confirm card selection | Select card → Click "Select" button | Card ID returned to subscription form, dialog closes |
| TC-E2E-204 | Cancel card selection | Open selection → Click "Cancel" | Dialog closes, no card selected |

### 5.2 Card Selection - Expired Cards

| ID | Scenario | User Action | Expected Result |
|----|----------|-------------|-----------------|
| TC-E2E-210 | View expired card in selection | Open selection with expired card present | Expired card displayed grayed out/disabled with "Expired" badge |
| TC-E2E-211 | Attempt to select expired card | Click on expired card in selection | Card not selectable (radio button disabled or warning shown) |
| TC-E2E-212 | Warning for expiring soon card | Select card expiring in 15 days | Yellow "Expiring Soon" badge shown, card selectable |

### 5.3 Card Selection - Add New Card

| ID | Scenario | User Action | Expected Result |
|----|----------|-------------|-----------------|
| TC-E2E-220 | Click "Add New Card" from selection | Selection dialog → Click "+ Add New Card" link | Add Card Form opens (in modal/overlay) |
| TC-E2E-221 | Add card from selection dialog | Add New Card → Fill form → Save | Card added, return to selection dialog, new card appears in list |
| TC-E2E-222 | Cancel add from selection | Add New Card → Cancel | Return to selection dialog, no card added |

---

## 6. Search and Filter Tests

### 6.1 Search Cards

| ID | Scenario | User Action | Expected Result |
|----|----------|-------------|-----------------|
| TC-E2E-250 | Search by card nickname | Enter "Visa" in search bar | Only cards with "Visa" in nickname displayed |
| TC-E2E-251 | Search by issuer | Enter "Chase" in search bar | Only cards from "Chase" displayed |
| TC-E2E-252 | Search with no results | Enter "NonExistent" in search | Empty state or "No cards found" message |
| TC-E2E-253 | Clear search | Search for "Visa" → Clear search field | All cards displayed again |
| TC-E2E-254 | Search with partial match | Enter "Vis" | Cards with "Visa" in nickname displayed |

---

## 7. Keyboard Navigation and Accessibility Tests

### 7.1 Keyboard Navigation

| ID | Scenario | User Action | Expected Result |
|----|----------|-------------|-----------------|
| TC-E2E-300 | Tab through add card form | Open Add Card Form → Press Tab repeatedly | Focus moves through: nickname → issuer → last4 → expiry → CVC → Save → Cancel |
| TC-E2E-301 | Submit form with Enter key | Fill all required fields → Press Enter | Form submitted, card saved |
| TC-E2E-302 | Close dialog with Escape | Open delete confirmation → Press Escape | Dialog closes, no action taken |
| TC-E2E-303 | Navigate card list with keyboard | Card list focused → Arrow keys | Focus moves between card items |

### 7.2 Accessibility

| ID | Scenario | User Action | Expected Result |
|----|----------|-------------|-----------------|
| TC-E2E-310 | Screen reader announces error messages | Enter invalid data → Trigger validation error | Error message announced with role="alert" |
| TC-E2E-311 | Screen reader reads card details | Focus on card item in list → Screen reader | Reads: "My Visa, Chase, ends in 1234, expires 12/28" |
| TC-E2E-312 | Focus indicators visible | Tab through form | All focused elements have visible focus ring/indicator |
| TC-E2E-313 | Form labels associated with inputs | Inspect form elements | All inputs have associated <label> or aria-label |

---

## 8. Error Handling and Edge Cases

### 8.1 Database Errors

| ID | Scenario | User Action | Expected Result |
|----|----------|-------------|-----------------|
| TC-E2E-350 | Handle database write failure | Simulate DB connection failure → Add card | Error message: "Failed to save card. Please try again.", card not added, stay on form |
| TC-E2E-351 | Handle database read failure | Simulate DB failure → Open card list | Error message: "Unable to load cards. Please check database connection." |

### 8.2 Encryption Errors

| ID | Scenario | User Action | Expected Result |
|----|----------|-------------|-----------------|
| TC-E2E-360 | Handle missing encryption key | Delete encryption key → Add card with CVC | Error or warning: "Encryption key not found. Reinitialize?", allow user to continue without CVC or reinitialize |
| TC-E2E-361 | Handle decryption failure | Corrupt encrypted CVC in DB → Load card | Card loads but CVC shows "Unable to decrypt", allow user to update CVC |

### 8.3 Network/System Issues

| ID | Scenario | User Action | Expected Result |
|----|----------|-------------|-----------------|
| TC-E2E-370 | Low disk space | Add card with very low disk space | Error: "Insufficient disk space", card not saved |
| TC-E2E-371 | App crash during save | Fill form → Click Save → Simulate app crash | On restart, card either saved completely or not at all (no partial save) |

---

## 9. Multi-Window and State Synchronization Tests

### 9.1 Window Focus and State

| ID | Scenario | User Action | Expected Result |
|----|----------|-------------|-----------------|
| TC-E2E-400 | Return to app after backgrounding | Add card → Minimize app → Restore | Card still visible in list, no state lost |
| TC-E2E-401 | Multiple form windows (if supported) | Open Add Card Form → Open Edit Form simultaneously | Both forms function independently, no state conflicts |

---

## 10. Visual Regression Tests

### 10.1 UI Consistency

| ID | Scenario | User Action | Expected Result |
|----|----------|-------------|-----------------|
| TC-E2E-450 | Card list matches design reference | View card list with cards | UI matches bill-organizer UI reference.jpg: purple gradient background, sidebar with white text, rounded cards |
| TC-E2E-451 | Add card form matches design | Open Add Card Form | Form styled consistently: rounded inputs, purple theme, proper spacing |
| TC-E2E-452 | Delete dialog matches design | Open delete confirmation | Modal overlay with semi-transparent background, centered white dialog with rounded corners |
| TC-E2E-453 | Success toast appears correctly | Save card | Green success toast appears in top-right corner, auto-dismisses after 3 seconds |

---

## Test Environment Setup

### Required Setup
- **Platform**: Desktop (Windows/macOS/Linux)
- **Qt Version**: Qt 6.x
- **Test Framework**: Qt Test with QtQuickTest for UI testing
- **Automation**: Qt Test or third-party UI automation tools
- **Test Data**: Clean test database for each test run

### Test Database
- Use separate test database file (not production)
- Reset database state between tests
- Seed test data as needed (e.g., cards with various expiry states)

### Screenshot/Recording
- Capture screenshots on test failure for debugging
- Record test runs for visual regression testing

---

## Success Criteria

- **All E2E tests pass**: 100% pass rate
- **User workflows complete**: All critical user journeys tested end-to-end
- **UI matches design**: Visual consistency with reference design
- **Accessibility verified**: Keyboard navigation and screen reader support working
- **Error handling tested**: All error scenarios handled gracefully
- **Execution time**: All E2E tests complete in < 5 minutes
- **Cross-platform**: Tests pass on Windows, macOS, and Linux
