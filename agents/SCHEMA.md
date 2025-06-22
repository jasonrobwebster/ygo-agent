# Agent Documentation System Schema v1.0.0

## Documentation Architecture

```
/agents/
├── SCHEMA.md              # This file - system definition and evolution rules
├── AGENTS.md             # Agent instructions and development protocols

├── ARCHITECTURE.md       # System-level architecture synthesis
├── COGNITIVE_MAP.md      # Task-based navigation guide
├── PATTERNS.md           # Cross-module design patterns
├── VALIDATION.md         # System consistency checks and quality gates
├── CHANGELOG.md          # Version history and evolution timeline
└── migrations/           # Change history with business reasoning
├── v1.0.0_initial_system_bootstrap.md
└── v1.1.0_[future_changes].md

/[source_directory]/
├── CONTEXT.md            # Directory-level documentation
├── [subdirectory]/
│   ├── CONTEXT.md        # Subdirectory-level documentation with detailed abstractions
│   └── [source_files]
```

## Core Abstraction Documentation Standards

### Abstraction Classification
Every significant code component must be classified as one of:
- **Function/Method**: Procedural logic with inputs, outputs, side effects
- **Class/Object**: Stateful entities with behavior and lifecycle
- **Data Structure/Type**: Information schemas with validation rules
- **API/Service Interface**: External boundaries with request/response contracts
- **Integration Point**: External system connections with protocols
- **Business Workflow**: Multi-step processes with decision points

### Required Documentation Elements
For each abstraction type, documentation must include:

#### Universal Elements (all abstractions)
- **Purpose**: Business problem solved or capability provided
- **Dependencies**: What this requires to function
- **Error conditions**: What can go wrong and how it's handled
- **Testing approach**: How to verify correct behavior

#### Type-Specific Elements
**Functions**: Input/output schemas, side effects, business rules, performance
**Classes**: State schema, lifecycle, public interface, usage patterns
**Data**: Schema definition, validation rules, transformations, persistence
**APIs**: Request/response schemas, authentication, error responses, rate limits
**Integrations**: Protocol, data mapping, fallback behavior, monitoring
**Workflows**: Process steps, decision points, state transitions, rollback

## Documentation Hierarchy Rules
1. **Granularity correlation**: Detail increases with directory depth
2. **Bottom-up synthesis**: Higher-level docs synthesize lower-level insights
3. **Context preservation**: Each level provides context for deeper exploration
4. **Agent navigation**: Clear entry points for different development tasks
5. **Business traceability**: Business value is traceable from system to implementation

## Version Management
### Semantic Versioning
- **Major version** (X.0.0): System architecture changes, breaking API changes
- **Minor version** (X.Y.0): New modules, features, or significant functionality
- **Patch version** (X.Y.Z): Bug fixes, documentation improvements, refactoring

### Change Triggers
Documentation version increments required for:
- **Major**: New modules, removed modules, architecture pattern changes
- **Minor**: New public APIs, significant business logic additions
- **Patch**: Bug fixes, internal refactoring, documentation improvements

## Consistency Requirements
### Coverage Requirements
- Every directory containing source files must have CONTEXT.md with detailed abstractions
- Every parent directory must have CONTEXT.md synthesizing child directories
- All processes must have entry points in COGNITIVE_MAP.md
- All architectural patterns must be documented in PATTERNS.md

### Quality Requirements
- All abstractions must include complete schemas and technical purpose
- All integrations must document protocols and error handling
- All workflows must document decision points and state transitions
- All changes must include reasoning in migration files

### Validation Requirements
- No broken internal references between documentation files
- Version numbers consistent across all documentation
- Migration history complete and chronologically ordered
- New agents can understand system from documentation alone

## Evolution Protocol
### When Documentation Must Be Updated
1. **Code changes**: Any modification to core abstractions requires documentation update
2. **Architecture changes**: Module boundaries or patterns require system-level updates
3. **Integration changes**: External system modifications require interface documentation
4. **Business logic changes**: Workflow or rule modifications require business context updates

### Documentation Drift Prevention
1. **Automated checks**: Validate documentation consistency on code changes
2. **Review requirements**: All code changes must include documentation updates
3. **Migration discipline**: All significant changes require migration files with reasoning
4. **Agent testing**: New agents must successfully understand system from docs alone 