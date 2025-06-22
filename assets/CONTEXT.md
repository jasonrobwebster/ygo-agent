# ASSETS Directory Context Documentation

## Purpose Statement
The assets directory contains static assets and configuration files for the YGO Agent system, including Yu-Gi-Oh! deck files, logging configurations, and other resources that support training, inference, and system operation. This directory provides the data and configuration foundation for the entire YGO Agent system.

## Core Abstractions

### `deck/` - Deck Files Directory

#### Directory Purpose
Contains Yu-Gi-Oh! deck files in various formats (.ydk, .txt, .json) used for training and testing agents.

#### File Types
- .ydk files: YGOPro deck format files
- .txt files: Text-based deck lists
- .json files: JSON-formatted deck data

#### Usage Patterns
- Training: Decks used for agent training scenarios
- Testing: Decks used for evaluation and benchmarking
- Validation: Decks used for system validation

### `log_conf.yaml` - Logging Configuration

#### File Purpose
Contains logging configuration for the YGO Agent system, defining log levels, formats, and output destinations.

#### Configuration Sections
- Loggers: Different logger configurations for various components
- Handlers: Log output handlers (file, console, etc.)
- Formatters: Log message formatting specifications
- Root Logger: Default logging configuration

### `asset_manager.py` - Asset Management Utilities

#### File Purpose
Contains utilities for loading and managing static assets including deck files and configurations.

#### Functions

**Name**: `load_deck_file`
**Purpose**: Loads a Yu-Gi-Oh! deck file from the assets directory
**Input Schema**:
- deck_name: str - [Name of deck file to load]
- deck_format: str - [Deck file format: 'ydk', 'txt', 'json']

**Output Schema**:
- Return Value: List[int] - [List of card codes from deck file]

**Side Effects**: 
- File system: Reads deck file from disk
- Memory: Allocates list for card codes

**Error Conditions**: 
- FileNotFoundError: Deck file doesn't exist
- ValueError: Invalid deck file format
- IOError: File reading errors

**Business Rules**: 
- Supports multiple deck file formats
- Validates card codes against database
- Handles missing or corrupted files
- Returns empty list for invalid files

**Performance Notes**: 
- Linear time complexity O(n) where n is number of cards
- Memory usage scales with deck size
- File I/O is primary bottleneck

**Name**: `load_config`
**Purpose**: Loads configuration files from assets directory
**Input Schema**:
- config_name: str - [Name of configuration file]
- config_type: str - [Configuration type: 'yaml', 'json', 'ini']

**Output Schema**:
- Return Value: Dict[str, Any] - [Configuration dictionary]

**Side Effects**: 
- File system: Reads configuration file
- Memory: Allocates configuration dictionary

**Error Conditions**: 
- FileNotFoundError: Configuration file not found
- ValueError: Invalid configuration format
- YAMLError: YAML parsing errors
- JSONDecodeError: JSON parsing errors

**Business Rules**: 
- Supports multiple configuration formats
- Validates configuration structure
- Provides default values for missing keys
- Handles environment variable substitution

**Performance Notes**: 
- Constant time complexity O(1) for small configs
- Memory usage scales with configuration size
- File I/O overhead

#### Classes

**Name**: `AssetManager`
**Responsibility**: Manages loading and caching of static assets
**State Schema**:
- assets_path: str - [Path to assets directory]
- cache: Dict[str, Any] - [Asset cache for performance]
- config: Dict[str, Any] - [Asset manager configuration]

**Public Interface**:
```python
def __init__(self, assets_path: str, config: Dict[str, Any]):
    # Initialize asset manager

def load_deck(self, deck_name: str) -> List[int]:
    # Load deck file

def load_config(self, config_name: str) -> Dict[str, Any]:
    # Load configuration file

def get_asset_path(self, asset_name: str) -> str:
    # Get full path to asset

def clear_cache(self):
    # Clear asset cache
```

**Lifecycle**: 
- Creation: Instantiated with assets directory path
- Usage: Manages asset loading and caching
- Cleanup: Automatic garbage collection

**Dependencies**: 
- pathlib for path operations
- yaml for YAML parsing
- json for JSON parsing

**Usage Patterns**: 
- Created at system startup
- Used throughout system for asset access
- Caches frequently used assets

**Name**: `DeckManager`
**Responsibility**: Manages Yu-Gi-Oh! deck files and operations
**State Schema**:
- deck_path: str - [Path to deck directory]
- deck_cache: Dict[str, List[int]] - [Deck cache]
- card_database: Dict[int, Dict] - [Card database reference]

**Public Interface**:
```python
def __init__(self, deck_path: str, card_database: Dict[int, Dict]):
    # Initialize deck manager

def load_deck(self, deck_name: str) -> List[int]:
    # Load deck by name

def save_deck(self, deck_name: str, card_codes: List[int]):
    # Save deck to file

def validate_deck(self, card_codes: List[int]) -> bool:
    # Validate deck legality

def get_deck_info(self, deck_name: str) -> Dict[str, Any]:
    # Get deck information
```

**Lifecycle**: 
- Creation: Instantiated with deck directory
- Usage: Manages deck loading and validation
- Cleanup: Automatic garbage collection

**Dependencies**: 
- Card database for validation
- File system for deck storage
- Deck format parsers

**Usage Patterns**: 
- Created with card database
- Used for training deck loading
- Validates deck legality

## Dependencies Map

### Imports Analysis

Internal Dependencies:
- ygoai for card database access
- ygoenv for environment configuration
- scripts for training configuration

External Dependencies:
- pathlib - [Purpose: Path operations]
- yaml - [Purpose: YAML configuration parsing]
- json - [Purpose: JSON file parsing]
- os - [Purpose: File system operations]

### Exports Analysis

What this directory exposes and why:
- deck/ - [Yu-Gi-Oh! deck files for training and testing]
- log_conf.yaml - [Logging configuration for system operation]
- Configuration files - [System configuration and settings]

## Integration Points

- **Called by**: scripts/ for training deck loading, ygoai/ for card data, ygoenv/ for configuration
- **Calls**: File system for asset storage, YAML/JSON parsers for configuration
- **Data dependencies**: Deck files, configuration files, logging settings
- **Event producers**: Asset loading events, configuration change events, cache update events
- **Event consumers**: Training scripts for deck access, system components for configuration

## Complexity Indicators

- **Cognitive load**: Low - [Reasoning: Simple asset management with file operations]
- **Change frequency**: Low - [How often assets are modified]
- **Test coverage**: Medium - [Estimated coverage of asset testing]
- **Performance sensitivity**: Medium - [Whether performance is critical for asset loading]
- **Error impact**: Low - [Blast radius if assets fail - system configuration impact] 