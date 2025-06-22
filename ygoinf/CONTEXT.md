# YGOINF Directory Context Documentation

## Purpose Statement
The ygoinf directory contains the inference system for deploying trained YGO agents as API services. It provides a FastAPI-based server that loads trained models and serves predictions for Yu-Gi-Oh! game states, enabling human-AI gameplay through YGOPro-compatible clients.

## Core Abstractions

### Functions/Methods

#### `server.py` - FastAPI Server

**Name**: `lifespan(app: FastAPI)`
**Purpose**: Manages the lifecycle of the inference server including model loading and cleanup
**Input Schema**:
- app: FastAPI - [FastAPI application instance]
- settings: Settings - [Server configuration including checkpoint path and thread count]

**Output Schema**:
- Return Value: None - [Context manager for server lifecycle]
- Side Effects: 
  - Model loading from checkpoint
  - Code list initialization
  - Test duel state creation
  - Resource cleanup on shutdown

**Side Effects**: 
- File system: Loads model checkpoint and code list
- Memory: Allocates model parameters and predictor state
- Network: Starts HTTP server on configured port
- State mutations: Initializes global model and duel state storage

**Error Conditions**: 
- Invalid checkpoint files cause loading failures
- Missing code list files cause initialization errors
- Memory issues with large models
- Port conflicts for server startup

**Business Rules**: 
- Model loaded once at startup
- Test duel state created for validation
- Resources cleaned up on shutdown
- State expiration managed by background thread

**Performance Notes**: 
- Model loading time: Seconds to minutes depending on size
- Memory usage: Scales with model parameters
- Startup time: Includes model initialization and validation

#### `predict` - Model Inference

**Name**: `predict(model_fn, input_data, prev_action_idx, duel_state)`
**Purpose**: Processes game state through trained model to generate action predictions
**Input Schema**:
- model_fn: Callable - [Trained model prediction function]
- input_data: Input - [Game state observation with cards and actions]
- prev_action_idx: int - [Index of previous action for state tracking]
- duel_state: PredictState - [Current duel state and history]

**Output Schema**:
- Return Value: MsgResponse - [Action predictions and win rate]
- Additional Info: PredictState - [Updated duel state]

**Side Effects**: 
- Duel state updates with new action history
- Model inference computation
- Memory allocation for intermediate results

**Error Conditions**: 
- Invalid game state causes KeyError
- Unsupported message types cause NotImplementedError
- Memory issues with large batch sizes
- Model prediction failures

**Business Rules**: 
- State consistency maintained across predictions
- Action indices validated against state
- Predictions filtered by game rules
- Win rate computed from model value function

**Performance Notes**: 
- Inference time: Milliseconds per prediction
- Memory usage: Scales with model size and batch size
- GPU utilization: High during model forward pass

### Classes/Objects

#### `Predictor` - Model Wrapper

**Name**: `Predictor`
**Responsibility**: Wraps trained neural network model for inference
**State Schema**:
- model: RNNAgent - [Trained neural network agent]
- params: dict - [Model parameters loaded from checkpoint]
- rstate: RNNState - [Current RNN state for temporal modeling]
- code_list: List[int] - [Supported card codes]
- num_threads: int - [Number of threads for parallel processing]

**Public Interface**:
```python
@classmethod
def load(checkpoint_path: str, num_threads: int) -> Predictor:
    # Load model from checkpoint file
    # Returns: Initialized predictor instance

def predict(self, input_data: Input, prev_action_idx: int, state: PredictState) -> MsgResponse:
    # Generate predictions for game state
    # Returns: Action predictions and win rate
```

**Lifecycle**: 
- Creation: Instantiated by loading checkpoint
- Initialization: Model parameters and state setup
- Inference: Processes game states to predictions
- Cleanup: Automatic resource management

**Dependencies**: 
- ygoai.rl.jax.agent for neural network model
- flax for parameter loading
- jax for computation

**Usage Patterns**: 
- Created once per server instance
- Shared across all prediction requests
- State maintained between predictions
- Thread-safe for concurrent access

#### `PredictState` - Duel State Management

