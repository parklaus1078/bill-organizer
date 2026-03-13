# bill-organizer

a desktop application that I can organize, schedule billings like electricity bills, gas bills, telephone bills, OTT subscription bills, and etc., includng the card number, cvc number, bcard compnan, billing date, and etc. and can get monthly/annual sum of the bills. I would like it to include the images or emails of the receipts as they are delivered as well.

---

## Project Information

- **Type**: desktop-app
- **Language**: C++
- **Framework**: Qt
- **Created**: 2026-03-12T15:15:35Z

---

## Directory Structure

```
bill-organizer/
├── .project-meta.json          # Project metadata
├── planning/                   # Planning documents
│   ├── tickets/                # Ticket files
│   ├── specs/                  # Specifications
│   └── test-cases/             # Test cases
├── src/                        # Actual code
├── logs/                       # Agent logs
└── README.md                   # This file
```

---

## Workflow

### 1. Create Tickets

```bash
cd team
bash scripts/run-agent.sh project-planner --project "Project description"
```

### 2. Generate Specifications

```bash
bash scripts/run-agent.sh pm --ticket-file projects/bill-organizer/planning/tickets/PLAN-001-*.md
```

### 3. Coding

```bash
bash scripts/run-agent.sh coding --ticket PLAN-001
```

### 4. Testing

```bash
bash scripts/run-agent.sh qa --ticket PLAN-001
```

---

## View Logs

```bash
bash scripts/show-logs.sh
```

---

**Created**: 2026-03-12T15:15:35Z
