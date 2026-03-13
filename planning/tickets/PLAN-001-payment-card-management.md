# PLAN-001: Payment Card Management

## Ticket Number
PLAN-001

## Title
Payment Card Management

## Description
A dedicated card wallet to store and manage payment method details that can be linked to subscriptions. Supports adding, editing, deleting, and viewing payment cards with secure storage.

## Acceptance Criteria
- Users can add new payment cards with card nickname, issuer/bank, last 4 digits, expiry date
- Card details stored with encryption in SQLite database
- Users can edit and delete existing cards
- Card list view displays all saved cards with masked numbers
- Cards can be selected when creating/editing subscriptions

## Scope

### In Scope
- CRUD operations for payment cards
- Card data encryption at rest (AES)
- Card list and detail views
- Card masking for security (show only last 4 digits)
- Optional CVC storage with encryption

### Out of Scope
- Payment gateway integration
- Card validation with actual banks
- Automatic card expiry notifications (handled in PLAN-005)

## Dependencies
None

## Priority
High

## Estimated Complexity
Small

## Comments
Foundation for subscription management. Security is critical — use Qt's encryption utilities or AES-256. CVC storage is optional but should be clearly marked as such in UI.