**Name**: `PredictState`
**Responsibility**: Maintains duel state and action history for temporal consistency
**State Schema**:
- index: int - [Current action index for state tracking]
- rstate: RNNState - [RNN state for temporal modeling]
- history: List[dict] - [Action history for state reconstruction]
- _timestamp: float - [Last access timestamp for expiration]

**Public Interface**:
```python
def __init__(self):
    # Initialize new duel state
    # Sets index=0 and empty history

def update(self, action_idx: int, action_data: dict):
    # Update state with new action
    # Increments index and stores action
```

**Lifecycle**: 
- Creation: Instantiated for new duels
- Updates: Modified with each action
- Expiration: Cleaned up after timeout
- Cleanup: Automatic garbage collection

**Dependencies**: 
- time for timestamp tracking
- RNN state management

**Usage Patterns**: 
- Created per duel session
- Updated with each prediction
- Expired after inactivity
- Thread-safe for concurrent access

#### `Settings` - Server Configuration

**Name**: `Settings`
**Responsibility**: Manages server configuration and environment variables
**State Schema**:
- code_list: str - [Path to card code list file, default "code_list.txt"]
- checkpoint: str - [Path to model checkpoint, default "latest.flax_model"]
- enable_cors: bool - [Enable CORS for web clients, default True]
- state_expire: int - [Duel state expiration time in seconds, default 3600]
- test_duel_id: str - [Test duel ID for validation, default UUID]
- ygo_num_threads: int - [Number of threads for YGO prediction, default 1]

**Public Interface**:
- All fields accessible as attributes
- Pydantic validation and type checking
- Environment variable override support

**Lifecycle**: 
- Creation: Instantiated at server startup
- Configuration: Loaded from environment variables
- Usage: Accessed throughout server lifecycle
- Cleanup: Automatic garbage collection

**Dependencies**: 
- pydantic_settings for configuration management
- pydantic for validation

**Usage Patterns**: 
- Created once at server startup
- Used for all configuration access
- Environment variable overrides
- Validation on startup

### Data Structures/Types

#### Game State Input Schema

**Name**: `Input`
**Purpose**: Complete game state representation for model inference
**Schema Definition**:

Required Fields:
- global_: Global - [Global game state including LP, turn, phase]
- cards: List[Card] - [List of all cards in game, max 160, min 2]
- action_msg: ActionMsg - [Current action message requiring response]

**Validation Rules**: 
- Cards list must have 2-160 cards
- Global state must be valid
- Action message must be supported type

**Transformation Patterns**: 
- YGOPro message → Input schema
- Input schema → Model observation
- Model output → Action response

**Persistence**: 
- Stored in duel state history
- Used for state reconstruction
- Logged for debugging

#### Card Information Schema

**Name**: `Card`
**Purpose**: Complete card representation with all game-relevant attributes
**Schema Definition**:

Required Fields:
- code: int - [Card code from cards.cdb database]
- location: Location - [Card location: deck, hand, mzone, szone, grave, removed, extra]
- sequence: int - [Sequence in ocgcore, 0 for N/A, starts from 1 for specific locations]
- controller: Controller - [Card controller: me or opponent]
- position: Position - [Card position: faceup_attack, facedown_defense, etc.]
- overlay_sequence: int - [Overlay index for XYZ materials, -1 if not overlay]
- attribute: Attribute - [Card attribute: earth, water, fire, wind, light, dark, divine]
- race: Race - [Card race: warrior, spellcaster, dragon, etc.]
- level: int - [Card level/rank/link, 0 for N/A]
- counter: int - [Number of counters, first type only]
- negated: bool - [Whether card effect is disabled]
- attack: int - [Attack points]
- defense: int - [Defense points]
- types: List[Type] - [Card types: monster, spell, trap, effect, fusion, etc.]

**Validation Rules**: 
- Code must be valid card ID
- Location and controller must be consistent
- Position valid for card type
- Level/rank/link must be non-negative

**Transformation Patterns**: 
- YGOPro card data → Card schema
- Card schema → Model features
- Model features → Card representation

**Persistence**: 
- Stored in game state
- Used for model inference
- Logged for debugging

#### Action Message Schema

