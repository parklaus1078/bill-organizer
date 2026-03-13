# PLAN-004: Dashboard & Analytics

## Ticket Number
PLAN-004

## Title
Dashboard & Analytics

## Description
Visual summary and analytics dashboard showing expense breakdowns, spending trends, upcoming payments, and category distributions. Includes charts, calendar view, and financial summaries.

## Acceptance Criteria
- Dashboard displays total monthly and annual expenses in display currency
- Breakdown by category shown with percentages
- Calendar or timeline view of upcoming payment dates
- Visual charts (bar/pie) for spending distribution across categories and services
- Annual cost projection graph
- All amounts displayed in user's preferred currency

## Scope

### In Scope
- Monthly and annual expense totals
- Category-based spending breakdown
- Calendar/timeline view of upcoming payments
- Bar and pie charts using Qt Charts module
- Annual projection calculations
- Filtering by date range or category

### Out of Scope
- Advanced analytics (trend prediction, anomaly detection)
- Export charts as images
- Comparison with previous periods

## Dependencies
PLAN-002, PLAN-003

## Priority
High

## Estimated Complexity
Large

## Comments
Requires Qt Charts module. Performance consideration for large datasets — use aggregation queries. Calendar view should highlight payment dates with visual indicators. Consider lazy loading for charts.
