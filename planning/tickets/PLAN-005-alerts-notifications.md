# PLAN-005: Alerts & Notifications

## Ticket Number
PLAN-005

## Title
Alerts & Notifications

## Description
Configurable reminder system that alerts users before upcoming billing dates via system tray notifications. Supports customizable advance notice periods and notification preferences.

## Acceptance Criteria
- Users can enable/disable notifications per subscription
- Configurable advance notice period (e.g., 1 day, 3 days, 1 week before billing)
- System tray notifications appear at configured times
- Notification displays service name, amount, and payment date
- Users can snooze or dismiss notifications
- Background notification scheduler runs when app is open

## Scope

### In Scope
- Notification settings per subscription
- System tray integration using Qt System Tray
- Background scheduler using QTimer
- Notification persistence (track shown/dismissed)
- Global notification preferences
- Optional: free trial expiry warnings

### Out of Scope
- Email or SMS notifications
- Notifications when app is completely closed (OS-level scheduling)
- Recurring notification reminders

## Dependencies
PLAN-002

## Priority
Medium

## Estimated Complexity
Medium

## Comments
Use QSystemTrayIcon for notifications. Scheduler should check upcoming payments daily. Consider time zone handling. For cross-platform consistency, test on macOS, Windows, and Linux.