**Name**: `ActionMsg`
**Purpose**: Represents different types of action messages requiring player response
**Schema Definition**:

Union Types:
- MsgSelectIdleCmd - [Idle command selection: summon, activate, etc.]
- MsgSelectChain - [Chain selection for spell/trap activation]
- MsgSelectCard - [Card selection with min/max constraints]
- MsgSelectTribute - [Tribute selection for summoning]
- MsgSelectPosition - [Position selection for summoning]
- MsgSelectEffectYn - [Effect activation yes/no]
- MsgSelectYesNo - [General yes/no selection]
- MsgSelectBattleCmd - [Battle command selection]
- MsgSelectUnselectCard - [Card unselection]
- MsgSelectOption - [Option selection]
- MsgSelectPlace - [Place selection for cards]
- MsgSelectSum - [Summon material selection]
- MsgSelectDisfield - [Discrete field selection]
- MsgAnnounceAttrib - [Attribute announcement]
- MsgAnnounceNumber - [Number announcement]

**Validation Rules**: 
- Message type must be supported
- Required fields must be present
- Constraints must be valid (min ≤ max, etc.)

**Transformation Patterns**: 
- YGOPro message → ActionMsg schema
- ActionMsg schema → Model input
- Model output → Action response

**Persistence**: 
- Stored in game state
- Used for model inference
- Logged for debugging

#### Prediction Response Schema

**Name**: `MsgResponse`
**Purpose**: Model prediction output with action probabilities and win rate
**Schema Definition**:

Required Fields:
- action_preds: List[ActionPredict] - [List of action predictions with probabilities]
- win_rate: float - [Estimated win probability for current player]

**Validation Rules**: 
- Probabilities must sum to 1.0
- Win rate must be between 0.0 and 1.0
- Action predictions must be valid

**Transformation Patterns**: 
- Model logits → Action probabilities
- Model value → Win rate
- Probabilities → Action predictions

**Persistence**: 
- Returned to client
- Logged for analysis
- Used for debugging

### API Endpoints/Routes

#### Health Check API

**Endpoint**: `GET /`
**Purpose**: Verify server is running and responsive
**Authentication**: None required
**Request Schema**: No parameters required

**Response Schema**:

Success Response (200):
- Content-Type: text/plain
- Body: "OK" - [Simple health check response]

Error Responses:
- 500: Server error - [Internal server error]

**Business Logic**: 
- Simple health check endpoint
- No model inference required
- Fast response for load balancers

**Side Effects**: 
- None - read-only endpoint

**Performance**: 
- Response time: Microseconds
- No model computation required

#### Duel Creation API

**Endpoint**: `POST /v0/duels`
**Purpose**: Create new duel session for AI gameplay
**Authentication**: None required
**Request Schema**: No request body required

**Response Schema**:

Success Response (200):
```json
{
  "duelId": "007f8d84-7944-4851-921c-d61d4884a841",
  "index": 0
}
```

Error Responses:
- 500: Server error - [Internal server error]

**Business Logic**: 
- Generates unique duel ID
- Creates new PredictState
- Initializes action index to 0

**Side Effects**: 
- Creates new duel state in memory
- Allocates RNN state for temporal modeling

**Performance**: 
- Response time: Milliseconds
- Memory allocation for new state

#### Duel Prediction API

**Endpoint**: `POST /v0/duels/{duelId}/predict`
**Purpose**: Get AI predictions for current game state
**Authentication**: None required
**Request Schema**:

Path Parameters:
- duelId: str - [Unique duel identifier]

Request Body:
```json
{
  "input": {
    "global": {
      "my_lp": 8000,
      "op_lp": 8000,
      "turn": 1,
      "phase": "main1",
      "is_first": true,
      "is_my_turn": true
    },
    "cards": [...],
    "action_msg": {...}
  },
  "prev_action_idx": 0,
  "index": 1
}
```

**Response Schema**:

Success Response (200):
```json
{
  "index": 2,
  "predict_results": {
    "action_preds": [
      {
        "prob": 0.85,
        "response": 0,
        "can_finish": false
      }
    ],
    "win_rate": 0.65
  }
}
```

