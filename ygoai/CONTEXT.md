# YGOAI Directory Context Documentation

## Purpose Statement
The ygoai directory contains the core AI utilities and constants for the YGO Agent system. It provides essential functions for YGOPro initialization, game constants and enums, card embedding utilities, and version management that are used throughout the training and inference systems.

## Core Abstractions

### `utils.py` - Core Utility Functions

#### File Purpose
Contains core utility functions for deck loading, path management, YGOPro initialization, and embedding loading.

#### Functions

**Name**: `load_deck(fn)`
**Purpose**: Loads a Yu-Gi-Oh! deck file (.ydk format) and extracts card codes
**Input Schema**: fn: str - [Path to .ydk deck file]
**Output Schema**: List[int] - [List of card codes from the deck file]
**Side Effects**: File system read, memory allocation
**Error Conditions**: FileNotFoundError, ValueError, IOError
**Business Rules**: Only numeric lines processed, empty lines ignored
**Performance Notes**: Linear time complexity, scales with deck size

**Name**: `get_root_directory()`
**Purpose**: Determines the root directory of the YGO Agent project
**Input Schema**: No parameters
**Output Schema**: str - [Absolute path to project root directory]
**Side Effects**: File system path resolution
**Error Conditions**: RuntimeError, PermissionError
**Business Rules**: Returns parent of parent directory, always absolute path
**Performance Notes**: Constant time complexity, minimal memory usage

**Name**: `extract_deck_name(path)`
**Purpose**: Extracts deck name from file path by removing extension
**Input Schema**: path: str - [File path to deck file]
**Output Schema**: str - [Deck name without file extension]
**Side Effects**: None - pure function
**Error Conditions**: ValueError for invalid path format
**Business Rules**: Removes file extension, handles relative/absolute paths
**Performance Notes**: Constant time complexity, no memory allocation

**Name**: `init_ygopro(env_id, lang, deck, code_list_file, preload_tokens=False, return_deck_names=False)`
**Purpose**: Initializes YGOPro environment with specified configuration
**Input Schema**: env_id: str, lang: str, deck: str, code_list_file: str, preload_tokens: bool, return_deck_names: bool
**Output Schema**: Union[str, Tuple[str, List[str]]] - [Deck name or (deck name, list of deck names)]
**Side Effects**: File system reads, memory allocation, module initialization
**Error Conditions**: FileNotFoundError, ValueError, ImportError
**Business Rules**: Supports single files/directories, auto-loads tokens, language-specific database
**Performance Notes**: Database loading time, scales with number of decks

**Name**: `load_embeddings(embedding_file, code_list_file, pad_to=999)`
**Purpose**: Loads card embeddings from file and aligns with code list
**Input Schema**: embedding_file: str, code_list_file: str, pad_to: Optional[int]
**Output Schema**: np.ndarray - [Embedding matrix with shape (pad_to, embedding_dim)]
**Side Effects**: File system reads, memory allocation
**Error Conditions**: FileNotFoundError, AssertionError, ValueError
**Business Rules**: Embeddings must match code list length, zero padding, float32 dtype
**Performance Notes**: File I/O time, memory scales with embedding size

### `constants.py` - Game Constants and Enums

#### File Purpose
Contains all game constants, enums, and mappings used throughout the YGO Agent system for Yu-Gi-Oh! game mechanics.

#### Key Constants
- `AMOUNT_ATTRIBUTES`: Number of card attributes (7)
- `AMOUNT_RACES`: Number of monster races (25)
- `ATTRIBUTES_OFFSET`: Offset for attribute values (1010)
- `RACES_OFFSET`: Offset for race values (1020)

#### Key Enums
- `LOCATION`: Card location enums (DECK, HAND, MZONE, SZONE, GRAVE, etc.)
- `POSITION`: Card position enums (FACEUP_ATTACK, FACEDOWN_DEFENSE, etc.)
- `TYPE`: Card type enums (MONSTER, SPELL, TRAP, NORMAL, EFFECT, etc.)
- `ATTRIBUTE`: Monster attribute enums (EARTH, WATER, FIRE, WIND, LIGHT, DARK, DEVINE)
- `RACE`: Monster race enums (WARRIOR, SPELLCASTER, FAIRY, FIEND, etc.)
- `REASON`: Card movement reason enums (DESTROY, RELEASE, SUMMON, etc.)
- `OPCODE`: Operation code enums for game logic
- `INFORM`: Information scope enums (PLAYER, OPPONENT, ALL)
- `DECK`: Deck visibility enums (OWNED, OTHER, PUBLIC, ALL, VISIBLE)

