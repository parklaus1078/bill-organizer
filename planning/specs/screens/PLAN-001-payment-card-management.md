# Payment Card Management - Screen Requirements

## UI Design Reference

**IMPORTANT**: All screens must follow the design language established in:
`projects/bill-organizer/planning/specs/screens/bill-organizer UI reference.jpg`

### Design System Guidelines from Reference:
- **Color Scheme**: Deep purple gradient background (#4A148C to #6A1B9A)
- **Sidebar**: Dark purple left sidebar with white icons and text
- **Main Content Area**: Centered content on gradient background
- **Typography**: Clean, modern sans-serif (similar to SF Pro or Roboto)
- **Icons**: Rounded, minimal style with subtle shadows
- **Buttons**: Rounded corners, gradient or solid fills matching theme
- **Cards/Panels**: Subtle shadows, rounded corners, semi-transparent backgrounds
- **Spacing**: Generous padding and margins for clean, uncluttered feel

### Sidebar Navigation (from reference):
The application uses a left sidebar navigation with items like:
- Smart Care (with icon)
- Cleanup
- Protection
- Performance
- Applications
- My Clutter
- Assistant

**For this feature**: Add "Payment Cards" (or "Cards") navigation item to the sidebar with a credit card icon.

---

## Screen List
- Card List View (default state)
- Card List View (empty state)
- Add Card Form
- Edit Card Form
- Delete Confirmation Dialog
- Card Selection Mode (invoked from subscription forms)

---

## User Flow

### Main Flow - Card List
1. User opens Card Management screen
2. System displays list of all saved cards
   - If no cards: Show empty state with "Add Card" prompt
   - If cards exist: Show card list with masked numbers
3. User can:
   - Click "Add Card" → Navigate to Add Card Form
   - Click card item → Navigate to Edit Card Form
   - Click delete icon on card → Show Delete Confirmation Dialog
   - Search/filter cards (optional enhancement)

### Add Card Flow
1. User clicks "Add Card" button
2. System displays Add Card Form with empty fields
3. User enters:
   - Card Nickname (required, e.g., "My Visa", "Work Card")
   - Issuer/Bank (required, e.g., "Chase", "Bank of America")
   - Last 4 Digits (required, numeric only, exactly 4 digits)
   - Expiry Date (required, MM/YY format, must be future date)
   - CVC (optional, 3-4 digits, encrypted if provided)
4. User clicks "Save"
   - Success: Card saved with encryption, return to Card List
   - Validation Error: Show inline error messages
5. User can click "Cancel" to return without saving

### Edit Card Flow
1. User clicks on a card from the list
2. System displays Edit Card Form pre-populated with existing data
   - CVC shown as "•••" if previously saved, or empty if not saved
3. User modifies fields
4. User clicks "Save"
   - Success: Card updated, return to Card List
   - Validation Error: Show inline error messages
5. User can click "Cancel" to return without changes

### Delete Card Flow
1. User clicks delete icon on a card
2. System shows Delete Confirmation Dialog
   - Display: "Delete [Card Nickname]?" with last 4 digits shown
   - Warning: "This action cannot be undone."
   - If card is linked to subscriptions: Show warning "This card is used by [N] subscription(s). Deleting it will not affect existing subscriptions, but you cannot reuse this card."
3. User confirms or cancels
   - Confirm: Card deleted, return to updated Card List
   - Cancel: Dialog closes, no changes

### Card Selection Flow (from Subscription Forms)
1. Subscription form invokes card selection
2. System displays Card List in selection mode
   - Simplified view: Show only nickname, issuer, last 4 digits
   - Click on card to select
3. User selects a card
4. System returns card ID to subscription form and closes selection

---

## Component Structure

### Card List View
**Components:**
- **Toolbar/Header**
  - Title: "Payment Cards"
  - "Add Card" button (primary action)
  - Search bar (optional)
- **Card List Container**
  - Individual card items (see Card Item component)
  - Empty state message (when no cards)
- **Card Item Component**
  - Card nickname (bold, primary text)
  - Issuer/Bank name (secondary text)
  - Masked card number: "•••• •••• •••• [last 4 digits]"
  - Expiry date: "Expires MM/YY"
  - Expiry warning indicator (if expired or expiring soon)
  - Edit icon/button (opens Edit Form)
  - Delete icon/button (shows Delete Confirmation)

**Empty State:**
- Icon/illustration (credit card graphic)
- Message: "No payment cards saved"
- Subtext: "Add a card to link it to your subscriptions"
- "Add Your First Card" button

### Add Card Form
**Input Fields:**
- **Card Nickname**
  - Label: "Card Nickname *"
  - Placeholder: "e.g., My Visa, Work Card"
  - Type: Text input
  - Validation: Required, max 50 characters
- **Issuer/Bank**
  - Label: "Issuer/Bank *"
  - Placeholder: "e.g., Chase, Bank of America"
  - Type: Text input or Dropdown (common banks)
  - Validation: Required, max 50 characters
- **Last 4 Digits**
  - Label: "Last 4 Digits *"
  - Placeholder: "1234"
  - Type: Numeric input
  - Validation: Required, exactly 4 digits, numeric only
- **Expiry Date**
  - Label: "Expiry Date *"
  - Placeholder: "MM/YY"
  - Type: Date picker or formatted text input
  - Validation: Required, must be future date, valid month (01-12)
- **CVC (Optional)**
  - Label: "CVC (Optional, encrypted)"
  - Placeholder: "•••"
  - Type: Password input (masked)
  - Validation: Optional, 3-4 digits if provided
  - Info icon: "CVC will be encrypted and stored securely"

**Actions:**
- "Save" button (primary, enabled only when required fields valid)
- "Cancel" button (secondary, returns to Card List)

**Validation Messages:**
- Inline error messages below each field
- Required field: "[Field name] is required"
- Invalid format: "Please enter a valid [field name]"
- Expiry validation: "Expiry date must be in the future"
- Last 4 digits: "Must be exactly 4 digits"

### Edit Card Form
**Same as Add Card Form, with differences:**
- Form title: "Edit Card"
- Fields pre-populated with existing data
- CVC field:
  - If previously saved: Show as "•••" (masked)
  - User can update by entering new value
  - User can clear by deleting masked value
- Additional action: "Delete Card" button (destructive, shows Delete Confirmation)

### Delete Confirmation Dialog
**Components:**
- Dialog title: "Delete Card?"
- Card information display:
  - Card nickname (bold)
  - Last 4 digits: "•••• [1234]"
- Warning message: "This action cannot be undone."
- Conditional warning (if linked to subscriptions):
  - "⚠️ This card is used by [N] subscription(s)."
  - "Deleting it will not affect existing subscriptions, but you cannot reuse this card."
- Actions:
  - "Cancel" button (secondary)
  - "Delete" button (destructive/red, primary action)

### Card Selection Mode (for Subscription Forms)
**Simplified Card Item:**
- Radio button or checkbox (single selection)
- Card nickname
- Issuer/Bank
- Last 4 digits: "•••• [1234]"
- Expiry date (with warning if expired)
- Disabled state if card is expired

**Actions:**
- "Select" button (enabled when card selected)
- "Cancel" button
- "Add New Card" link (opens Add Card Form in modal/overlay)

---

## Visual States

### Card Item States
- **Default**: Normal display
- **Hover**: Highlight background, show edit/delete actions
- **Expired**: Red/orange warning badge, "Expired" label
- **Expiring Soon** (within 30 days): Yellow warning badge, "Expiring soon" label
- **Selected** (in selection mode): Checked radio button, highlighted border

### Form States
- **Default**: All fields empty (Add) or populated (Edit)
- **Loading**: During save operation, show loading spinner on Save button
- **Validation Error**: Red border on invalid fields, error message below
- **Success**: Brief success message/toast, then navigate to Card List

### Button States
- **Enabled**: Full color, clickable
- **Disabled**: Grayed out (when required fields not filled)
- **Loading**: Spinner replacing button text during async operations

---

## Layout Guidelines

### Application Structure (matching UI reference)
- **Sidebar**: Fixed left sidebar (~240px width) with navigation items
  - Dark purple background matching reference design
  - Add "Payment Cards" or "Cards" navigation item with credit card icon
  - White text and icons
  - Hover state: slightly lighter background
- **Main Content Area**: Fills remaining space to the right of sidebar
  - Deep purple gradient background (matching reference)
  - Content centered horizontally with max-width constraints
  - Generous top padding (~60px from top of window)

### Desktop Layout (Card Management Screens)
- **Card List**:
  - Centered in main content area
  - Grid layout, 2-3 cards per row depending on window width
  - Max width: 1200px
  - Cards with rounded corners, subtle shadow, semi-transparent white background
- **Forms**:
  - Centered single-column layout
  - Max width 600px
  - Form container: rounded corners, semi-transparent white/light background
  - Inputs: rounded corners matching design system
- **Dialogs**:
  - Modal overlay with semi-transparent dark background (rgba(0,0,0,0.6))
  - Dialog centered, max width 400px
  - Rounded corners, white background with subtle shadow

### Color Palette (from UI reference)
- **Primary Background**: Deep purple gradient (#4A148C → #6A1B9A)
- **Sidebar**: Dark purple (#311B92 or similar)
- **Accent**: Pink/magenta for primary actions (#E91E63 or similar from reference graphic)
- **Text on Dark**: White (#FFFFFF)
- **Text on Light**: Dark gray (#212121)
- **Success**: Green (#4CAF50)
- **Warning**: Amber/Yellow (#FFC107)
- **Error**: Red (#F44336)

### Responsive Considerations (if applicable)
- Small windows: Single column card list
- Forms remain single column at all sizes
- Sidebar may collapse to icons-only on very narrow windows (optional)

---

## Accessibility Requirements
- All form fields must have associated labels
- Error messages announced to screen readers (role="alert")
- Delete confirmation critical action clearly marked
- Keyboard navigation support:
  - Tab through all interactive elements
  - Enter to submit forms
  - Escape to close dialogs
- Focus indicators visible on all interactive elements
- Color not the only indicator of validation errors

---

## Edge Cases & Error Handling

### Duplicate Detection
- When saving a card with same last 4 digits + expiry as existing card:
  - Show warning: "A card with these details already exists. Do you want to add it anyway?"
  - Allow user to proceed or cancel

### Expired Card Warning
- Cards past expiry date marked with "Expired" badge
- Warning shown in card selection: "This card is expired and may not work for new subscriptions"
- Allow selection but show confirmation: "This card is expired. Continue?"

### Database Error
- If save/update/delete fails:
  - Show error message: "Failed to save card. Please try again."
  - Log error for debugging
  - Do not navigate away from form

### Empty Last 4 Digits (historical data issue)
- If somehow card has no last 4 digits stored:
  - Display: "•••• •••• •••• ••••" (fully masked)
  - Allow editing to fix

### CVC Update Behavior
- If user enters new CVC while editing: Replace encrypted value
- If user clears CVC field while editing: Remove encrypted value from storage
- If user leaves CVC as "•••" (doesn't modify): Keep existing encrypted value unchanged

---

## Connected Data Operations
- **Card List**: Load all cards from database, decrypt for display (masked)
- **Add Card**: Validate → Encrypt sensitive data → Store in database
- **Edit Card**: Load card by ID → Decrypt for display → Validate updates → Re-encrypt → Update database
- **Delete Card**: Check for subscription links → Confirm → Delete from database
- **Card Selection**: Load cards → Filter expired (optional) → Return selected card ID

---

## Security Considerations
- **Display**: Never show full card numbers, only last 4 digits
- **CVC**: Always masked in UI (password input), never logged
- **Memory**: Clear sensitive data from memory after encryption
- **Clipboard**: Disable copy/paste on CVC field
- **Logging**: Never log card details, CVC, or encryption keys
