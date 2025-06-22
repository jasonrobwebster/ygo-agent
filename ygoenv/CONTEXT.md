# YGOENV Directory Context Documentation

## Purpose Statement
The ygoenv directory contains the Yu-Gi-Oh! game environment implementation for the YGO Agent system. It provides a gymnasium-compatible environment interface that wraps YGOPro game engines, enabling reinforcement learning training and evaluation through standardized environment APIs.

## Core Abstractions

### Functions/Methods

#### `register` - Environment Registration

**Name**: `register(task_id, import_path, spec_cls, dm_cls, gym_cls, gymnasium_cls, **kwargs)`
**Purpose**: Registers a new environment specification and environment pool classes in the global registry
**Input Schema**:
- task_id: str - [Unique environment identifier]
- import_path: str - [Python module path for environment implementation]
- spec_cls: str - [Environment specification class name]
- dm_cls: str - [DeepMind environment pool class name]
- gym_cls: str - [Gym environment pool class name]
- gymnasium_cls: str - [Gymnasium environment pool class name]
- **kwargs: Any - [Additional configuration parameters]

**Output Schema**:
- Return Value: None - [Registration side effect only]

**Side Effects**: 
- Registry: Updates global environment registry
- Memory: Allocates registry entries for environment specifications

**Error Conditions**: 
- AssertionError: task_id already exists in registry
- ImportError: Environment module import failures
- ValueError: Invalid configuration parameters

**Business Rules**: 
- Each task_id must be unique
- All class names must reference valid classes
- Base path automatically added if not provided

**Performance Notes**: 
- Constant time complexity O(1)
- Minimal memory overhead
- Registration happens at import time

#### `make` - Environment Creation

**Name**: `make(task_id, env_type, **kwargs)`
**Purpose**: Creates an environment pool instance for the specified task and type
**Input Schema**:
- task_id: str - [Environment identifier to create]
- env_type: str - [Environment type: 'dm', 'gym', or 'gymnasium']
- **kwargs: Any - [Environment configuration parameters]

**Output Schema**:
- Return Value: Any - [Environment pool instance]

**Side Effects**: 
- Module: Imports environment implementation
- Memory: Allocates environment pool and specifications
- System: Initializes game engine if required

**Error Conditions**: 
- AssertionError: task_id not found in registry
- ValueError: Invalid env_type or configuration
- ImportError: Environment module import failures

**Business Rules**: 
- task_id must be registered before creation
- env_type must be one of supported types
- Configuration parameters validated before creation
- Gym version compatibility automatically handled

**Performance Notes**: 
- Module import overhead on first creation
- Environment pool initialization time
- Memory usage scales with num_envs parameter

#### `make_gymnasium` - Gymnasium Environment Creation

**Name**: `make_gymnasium(task_id, **kwargs)`
**Purpose**: Creates a gymnasium-compatible environment pool
**Input Schema**:
- task_id: str - [Environment identifier]
- **kwargs: Any - [Environment configuration parameters]

**Output Schema**:
- Return Value: Any - [Gymnasium environment pool instance]

**Side Effects**: 
- Same as make() with env_type='gymnasium'

**Error Conditions**: 
- Same as make() function

**Business Rules**: 
- Uses gymnasium.Env interface
- Automatic gym version compatibility handling
- Reset returns (observation, info) tuple

**Performance Notes**: 
- Same as make() function

#### `make_spec` - Environment Specification Creation

**Name**: `make_spec(task_id, **make_kwargs)`
**Purpose**: Creates an environment specification instance
**Input Schema**:
- task_id: str - [Environment identifier]
- **make_kwargs: Any - [Specification configuration parameters]

**Output Schema**:
- Return Value: Any - [Environment specification instance]

**Side Effects**: 
- Module: Imports specification class
- Memory: Allocates specification instance

**Error Conditions**: 
- AssertionError: task_id not found or invalid parameters
- ImportError: Specification module import failures
- ValueError: Invalid configuration parameters

**Business Rules**: 
- Validates seed range (-2^31 to 2^31-1)
- Validates num_envs >= 1
- Validates batch_size <= num_envs
- Validates max_num_players >= 1

**Performance Notes**: 
- Module import overhead
- Configuration validation time
- Minimal memory usage

### Classes/Objects

#### `EnvRegistry` - Environment Registry