Error Responses:
- 400: Bad request - [Invalid input data]
- 404: Duel not found - [Invalid duel ID]
- 409: Index mismatch - [State inconsistency]
- 500: Server error - [Model inference error]

**Business Logic**: 
- Validates duel state and index
- Processes game state through model
- Returns action predictions and win rate
- Updates duel state with new action

**Side Effects**: 
- Model inference computation
- Duel state updates
- Memory allocation for results

**Performance**: 
- Response time: 10-100ms depending on model size
- GPU utilization during inference
- Memory usage scales with batch size

#### Duel Cleanup API

**Endpoint**: `DELETE /v0/duels/{duelId}`
**Purpose**: Clean up duel session and free resources
**Authentication**: None required
**Request Schema**:

Path Parameters:
- duelId: str - [Unique duel identifier]

**Response Schema**:

Success Response (204):
- No content - [Successful deletion]

Error Responses:
- 404: Duel not found - [Invalid duel ID]
- 500: Server error - [Internal server error]

**Business Logic**: 
- Removes duel state from memory
- Frees RNN state resources
- Cleans up action history

**Side Effects**: 
- Memory deallocation
- State cleanup

**Performance**: 
- Response time: Microseconds
- Immediate resource cleanup

### Service/Module Interfaces

#### Inference Service Interface

**Service Name**: `InferenceService`
**Domain Responsibility**: Provides AI prediction capabilities for Yu-Gi-Oh! gameplay
**Public Interface**:

```python
class InferenceService:
    # Core operations
    def create_duel() -> DuelCreateResponse:  # Create new duel session
    def predict_action(duel_id: str, input_data: Input) -> MsgResponse:  # Get AI predictions
    def delete_duel(duel_id: str) -> None:  # Clean up duel session
    
    # Model management
    def load_model(checkpoint_path: str) -> Predictor:  # Load trained model
    def unload_model() -> None:  # Unload model and free resources
    
    # State management
    def get_duel_state(duel_id: str) -> PredictState:  # Get current duel state
    def update_duel_state(duel_id: str, action: dict) -> None:  # Update with new action
    def cleanup_expired_states() -> None:  # Remove expired duel states
    
    # Configuration
    def update_settings(settings: Settings) -> None:  # Update server configuration
    def get_server_status() -> dict:  # Get server health and statistics
```

**Dependencies**: 
- ygoai.rl.jax.agent for neural network models
- fastapi for HTTP server
- pydantic for data validation

**Configuration**:
```python
{
    "code_list": "code_list.txt",
    "checkpoint": "latest.flax_model",
    "enable_cors": True,
    "state_expire": 3600,
    "ygo_num_threads": 1,
    "host": "127.0.0.1",
    "port": 3000
}
```

**Error Handling**: 
- Invalid requests return 400 errors
- Missing duels return 404 errors
- Model errors return 500 errors
- State inconsistencies return 409 errors

**Monitoring**: 
- Request/response timing
- Model inference performance
- Memory usage tracking
- Error rate monitoring

### Integration/External System Interfaces

#### YGOPro Client Integration

**System Name**: `YGOPro Clients`
**Purpose**: Yu-Gi-Oh! game clients that use AI predictions for gameplay
**Protocol**: HTTP REST API
**Authentication**: None - local network access
**Request/Response Examples**:

Prediction Request:
```http
POST /v0/duels/007f8d84-7944-4851-921c-d61d4884a841/predict
Content-Type: application/json

{
  "input": {
    "global": {"my_lp": 8000, "op_lp": 8000, "turn": 1, "phase": "main1"},
    "cards": [...],
    "action_msg": {"msg_type": "select_idlecmd", "idle_cmds": [...]}
  },
  "prev_action_idx": 0,
  "index": 1
}
```

Prediction Response:
```http
HTTP/1.1 200 OK
Content-Type: application/json

{
  "index": 2,
  "predict_results": {
    "action_preds": [{"prob": 0.85, "response": 0}],
    "win_rate": 0.65
  }
}
```

**Error Handling**: 
- Network errors trigger retry logic
- Invalid responses cause fallback behavior
- Timeout errors use default actions

**Rate Limits**: 
- Request frequency limits
- Concurrent duel limits
- Memory usage constraints

