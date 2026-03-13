# PLAN-002: Subscription Entry & Management

## Ticket Number
PLAN-002

## Title
Subscription Entry & Management

## Description
Core subscription tracking system allowing users to record and manage all recurring expenses. Includes service details, billing cycles, payment amounts, and linked payment cards.

## Acceptance Criteria
- Users can create subscriptions with service name, category, billing cycle, amount, currency
- Support for multiple billing cycles: monthly, annual, weekly, custom
- Users can link a payment card from the card wallet
- Users can edit and delete subscriptions
- Subscription list displays all entries with key information
- Calculate and store next/last payment dates based on billing cycle

## Scope

### In Scope
- CRUD operations for subscriptions
- Multiple billing cycle types (monthly, annual, weekly, custom interval)
- Category management (utilities, streaming, cloud/SaaS, telecom, etc.)
- Payment card linking
- Notes and tags support
- Payment history tracking (past payments)

### Out of Scope
- Currency conversion (handled in PLAN-003)
- Dashboard visualizations (handled in PLAN-004)
- Notifications (handled in PLAN-005)
- Invoice attachments (handled in PLAN-006)

## Dependencies
PLAN-001

## Priority
High

## Estimated Complexity
Medium

## Comments
This is the core domain model. Billing cycle calculation logic should be robust and handle edge cases (e.g., monthly on 31st when month has 30 days). Consider using Qt's QDate for date calculations.
