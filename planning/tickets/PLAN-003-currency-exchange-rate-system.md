# PLAN-003: Currency & Exchange Rate System

## Ticket Number
PLAN-003

## Title
Currency & Exchange Rate System

## Description
Multi-currency support with automatic exchange rate fetching. Converts foreign currency subscriptions to user's display currency, using historical rates for past payments and current rates for future estimates.

## Acceptance Criteria
- Users can set a preferred display currency in settings
- App fetches current exchange rates from public API (e.g., ExchangeRate-API)
- Past payments store converted amount using historical exchange rate from payment date
- Future payments display estimated amount using today's exchange rate
- Exchange rates cached locally to minimize API calls
- Users can manually refresh exchange rates

## Scope

### In Scope
- Integration with exchange rate API (ExchangeRate-API, Open Exchange Rates, or ECB)
- Display currency preference setting
- Historical exchange rate storage
- Automatic currency conversion for display
- Rate cache with timestamp
- Manual and automatic rate refresh

### Out of Scope
- Offline exchange rate calculation without API
- Support for cryptocurrency
- Custom exchange rate manual entry

## Dependencies
PLAN-002

## Priority
High

## Estimated Complexity
Medium

## Comments
API key management required for most exchange rate services. Consider fallback behavior when API is unavailable. Use Qt Network module for HTTP requests. Store rates with timestamp to determine freshness.