**Name**: `EnvRegistry`
**Responsibility**: Global registry for environment specifications and pools
**State Schema**:
- specs: Dict[str, Tuple[str, str, Dict[str, Any]]] - [task_id → (import_path, spec_cls, kwargs)]
- envpools: Dict[str, Dict[str, Tuple[str, str]]] - [task_id → {env_type → (import_path, cls)}]

**Public Interface**:
```python
def register(self, task_id, import_path, spec_cls, dm_cls, gym_cls, gymnasium_cls, **kwargs):
    # Register environment specification and pool classes

def make(self, task_id, env_type, **kwargs):
    # Create environment pool instance

def make_spec(self, task_id, **make_kwargs):
    # Create environment specification instance

def list_all_envs(self):
    # Return list of all available task_ids
```

**Lifecycle**: 
- Creation: Instantiated as global registry
- Registration: Environments registered at import time
- Usage: Environment creation and specification access
- Cleanup: Automatic garbage collection

**Dependencies**: 
- importlib for dynamic module loading
- gym for version compatibility
- packaging for version parsing

**Usage Patterns**: 
- Global singleton instance
- Registration during module imports
- Environment creation on demand
- Specification access for configuration

#### `YGOProEnvSpec` - YGOPro Environment Specification

**Name**: `YGOProEnvSpec`
**Responsibility**: Configuration specification for YGOPro environments
**State Schema**:
- config: Dict[str, Any] - [Environment configuration parameters]
- task_id: str - [Environment identifier]
- base_path: str - [Base path for environment resources]

**Public Interface**:
```python
@classmethod
def gen_config(cls, **kwargs):
    # Generate configuration from parameters

def __init__(self, config):
    # Initialize specification with configuration
```

**Lifecycle**: 
- Creation: Instantiated with configuration
- Usage: Environment pool creation
- Cleanup: Automatic garbage collection

**Dependencies**: 
- ygoenv.core for base specification
- YGOPro game engine for implementation

**Usage Patterns**: 
- Created by make_spec()
- Used by environment pools
- Configuration validation and storage

### Data Structures/Types

#### Environment Configuration Schema

**Name**: `EnvConfig`
**Purpose**: Complete configuration for environment creation
**Schema Definition**:

Required Fields:
- task_id: str - [Environment identifier]
- num_envs: int - [Number of parallel environments, must be >= 1]
- batch_size: int - [Batch size for environment steps, must be <= num_envs]

Optional Fields:
- seed: int - [Random seed, must be in int32 range]
- max_num_players: int - [Maximum number of players, must be >= 1]
- base_path: str - [Base path for environment resources]
- gym_reset_return_info: bool - [Whether reset returns info dict]

Computed/Derived Fields:
- env_type: str - [Environment type: 'dm', 'gym', 'gymnasium']
- spec_cls: str - [Environment specification class]
- pool_cls: str - [Environment pool class]

**Validation Rules**: 
- num_envs must be positive integer
- batch_size must be <= num_envs
- seed must be in int32 range
- max_num_players must be >= 1

**Transformation Patterns**: 
- Parameters → Configuration dict
- Configuration → Environment specification
- Specification → Environment pool

**Persistence**: 
- Stored in environment registry
- Used for environment creation
- Cached for performance

#### Environment Pool Schema

**Name**: `EnvPool`
**Purpose**: Parallel environment execution pool
**Schema Definition**:

Required Fields:
- spec: EnvSpec - [Environment specification]
- num_envs: int - [Number of parallel environments]
- batch_size: int - [Batch size for steps]

Optional Fields:
- seed: int - [Random seed for reproducibility]
- max_num_players: int - [Maximum players per environment]

**Validation Rules**: 
- All environments use same specification
- Batch size <= number of environments
- Seed provides reproducible behavior

**Transformation Patterns**: 
- Specification → Pool initialization
- Pool → Parallel environment execution
- Pool → Step/reset operations

**Persistence**: 
- Lives for environment lifetime
- Manages parallel environment state
- Handles environment cleanup

### API Endpoints/Routes

#### Environment Creation API

**Endpoint**: `make(task_id: str, env_type: str, **kwargs) -> Any`
**Purpose**: Create environment pool for specified task and type
**Authentication**: None - local environment creation
**Request Schema**:

