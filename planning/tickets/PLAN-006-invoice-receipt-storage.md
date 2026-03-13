# PLAN-006: Invoice & Receipt Storage

## Ticket Number
PLAN-006

## Title
Invoice & Receipt Storage

## Description
Document management system for attaching and viewing invoices/receipts. Supports multiple input formats (images, PDFs, email HTML, screenshots) with in-app viewer and organized storage structure.

## Acceptance Criteria
- Users can attach one or more invoice files to each payment record
- Support for image formats (JPG, PNG, HEIC), PDF, and HTML/text
- Drag-and-drop file upload and file picker support
- In-app viewer for images and PDFs with zoom capability
- Thumbnail previews in payment history list
- Invoice files stored in organized folder structure (/invoices/{year}/{month}/{service_name}/)
- File metadata tracked in database (file path, type, upload date, original filename)

## Scope

### In Scope
- Multi-format file attachment (image, PDF, HTML/text)
- In-app invoice viewer using QPdfDocument and QImageReader
- Thumbnail generation and display
- Organized local file storage structure
- File metadata storage in SQLite
- Drag-and-drop and file picker UI
- Full-screen viewer with zoom/pan for images

### Out of Scope
- Cloud storage integration
- Invoice compression/archiving
- Bulk invoice upload
- OCR parsing (handled in PLAN-007)

## Dependencies
PLAN-002

## Priority
Medium

## Estimated Complexity
Large

## Comments
Use Qt's QPdfDocument for PDF rendering and QImageReader for images. For HTML content, consider QTextBrowser or QWebEngineView. Security: validate file types and scan for malicious content. Consider max file size limits.
