# bill-organizer

A desktop application to organize the scheduled recurring bills (electricity, gas, phone, OTT subscriptions, etc.), securely manage payment cards linked to those subscriptions (including optional encrypted CVC storage), track monthly and annual spending totals, and store receipt images or email copies as they are delivered.

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