#### Mappings
- `location2str`, `position2str`, `type2str`, `attribute2str`, `race2str`: Enum to string mappings
- `PHASES`: Phase value to string mappings
- `LINK_MARKERS`: Link marker value to string mappings

*Note: Detailed enum values and mappings are defined in the constants.py file for reference.*

### `embed.py` - Card Embedding and Database Utilities

#### File Purpose
Contains utilities for parsing card data from Yu-Gi-Oh! database, creating card objects, and formatting card information.

#### Functions

**Name**: `parse_types(value)`
**Purpose**: Parses card type flags into list of type strings
**Input Schema**: value: int - [Card type flags]
**Output Schema**: List[str] - [List of type strings]
**Side Effects**: None - pure function
**Business Rules**: Uses type2str mapping, handles multiple type combinations
**Performance Notes**: Linear time complexity, efficient bitwise operations

**Name**: `parse_attribute(value)`
**Purpose**: Parses attribute value into attribute string
**Input Schema**: value: int - [Attribute value]
**Output Schema**: str - [Attribute string]
**Side Effects**: None - pure function
**Error Conditions**: AssertionError for invalid attribute value
**Business Rules**: Uses attribute2str mapping, validates value exists
**Performance Notes**: Constant time complexity, dictionary lookup

**Name**: `parse_race(value)`
**Purpose**: Parses race value into race string
**Input Schema**: value: int - [Race value]
**Output Schema**: str - [Race string]
**Side Effects**: None - pure function
**Error Conditions**: AssertionError for invalid race value
**Business Rules**: Uses race2str mapping, validates value exists
**Performance Notes**: Constant time complexity, dictionary lookup

**Name**: `format_monster_card(card: MonsterCard)`
**Purpose**: Formats monster card information into display string
**Input Schema**: card: MonsterCard - [Monster card object]
**Output Schema**: str - [Formatted monster card string]
**Side Effects**: None - pure function
**Business Rules**: Formats name, type, attribute, race, level, ATK, DEF, description
**Performance Notes**: Constant time complexity, string concatenation

**Name**: `format_spell_trap_card(card: Union[SpellCard, TrapCard])`
**Purpose**: Formats spell/trap card information into display string
**Input Schema**: card: Union[SpellCard, TrapCard] - [Spell or trap card object]
**Output Schema**: str - [Formatted spell/trap card string]
**Side Effects**: None - pure function
**Business Rules**: Formats name, type, and description
**Performance Notes**: Constant time complexity, string concatenation

**Name**: `format_card(card: Card)`
**Purpose**: Formats any card object into display string
**Input Schema**: card: Card - [Card object (monster, spell, or trap)]
**Output Schema**: str - [Formatted card string]
**Side Effects**: None - pure function
**Error Conditions**: ValueError for invalid card type
**Business Rules**: Delegates to appropriate formatter based on card type
**Performance Notes**: Constant time complexity, type checking overhead

**Name**: `parse_monster_card(data) -> MonsterCard`
**Purpose**: Parses monster card data from database into MonsterCard object
**Input Schema**: data: Dict - [Card data from database]
**Output Schema**: MonsterCard - [Parsed monster card object]
**Side Effects**: None - pure function
**Error Conditions**: ValueError, AssertionError for invalid data
**Business Rules**: Parses all monster fields, handles pendulum level adjustment
**Performance Notes**: Constant time complexity, object creation overhead

**Name**: `parse_spell_card(data) -> SpellCard`
**Purpose**: Parses spell card data from database into SpellCard object
**Input Schema**: data: Dict - [Card data from database]
**Output Schema**: SpellCard - [Parsed spell card object]
**Side Effects**: None - pure function
**Error Conditions**: ValueError for invalid card data
**Business Rules**: Parses spell card fields (code, name, desc, types)
**Performance Notes**: Constant time complexity, object creation overhead

**Name**: `parse_trap_card(data) -> TrapCard`
**Purpose**: Parses trap card data from database into TrapCard object
**Input Schema**: data: Dict - [Card data from database]
**Output Schema**: TrapCard - [Parsed trap card object]
**Side Effects**: None - pure function
**Error Conditions**: ValueError for invalid card data
**Business Rules**: Parses trap card fields (code, name, desc, types)
**Performance Notes**: Constant time complexity, object creation overhead