Input Parameters:
- task_id: str - [Environment identifier]
- env_type: str - [Environment type: 'dm', 'gym', 'gymnasium']
- **kwargs: Any - [Environment configuration]

**Response Schema**:

Success Response:
- Return Value: Any - [Environment pool instance]

Error Responses:
- AssertionError: task_id not found or invalid parameters
- ValueError: Invalid env_type or configuration
- ImportError: Environment module import failures

**Business Logic**: 
- Validates task_id exists in registry
- Validates env_type is supported
- Creates environment specification
- Initializes environment pool

**Side Effects**: 
- Module imports
- Memory allocation
- Game engine initialization

**Performance**: 
- Module import overhead
- Pool initialization time
- Memory scales with num_envs

#### Environment Registration API

**Endpoint**: `register(task_id, import_path, spec_cls, dm_cls, gym_cls, gymnasium_cls, **kwargs)`
**Purpose**: Register new environment in global registry
**Authentication**: None - local registration
**Request Schema**:

Input Parameters:
- task_id: str - [Unique environment identifier]
- import_path: str - [Module path for implementation]
- spec_cls: str - [Specification class name]
- dm_cls: str - [DeepMind pool class name]
- gym_cls: str - [Gym pool class name]
- gymnasium_cls: str - [Gymnasium pool class name]
- **kwargs: Any - [Additional configuration]

**Response Schema**:

Success Response:
- Return Value: None - [Registration side effect]

Error Responses:
- AssertionError: task_id already exists
- ImportError: Module import failures

**Business Logic**: 
- Validates task_id uniqueness
- Stores environment specifications
- Registers pool classes

**Side Effects**: 
- Registry updates
- Memory allocation for specifications

**Performance**: 
- Constant time registration
- Minimal memory overhead

### Service/Module Interfaces

#### Environment Management Service Interface

**Service Name**: `EnvironmentManagementService`
**Domain Responsibility**: Provides environment creation and management capabilities
**Public Interface**:

```python
class EnvironmentManagementService:
    # Core operations
    def register(task_id: str, import_path: str, spec_cls: str, **kwargs):  # Register environment
    def make(task_id: str, env_type: str, **kwargs) -> Any:  # Create environment pool
    def make_spec(task_id: str, **kwargs) -> Any:  # Create environment specification
    
    # Environment type shortcuts
    def make_dm(task_id: str, **kwargs) -> Any:  # Create DeepMind environment
    def make_gym(task_id: str, **kwargs) -> Any:  # Create Gym environment
    def make_gymnasium(task_id: str, **kwargs) -> Any:  # Create Gymnasium environment
    
    # Information
    def list_all_envs() -> List[str]:  # List available environments
    def get_env_info(task_id: str) -> dict:  # Get environment information
```

**Dependencies**: 
- importlib for dynamic module loading
- gym for version compatibility
- packaging for version parsing
- ygoenv.core for base functionality

**Configuration**:
```python
{
    "supported_env_types": ["dm", "gym", "gymnasium"],
    "default_env_type": "gymnasium",
    "max_num_envs": 10000,
    "default_batch_size": 64,
    "seed_range": (-2**31, 2**31-1)
}
```

**Error Handling**: 
- Invalid task_id returns AssertionError
- Invalid env_type returns ValueError
- Module import failures return ImportError
- Configuration validation errors

**Monitoring**: 
- Environment creation performance
- Memory usage for pools
- Module import times

### Integration/External System Interfaces

#### YGOPro Game Engine Integration

**System Name**: `YGOPro Game Engine`
**Purpose**: Yu-Gi-Oh! game rule enforcement and state management
**Protocol**: C++ extension module interface
**Authentication**: None - local module loading
**Request/Response Examples**:

Environment Creation:
```python
from ygoenv.ygopro import YGOProGymnasiumEnvPool
envs = YGOProGymnasiumEnvPool(spec)
```

Environment Step:
```python
obs, reward, done, info = envs.step(actions)
# obs: Game state observations
# reward: Reward signal
# done: Episode termination
# info: Additional information
```

**Error Handling**: 
- Game engine initialization failures
- Invalid game state errors
- Rule violation handling
- Memory allocation errors

**Rate Limits**: 
- Parallel environment limits
- Memory constraints per environment
- Game engine performance limits

**Data Mapping**: 
- Python actions → YGOPro actions
- YGOPro state → Python observations
- Game events → Reward signals