**Data Mapping**: 
- YGOPro messages → API requests
- API responses → YGOPro actions
- Game state → Model observations

#### Neos Web Client Integration

**System Name**: `Neos`
**Purpose**: Web-based YGOPro client with AI integration
**Protocol**: HTTP REST API with CORS
**Authentication**: None - browser-based access
**Request/Response Examples**:

Same as YGOPro client integration but with CORS headers:
```http
Access-Control-Allow-Origin: *
Access-Control-Allow-Methods: GET, POST, DELETE
Access-Control-Allow-Headers: Content-Type
```

**Error Handling**: 
- Browser network errors
- CORS policy violations
- JavaScript error handling

**Rate Limits**: 
- Browser request limits
- WebSocket connection limits
- Memory constraints

**Data Mapping**: 
- Web client state → API requests
- API responses → Web client actions
- Real-time game updates

### Business Logic/Workflow Abstractions

#### AI Gameplay Workflow

**Workflow Name**: `AIGameplaySession`
**Business Purpose**: Enable human-AI gameplay through YGOPro-compatible clients
**Process Steps**:

```
1. Session Initialization
   ├── Client creates duel session
   ├── Server generates unique duel ID
   ├── Initialize AI state and RNN memory
   └── Return session credentials

2. Game State Processing
   ├── Client sends current game state
   ├── Server validates state consistency
   ├── Process through trained neural network
   └── Generate action predictions

3. Action Selection
   ├── Convert model logits to probabilities
   ├── Apply game rule constraints
   ├── Select optimal action
   └── Return action to client

4. State Updates
   ├── Client executes selected action
   ├── Update game state
   ├── Increment action index
   └── Prepare for next prediction

5. Session Cleanup
   ├── Game completion or timeout
   ├── Clean up server resources
   ├── Free memory and state
   └── Log session statistics
```

**Decision Points**: 
- Action selection based on model probabilities
- State validation and consistency checks
- Resource management and cleanup
- Error handling and fallback strategies

**Rollback/Compensation**: 
- State reset on inconsistencies
- Fallback to default actions on errors
- Resource cleanup on failures
- Session recovery mechanisms

**Business Rules**: 
- State consistency maintained across predictions
- Action indices must be sequential
- Resource limits enforced per session
- Error handling with graceful degradation

**State Transitions**: 
- Initialization → Active → Prediction → Update → Active (repeat)
- Active → Completion → Cleanup
- Active → Error → Recovery → Active
- Active → Timeout → Cleanup

## Dependencies Map

### Imports Analysis

Internal Dependencies:
- ygoai.rl.jax.agent from ygoai.rl.jax - [Purpose: Neural network agent for predictions]
- ygoai.utils from ygoai - [Purpose: Utility functions for YGOPro integration]

External Dependencies:
- fastapi - [Purpose: HTTP server framework]
- pydantic - [Purpose: Data validation and serialization]
- uvicorn - [Purpose: ASGI server for FastAPI]
- jax - [Purpose: Neural network computation]
- flax - [Purpose: Model parameter management]

### Exports Analysis

What this directory exposes and why:
- server.py - [FastAPI server for AI inference]
- models.py - [Data models for API requests/responses]
- features.py - [Feature processing and model inference]
- jax_inf.py - [JAX-based inference utilities]
- tflite_inf.py - [TensorFlow Lite inference support]

## Integration Points

- **Called by**: YGOPro clients, Neos web client, other game clients
- **Calls**: ygoai.rl.jax.agent for model inference, ygoai.utils for utilities
- **Data dependencies**: Model checkpoints, card code lists, game state data
- **Event producers**: Action predictions, win rate estimates, server statistics
- **Event consumers**: Game clients for AI gameplay, monitoring systems

## Complexity Indicators

- **Cognitive load**: Medium - [Reasoning: API server with model inference complexity]
- **Change frequency**: Low - [How often inference system is modified]
- **Test coverage**: Medium - [Estimated coverage of API testing]
- **Performance sensitivity**: High - [Whether performance is critical for real-time gameplay]
- **Error impact**: Medium - [Blast radius if inference fails - gameplay interruption] 