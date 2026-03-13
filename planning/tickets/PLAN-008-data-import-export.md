# PLAN-008: Data Import/Export

## Ticket Number
PLAN-008

## Title
Data Import/Export

## Description
Data portability features allowing users to export their subscription data to CSV/JSON for backup or migration, and import data from supported formats.

## Acceptance Criteria
- Users can export all subscriptions to CSV format
- Users can export all data (subscriptions + cards) to JSON format
- Export includes all fields: service details, amounts, currencies, payment cards, dates
- Users can import subscriptions from CSV
- Import validates data format and handles errors gracefully
- Export/import accessible from File menu or settings

## Scope

### In Scope
- CSV export for subscriptions
- JSON export for full data (subscriptions + cards)
- CSV import with validation
- File save/open dialogs
- Error handling and user feedback for import failures
- Data mapping for import (field matching)

### Out of Scope
- Export of invoice files (only metadata exported)
- Import from proprietary formats (Excel, Google Sheets native)
- Automatic backup scheduling
- Cloud sync

## Dependencies
PLAN-002

## Priority
Low

## Estimated Complexity
Small

## Comments
Use Qt's QTextStream for CSV and QJsonDocument for JSON. For CSV, use standard format (RFC 4180). Consider UTF-8 encoding for international characters. Invoice files not included in export (only file paths/metadata).