#### Gymnasium Interface Integration

**System Name**: `Gymnasium`
**Purpose**: Standardized reinforcement learning environment interface
**Protocol**: Gymnasium.Env interface
**Authentication**: None - interface compliance
**Request/Response Examples**:

Environment Creation:
```python
import gymnasium as gym
envs = ygoenv.make("YGOPro-v1", "gymnasium", num_envs=128)
```

Environment Usage:
```python
obs, info = envs.reset()
obs, reward, terminated, truncated, info = envs.step(actions)
```

**Error Handling**: 
- Interface compliance validation
- Observation space validation
- Action space validation
- Environment state errors

**Rate Limits**: 
- Gymnasium interface constraints
- Observation/action space limits
- Environment step time limits

**Data Mapping**: 
- Environment state → Gymnasium observations
- Gymnasium actions → Environment actions
- Environment events → Gymnasium info

### Business Logic/Workflow Abstractions

#### Environment Lifecycle Management Workflow

**Workflow Name**: `EnvironmentLifecycleManagement`
**Business Purpose**: Manage complete lifecycle of Yu-Gi-Oh! environments for reinforcement learning
**Process Steps**:

```
1. Environment Registration
   ├── Import environment modules
   ├── Register environment specifications
   ├── Register environment pool classes
   └── Validate registration completeness

2. Environment Creation
   ├── Validate task_id exists in registry
   ├── Create environment specification
   ├── Initialize environment pool
   └── Configure parallel environments

3. Environment Execution
   ├── Reset environments to initial state
   ├── Execute parallel environment steps
   ├── Collect observations and rewards
   └── Handle episode termination

4. Environment Cleanup
   ├── Close environment pools
   ├── Release game engine resources
   ├── Clean up memory allocations
   └── Reset registry state
```

**Decision Points**: 
- Environment type selection (dm/gym/gymnasium)
- Parallel environment count configuration
- Batch size optimization
- Error handling and recovery

**Rollback/Compensation**: 
- Environment creation failure recovery
- Resource cleanup on errors
- Registry state restoration
- Memory leak prevention

**Business Rules**: 
- Each task_id must be unique
- Environment types must be supported
- Configuration parameters must be valid
- Resources must be properly cleaned up

**State Transitions**: 
- Registration → Creation → Execution → Cleanup
- Error → Recovery → Retry
- Invalid State → Validation → Error Handling

## Dependencies Map

### Imports Analysis

Internal Dependencies:
- ygoenv.core from ygoenv - [Purpose: Core environment functionality]
- ygoenv.ygopro from ygoenv - [Purpose: YGOPro environment implementation]
- ygoenv.ygopro0 from ygoenv - [Purpose: YGOPro v0 environment implementation]
- ygoenv.edopro from ygoenv - [Purpose: EDOPro environment implementation]
- ygoenv.dummy from ygoenv - [Purpose: Dummy environment for testing]

External Dependencies:
- gym - [Purpose: Gym environment interface]
- gymnasium - [Purpose: Gymnasium environment interface]
- importlib - [Purpose: Dynamic module loading]
- packaging - [Purpose: Version parsing and compatibility]

### Exports Analysis

What this directory exposes and why:
- registration.py - [Global environment registry and registration functions]
- entry.py - [Environment module import entry point]
- ygopro/ - [YGOPro v1 environment implementation]
- ygopro0/ - [YGOPro v0 environment implementation]
- edopro/ - [EDOPro environment implementation]
- dummy/ - [Dummy environment for testing]
- core/ - [Core environment functionality]

## Integration Points

- **Called by**: scripts/ for training environments, ygoai/ for environment initialization
- **Calls**: YGOPro game engines for rule enforcement, gymnasium for interface compliance
- **Data dependencies**: Game engine modules, environment specifications, configuration files
- **Event producers**: Environment creation events, step execution events, episode termination events
- **Event consumers**: Training scripts for environment interaction, evaluation for performance measurement

## Complexity Indicators

- **Cognitive load**: High - [Reasoning: Complex environment management with multiple game engines]
- **Change frequency**: Low - [How often environment system is modified]
- **Test coverage**: Medium - [Estimated coverage of environment testing]
- **Performance sensitivity**: High - [Whether performance is critical for parallel training]
- **Error impact**: High - [Blast radius if environment fails - training stops] 