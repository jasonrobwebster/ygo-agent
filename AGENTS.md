# AI Agent System Instructions v1.0.0

## System Understanding Protocol
**When you first encounter this codebase**:

1. **Load System Context**:
   - Read `/agents/SCHEMA.md` - understand documentation structure
   - Read `/agents/ARCHITECTURE.md` - understand system architecture
   - Read `/agents/COGNITIVE_MAP.md` - understand navigation patterns
   - Read `/agents/PATTERNS.md` - understand coding patterns

2. **Task-Specific Context Loading**:
   - Identify relevant directories from COGNITIVE_MAP.md based on your task
   - Read relevant CONTEXT.md files for affected directories
   - Understand the data flow and integration points

3. **Validate Understanding**:
   - Can you explain the system's purpose and architecture?
   - Can you identify the directories you need to work with?
   - Do you understand the testing strategy for your changes?
   - Do you know the integration points that might be affected?

## Development Protocol
**For any code changes**:

1. **Pre-Change Analysis**:
   - What directories and abstractions are affected?
   - What technical logic or data flows change?
   - What integration points or APIs are impacted?
   - What testing strategy is needed?

2. **Implementation Strategy**:
   - Follow patterns identified in PATTERNS.md
   - Maintain consistency with existing abstractions
   - Update CONTEXT.md for any affected directories
   - Update ARCHITECTURE.md if system architecture changes
   - Update COGNITIVE_MAP.md if navigation patterns change

3. **Post-Change Documentation**:
   - Create migration file in `/agents/migrations/`
   - Update all affected documentation
   - Validate documentation consistency using VALIDATION.md
   - Test that new agents can understand the changes

## Core Abstractions Documentation Protocol
**When documenting or updating core abstractions**:

### For Functions/Methods
Always document:
- Technical purpose and problem solved
- Complete input/output schemas using language-appropriate type systems
- Side effects and error conditions
- Technical rules and validation logic
- Performance characteristics

### For Classes/Objects
Always document:
- Single responsibility and domain
- Complete state schema and lifecycle
- Public interface with purpose of each method
- Dependencies and usage patterns
- Invariants and constraints

### For Data Structures
Always document:
- Business concept represented
- Complete schema with validation rules
- Transformation and persistence patterns
- Relationships to other structures
- Evolution and migration history

### For APIs/Services
Always document:
- Business operation and users
- Complete request/response schemas
- Authentication and authorization
- Error scenarios and handling
- Performance and rate limiting

### For Integrations
Always document:
- Business capability provided
- Protocol and data mapping
- Error handling and fallbacks
- Rate limits and monitoring
- Configuration and dependencies

## Migration Protocol
**When making significant changes**:

Create `/agents/migrations/v[X.Y.Z]_[description].md`:

```yaml
# Migration v[X.Y.Z]: [Description]
date: [ISO_DATE]
type: [feature|bugfix|refactor|architecture]
scope: [file|module|system]
risk: [low|medium|high]

## Changes Made
### Code Changes
- Modified: [files and specific abstractions changed]
- Added: [new files and abstractions with purposes]
- Removed: [deleted files and abstractions with reasoning]

### Abstraction Changes
- **New abstractions**: [What new core abstractions were added]
- **Modified abstractions**: [How existing abstractions changed]
- **Deprecated abstractions**: [What abstractions are being phased out]

### Architecture Impact
- [How system architecture or patterns changed]
- [New integration points or dependencies]
- [Changes to data flow or module boundaries]

### Documentation Updates
- Updated: [which documentation files were modified]
- Added: [new documentation created]
- Reasoning: [why these documentation changes were necessary]

## Context
### Problem Solved
[What problem this addresses and why this solution was chosen - business or technical]

### Alternative Approaches
[What other solutions were considered and why this was selected]

### Future Implications
[How this change affects future development and system evolution]

## Technical Details
### Breaking Changes
[Any changes that affect existing functionality or APIs]

### Performance Impact
[Expected performance changes and measurement results]

### Security Impact
[Any security considerations or improvements]

### Testing Strategy
[How to verify this change works correctly]
- Unit tests: [What was tested]
- Integration tests: [What integration scenarios were covered]
- Manual testing: [What manual verification was performed]

## Validation Checklist
- [ ] All affected CONTEXT.md files updated with new/changed abstractions
- [ ] Parent directory CONTEXT.md files reflect changes if directory interfaces changed
- [ ] ARCHITECTURE.md updated if system boundaries changed
- [ ] COGNITIVE_MAP.md updated if navigation patterns changed
- [ ] PATTERNS.md updated if new patterns introduced
- [ ] All tests pass
- [ ] Documentation is internally consistent
- [ ] New agent can understand changes from documentation alone

## Quality Gates

**Before any changes are considered complete**:

- [ ]  All core abstractions are properly documented with full schemas
- [ ]  Documentation accurately reflects code reality
- [ ]  Future agents can understand changes from documentation alone
- [ ]  No broken references in documentation
- [ ]  Migration file explains business reasoning for changes
- [ ]  Testing validates all critical paths and edge cases 