**Name**: `parse_card(data) -> Card`
**Purpose**: Parses card data from database into appropriate card object
**Input Schema**: data: Dict - [Card data from database]
**Output Schema**: Card - [Parsed card object (MonsterCard, SpellCard, or TrapCard)]
**Side Effects**: None - pure function
**Error Conditions**: ValueError for invalid card type
**Business Rules**: Determines card type, delegates to appropriate parser
**Performance Notes**: Constant time complexity, type checking overhead

**Name**: `read_cards(cards_path)`
**Purpose**: Reads and parses all cards from Yu-Gi-Oh! database
**Input Schema**: cards_path: str - [Path to cards.cdb database file]
**Output Schema**: Tuple[pd.DataFrame, List[Card]] - [Merged dataframe and list of card objects]
**Side Effects**: File system reads, memory allocation
**Error Conditions**: FileNotFoundError, sqlite3.Error, ValueError
**Business Rules**: Reads datas and texts tables, merges on card ID, parses all cards
**Performance Notes**: Database I/O time, scales with number of cards

#### Classes

**Name**: `Card`
**Responsibility**: Base class for all Yu-Gi-Oh! cards
**State Schema**: code: int, name: str, desc: str, types: List[str]
**Public Interface**: format() -> str
**Dependencies**: ygoai.constants, format_card function
**Usage Patterns**: Created from database data, used for card information display

**Name**: `MonsterCard`
**Responsibility**: Represents monster cards with combat statistics
**State Schema**: code: int, name: str, desc: str, types: List[str], atk: int, def_: int, level: int, race: str, attribute: str
**Public Interface**: format() -> str
**Dependencies**: Card base class, format_monster_card function
**Usage Patterns**: Created from database monster data, contains combat statistics

**Name**: `SpellCard`
**Responsibility**: Represents spell cards
**State Schema**: code: int, name: str, desc: str, types: List[str]
**Public Interface**: format() -> str
**Dependencies**: Card base class, format_spell_trap_card function
**Usage Patterns**: Created from database spell data, no combat statistics

**Name**: `TrapCard`
**Responsibility**: Represents trap cards
**State Schema**: code: int, name: str, desc: str, types: List[str]
**Public Interface**: format() -> str
**Dependencies**: Card base class, format_spell_trap_card function
**Usage Patterns**: Created from database trap data, no combat statistics

### `_version.py` - Version Information

#### File Purpose
Contains version information for the ygoai package.

#### Constants
- `__version__`: Current version of the ygoai package ("0.1.0")

### `__init__.py` - Package Initialization

#### File Purpose
Contains package initialization code and exports for the ygoai package.

#### Content
- Empty file for package initialization
- No specific functionality

## Dependencies Map

### Imports Analysis

Internal Dependencies:
- ygoenv for YGOPro environment integration

External Dependencies:
- numpy - [Purpose: Numerical operations and arrays]
- pandas - [Purpose: Data manipulation and analysis]
- sqlite3 - [Purpose: Database access]
- pathlib - [Purpose: Path operations]
- pickle - [Purpose: Object serialization]
- typing - [Purpose: Type hints]
- dataclasses - [Purpose: Data class definitions]

### Exports Analysis

What this directory exposes and why:
- utils.py - [Core utility functions for deck loading and YGOPro initialization]
- constants.py - [Game constants and enums for Yu-Gi-Oh! mechanics]
- embed.py - [Card embedding and database utilities]
- _version.py - [Package version information]
- __init__.py - [Package initialization]

## Integration Points

- **Called by**: scripts/ for training setup, ygoenv/ for environment initialization, ygoinf/ for inference
- **Calls**: ygoenv/ for YGOPro module initialization
- **Data dependencies**: Deck files, card databases, embedding files, code lists
- **Event producers**: YGOPro initialization events, deck loading events, embedding loading events
- **Event consumers**: Training scripts for environment setup, inference for card data access

## Complexity Indicators

- **Cognitive load**: Medium - [Reasoning: Core utilities with moderate complexity]
- **Change frequency**: Low - [How often core utilities are modified]
- **Test coverage**: Medium - [Estimated coverage of utility testing]
- **Performance sensitivity**: Medium - [Whether performance is critical for utility functions]
- **Error impact**: High - [Blast radius if utilities fail - system initialization stops] 