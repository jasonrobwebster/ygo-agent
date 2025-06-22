# YGOAI RL JAX Directory Context Documentation

## Purpose Statement
The ygoai/rl/jax directory contains the core neural network agent implementation for the YGO Agent system. It provides the RNNAgent architecture that processes Yu-Gi-Oh! game states and outputs action probabilities and value estimates for reinforcement learning training and inference.

## Core Abstractions

### Functions/Methods

#### `rnn_step_by_main` - RNN State Management

**Name**: `rnn_step_by_main(rnn_layer, rstate, f_state, done, main, return_state=False)`
**Purpose**: Manages RNN state transitions based on game phase and episode termination
**Input Schema**:
- rnn_layer: nn.Module - [RNN cell (LSTM, GRU, or RWKV)]
- rstate: Union[tuple, jnp.ndarray] - [Current RNN state]
- f_state: jnp.ndarray - [Feature representation of game state]
- done: jnp.ndarray - [Episode termination flags]
- main: jnp.ndarray - [Main phase indicators]
- return_state: bool - [Whether to return intermediate states]

**Output Schema**:
- Return Value: tuple - [Updated RNN state and processed features]
- Additional Info: tuple - [Intermediate states if return_state=True]

**Side Effects**: 
- RNN state updates based on game phase
- State reset on episode termination
- Feature processing through RNN layers

**Error Conditions**: 
- Invalid RNN layer type causes TypeError
- Shape mismatches cause ValueError
- Memory issues with large batch sizes

**Business Rules**: 
- RNN state resets when done=True
- State updates only during main phases
- Feature processing follows RNN architecture

**Performance Notes**: 
- JAX JIT compilation for efficiency
- Vectorized operations for batch processing
- Memory-efficient state management

#### `rnn_forward_2p` - Multi-Step RNN Processing

**Name**: `rnn_forward_2p(rnn_layer, rstate, f_state, done, switch_or_main, switch=False, return_state=False)`
**Purpose**: Processes multi-step sequences through RNN for two-player game dynamics
**Input Schema**:
- rnn_layer: nn.Module - [RNN cell for sequence processing]
- rstate: Union[tuple, jnp.ndarray] - [Initial RNN state]
- f_state: jnp.ndarray - [Sequence of feature representations]
- done: jnp.ndarray - [Episode termination flags for each step]
- switch_or_main: jnp.ndarray - [Player switching or main phase indicators]
- switch: bool - [Whether to use player switching logic]
- return_state: bool - [Whether to return intermediate states]

**Output Schema**:
- Return Value: tuple - [Final RNN state and processed features]
- Additional Info: tuple - [Intermediate states if return_state=True]

**Side Effects**: 
- Sequential RNN state updates
- Player switching state management
- Episode boundary handling

**Error Conditions**: 
- Invalid sequence lengths cause ValueError
- Memory issues with long sequences
- RNN cell compatibility issues

**Business Rules**: 
- Handles player switching in two-player games
- Maintains state continuity across turns
- Resets state on episode boundaries

**Performance Notes**: 
- JAX scan for efficient sequence processing
- Optimized for parallel execution
- Memory-efficient for long sequences

### Classes/Objects

#### `RNNAgent` - Main Neural Network Agent

**Name**: `RNNAgent`
**Responsibility**: Complete neural network agent for Yu-Gi-Oh! reinforcement learning
**State Schema**:
- num_layers: int - [Number of transformer layers, default 2]
- num_channels: int - [Feature dimension, default 128]
- rnn_channels: int - [RNN hidden dimension, default 512]
- rnn_type: str - [RNN type: 'lstm', 'gru', 'rwkv', 'none', default 'lstm']
- use_history: bool - [Whether to use action history, default True]
- card_mask: bool - [Whether to mask padding cards, default False]
- film: bool - [Whether to use FiLM conditioning, default True]
- noam: bool - [Whether to use Noam architecture, default True]
- oppo_info: bool - [Whether to use opponent information, default False]
- embedding_shape: Optional[Union[int, Tuple[int, int]]] - [Card embedding dimensions]
- dtype: jnp.dtype - [Computation dtype, default float32]
- param_dtype: jnp.dtype - [Parameter dtype, default float32]

**Public Interface**:
```python
def __call__(self, x, rstate, done=None, switch_or_main=None, train=False):
    # Forward pass through the agent
    # Returns: (rstate, logits, value, valid) or (rstate, logits, valid) for Q-heads

def init_rnn_state(self, batch_size):
    # Initialize RNN state for given batch size
    # Returns: RNN state appropriate for rnn_type
```

**Lifecycle**: 
- Creation: Instantiated with configuration parameters
- Initialization: RNN state created for batch processing
- Forward Pass: Processes game states to action probabilities
- Training: Parameters updated via backpropagation
- Inference: Used for action selection and value estimation

**Dependencies**: 
- flax.linen for neural network layers
- jax for computation
- ygoai.rl.jax.transformer for transformer components
- ygoai.rl.jax.modules for utility modules

**Usage Patterns**: 
- Created once per training run
- RNN state initialized for each batch
- Forward pass called for each game step
- Parameters shared across all environments

#### `Encoder` - Game State Encoder

**Name**: `Encoder`
**Responsibility**: Encodes Yu-Gi-Oh! game state into neural network features
**State Schema**:
- channels: int - [Feature dimension, default 128]
- num_layers: int - [Number of transformer layers, default 2]
- embedding_shape: Optional[Union[int, Tuple[int, int]]] - [Card embedding dimensions]
- use_history: bool - [Whether to use action history, default True]
- card_mask: bool - [Whether to mask padding cards, default False]
- noam: bool - [Whether to use Noam architecture, default False]
- action_feats: bool - [Whether to include action features, default True]
- oppo_info: bool - [Whether to include opponent information, default False]
- version: int - [Environment version, default 2]

**Public Interface**:
```python
def __call__(self, x):
    # Encode game state into features
    # Returns: (f_actions, f_state, f_g, mask, valid)
    # f_actions: Action features
    # f_state: State features
    # f_g: Global features
    # mask: Attention mask
    # valid: Validity indicators
```

**Lifecycle**: 
- Creation: Instantiated with encoder configuration
- Encoding: Processes raw game state into features
- Integration: Used by RNNAgent for state representation

**Dependencies**: 
- CardEncoder for card feature extraction
- ActionEncoder for action feature extraction
- GlobalEncoder for global state features
- Transformer layers for feature processing

**Usage Patterns**: 
- Called by RNNAgent during forward pass
- Processes observation dictionary
- Outputs features for actor and critic networks

#### `CardEncoder` - Card Feature Encoder

**Name**: `CardEncoder`
**Responsibility**: Encodes individual card information into neural features
**State Schema**:
- channels: int - [Feature dimension, default 128]
- oppo_info: bool - [Whether to include opponent card info, default False]
- version: int - [Encoder version, default 2]
- dtype: Optional[jnp.dtype] - [Computation dtype]
- param_dtype: jnp.dtype - [Parameter dtype, default float32]

**Public Interface**:
```python
def __call__(self, x_id, x, mask):
    # Encode card information
    # Returns: (f_cards_g, f_cards, c_mask)
    # f_cards_g: Global card features (if oppo_info=True)
    # f_cards: Local card features
    # c_mask: Card validity mask
```

**Lifecycle**: 
- Creation: Instantiated with card encoder configuration
- Encoding: Processes card data into features
- Integration: Used by Encoder for card representation

**Dependencies**: 
- MLP for feature transformation
- nn.Embed for categorical features
- nn.Dense for numerical features
- Layer normalization for feature stabilization

**Usage Patterns**: 
- Called by Encoder for each card in game state
- Processes card ID, attributes, and position
- Outputs features for transformer processing

#### `ActionEncoder` - Action Feature Encoder

**Name**: `ActionEncoder`
**Responsibility**: Encodes action information into neural features
**State Schema**:
- channels: int - [Feature dimension, default 128]
- dtype: Optional[jnp.dtype] - [Computation dtype]
- param_dtype: jnp.dtype - [Parameter dtype, default float32]

**Public Interface**:
```python
def __call__(self, x):
    # Encode action information
    # Returns: List of action feature tensors
    # Features include: message, action type, yes/no, phase, etc.
```

**Lifecycle**: 
- Creation: Instantiated with action encoder configuration
- Encoding: Processes action data into features
- Integration: Used by Encoder for action representation

**Dependencies**: 
- nn.Embed for categorical action features
- Partial function for embedding configuration

**Usage Patterns**: 
- Called by Encoder for action history
- Processes different action components
- Outputs features for transformer processing

#### `Actor` - Policy Network

**Name**: `Actor`
**Responsibility**: Outputs action probabilities for policy-based reinforcement learning
**State Schema**:
- channels: int - [Feature dimension, default 128]
- dtype: Optional[jnp.dtype] - [Computation dtype]
- param_dtype: jnp.dtype - [Parameter dtype, default float32]
- final_init: nn.initializers.Initializer - [Final layer initialization, default orthogonal(0.01)]

**Public Interface**:
```python
def __call__(self, f_state, f_actions, mask):
    # Compute action logits
    # Returns: Action logits for policy
```

**Lifecycle**: 
- Creation: Instantiated with actor configuration
- Forward Pass: Computes action probabilities
- Training: Updated via policy gradient methods

**Dependencies**: 
- MLP for feature processing
- Attention mechanism for action selection
- Softmax for probability computation

**Usage Patterns**: 
- Called by RNNAgent for action selection
- Processes state and action features
- Outputs logits for action sampling

#### `FiLMActor` - FiLM-Conditioned Actor

**Name**: `FiLMActor`
**Responsibility**: Actor network with FiLM (Feature-wise Linear Modulation) conditioning
**State Schema**:
- channels: int - [Feature dimension, default 128]
- dtype: Optional[jnp.dtype] - [Computation dtype]
- param_dtype: jnp.dtype - [Parameter dtype, default float32]
- noam: bool - [Whether to use Noam architecture, default False]
- final_init: nn.initializers.Initializer - [Final layer initialization, default orthogonal(0.01)]

**Public Interface**:
```python
def __call__(self, f_state, f_actions, mask):
    # Compute action logits with FiLM conditioning
    # Returns: Action logits for policy
```

**Lifecycle**: 
- Creation: Instantiated with FiLM actor configuration
- Forward Pass: Computes conditioned action probabilities
- Training: Updated via policy gradient methods

**Dependencies**: 
- FiLM conditioning layers
- MLP for feature processing
- Attention mechanism for action selection

**Usage Patterns**: 
- Called by RNNAgent when film=True
- Processes state with FiLM conditioning
- Outputs logits for action sampling

#### `Critic` - Value Network

**Name**: `Critic`
**Responsibility**: Estimates state value for value-based reinforcement learning
**State Schema**:
- channels: Sequence[int] - [Layer dimensions, default (128, 128, 128)]
- dtype: Optional[jnp.dtype] - [Computation dtype]
- param_dtype: jnp.dtype - [Parameter dtype, default float32]

**Public Interface**:
```python
def __call__(self, f_state, train):
    # Compute state value
    # Returns: Value estimate for current state
```

**Lifecycle**: 
- Creation: Instantiated with critic configuration
- Forward Pass: Computes state values
- Training: Updated via value function methods

**Dependencies**: 
- MLP for value computation
- Activation functions for non-linearity

**Usage Patterns**: 
- Called by RNNAgent for value estimation
- Processes state features
- Outputs value for advantage computation

#### `CrossCritic` - Batch-Normalized Critic

**Name**: `CrossCritic`
**Responsibility**: Critic network with batch normalization for training stability
**State Schema**:
- channels: Sequence[int] - [Layer dimensions, default (128, 128, 128)]
- batch_norm_momentum: float - [Batch norm momentum, default 0.99]
- dtype: Optional[jnp.dtype] - [Computation dtype]
- param_dtype: jnp.dtype - [Parameter dtype, default float32]

**Public Interface**:
```python
def __call__(self, f_state, train):
    # Compute state value with batch normalization
    # Returns: Value estimate for current state
```

**Lifecycle**: 
- Creation: Instantiated with cross critic configuration
- Forward Pass: Computes normalized state values
- Training: Updated with batch statistics

**Dependencies**: 
- MLP for value computation
- Batch normalization for training stability
- Activation functions for non-linearity

**Usage Patterns**: 
- Called by RNNAgent when batch_norm=True
- Processes state features with normalization
- Outputs value for advantage computation

### Data Structures/Types

#### Model Configuration Schema

**Name**: `ModelArgs`
**Purpose**: Complete configuration for RNNAgent architecture
**Schema Definition**:

Required Fields:
- num_layers: int - [Number of transformer layers, must be positive]
- num_channels: int - [Feature dimension, must be positive]
- rnn_channels: int - [RNN hidden dimension, must be positive]
- rnn_type: Literal['lstm', 'gru', 'rwkv', 'none'] - [RNN architecture type]
- embedding_shape: Optional[Union[int, Tuple[int, int]]] - [Card embedding dimensions]

Optional Fields:
- use_history: bool - [Use action history, default True]
- card_mask: bool - [Mask padding cards, default False]
- film: bool - [Use FiLM conditioning, default True]
- noam: bool - [Use Noam architecture, default True]
- oppo_info: bool - [Use opponent information, default False]
- rnn_shortcut: bool - [Use RNN shortcuts, default False]
- batch_norm: bool - [Use batch normalization, default False]
- critic_width: int - [Critic network width, default 128]
- critic_depth: int - [Critic network depth, default 3]
- rwkv_head_size: int - [RWKV head size, default 32]
- version: int - [Environment version, default 2]

Computed/Derived Fields:
- total_params: int - [Total number of parameters]
- memory_usage: float - [Estimated memory usage in MB]

**Validation Rules**: 
- All dimensions must be positive integers
- RNN type must be valid option
- Embedding shape must be compatible with card data
- Version must match environment version

**Transformation Patterns**: 
- Configuration → Model instantiation
- Model → Parameter initialization
- Parameters → Training state

**Persistence**: 
- Stored in model checkpoints
- Logged in training metadata
- Versioned with model architecture

#### Game State Observation Schema

**Name**: `GameObservation`
**Purpose**: Complete game state representation for neural network processing
**Schema Definition**:

Required Fields:
- cards_: jnp.ndarray - [Card information, shape (batch, max_cards, card_features)]
- global_: jnp.ndarray - [Global game state, shape (batch, global_features)]
- actions_: jnp.ndarray - [Available actions, shape (batch, max_actions, action_features)]
- h_actions_: jnp.ndarray - [Action history, shape (batch, history_length, action_features)]
- mask_: jnp.ndarray - [Validity masks, shape (batch, max_cards, mask_features)]

Optional Fields:
- embeddings: Optional[jnp.ndarray] - [Card embeddings if available]
- metadata: Optional[dict] - [Additional game metadata]

**Validation Rules**: 
- All arrays must have consistent batch dimensions
- Card features must match encoder expectations
- Action features must match action encoder
- Masks must be boolean arrays

**Transformation Patterns**: 
- Raw environment observation → GameObservation
- GameObservation → Encoder input
- Encoder output → Actor/Critic input

**Persistence**: 
- Stored in training buffers
- Used for model inference
- Logged for debugging

#### RNN State Schema

**Name**: `RNNState`
**Purpose**: Recurrent neural network state for temporal modeling
**Schema Definition**:

LSTM State:
- h: jnp.ndarray - [Hidden state, shape (batch, rnn_channels)]
- c: jnp.ndarray - [Cell state, shape (batch, rnn_channels)]

GRU State:
- h: jnp.ndarray - [Hidden state, shape (batch, rnn_channels)]

RWKV State:
- k: jnp.ndarray - [Key state, shape (batch, num_heads * head_size)]
- v: jnp.ndarray - [Value state, shape (batch, num_heads * head_size * head_size)]

None State:
- None - [No recurrent state]

**Validation Rules**: 
- State dimensions must match model configuration
- Batch size must be consistent
- Data types must match model dtype

**Transformation Patterns**: 
- Initialization → Zero state
- Forward pass → Updated state
- Episode reset → Zero state

**Persistence**: 
- Stored in training buffers
- Used for model inference
- Reset on episode boundaries

### API Endpoints/Routes

#### Model Forward Pass API

**Endpoint**: `RNNAgent.__call__(x, rstate, done=None, switch_or_main=None, train=False)`
**Purpose**: Process game state through neural network for action selection and value estimation
**Authentication**: None - internal model call
**Request Schema**:

Input Parameters:
- x: dict - [Game observation dictionary]
- rstate: RNNState - [Current RNN state]
- done: Optional[jnp.ndarray] - [Episode termination flags]
- switch_or_main: Optional[jnp.ndarray] - [Player switching or main phase indicators]
- train: bool - [Whether in training mode]

**Response Schema**:

Success Response (Standard Mode):
- rstate: RNNState - [Updated RNN state]
- logits: jnp.ndarray - [Action logits for policy]
- value: jnp.ndarray - [State value estimate]
- valid: jnp.ndarray - [Validity indicators]

Success Response (Q-Head Mode):
- rstate: RNNState - [Updated RNN state]
- logits: jnp.ndarray - [Action logits for policy]
- valid: jnp.ndarray - [Validity indicators]

Error Responses:
- ValueError: Invalid input shapes or types
- TypeError: Incompatible RNN type or configuration
- RuntimeError: Memory or computation errors

**Business Logic**: 
- Encodes game state into features
- Processes through transformer layers
- Updates RNN state based on game phase
- Computes action probabilities and value estimates

**Side Effects**: 
- RNN state updates
- Feature computation
- Memory allocation for intermediate results

**Performance**: 
- Forward pass time: Milliseconds per batch
- Memory usage: Scales with batch size and model size
- GPU utilization: High during computation

#### RNN State Initialization API

**Endpoint**: `RNNAgent.init_rnn_state(batch_size)`
**Purpose**: Initialize RNN state for given batch size
**Authentication**: None - internal model call
**Request Schema**:

Input Parameters:
- batch_size: int - [Number of environments in batch]

**Response Schema**:

Success Response:
- rstate: RNNState - [Initialized RNN state appropriate for rnn_type]

Error Responses:
- ValueError: Invalid batch size
- TypeError: Incompatible RNN type

**Business Logic**: 
- Creates zero state for LSTM/GRU
- Initializes RWKV state dimensions
- Returns None for non-recurrent models

**Side Effects**: 
- Memory allocation for state arrays
- State initialization

**Performance**: 
- Initialization time: Microseconds
- Memory usage: Scales with batch size and RNN dimensions

### Service/Module Interfaces

#### Neural Network Agent Service Interface

**Service Name**: `NeuralAgentService`
**Domain Responsibility**: Provides neural network-based decision making for Yu-Gi-Oh! agents
**Public Interface**:

```python
class NeuralAgentService:
    # Core operations
    def create_agent(config: ModelArgs) -> RNNAgent:  # Create agent with configuration
    def forward_pass(agent: RNNAgent, obs: dict, rstate: RNNState) -> tuple:  # Process observation
    def init_state(agent: RNNAgent, batch_size: int) -> RNNState:  # Initialize RNN state
    
    # Training utilities
    def compute_loss(agent: RNNAgent, batch: dict) -> dict:  # Compute training loss
    def update_params(agent: RNNAgent, grads: dict) -> RNNAgent:  # Update parameters
    def save_checkpoint(agent: RNNAgent, path: str) -> None:  # Save model state
    def load_checkpoint(agent: RNNAgent, path: str) -> RNNAgent:  # Load model state
    
    # Inference utilities
    def sample_action(logits: jnp.ndarray, mask: jnp.ndarray) -> jnp.ndarray:  # Sample actions
    def compute_value(agent: RNNAgent, obs: dict) -> jnp.ndarray:  # Compute state values
    def get_action_probs(logits: jnp.ndarray, mask: jnp.ndarray) -> jnp.ndarray:  # Get probabilities
```

**Dependencies**: 
- flax.linen for neural network layers
- jax for computation
- ygoai.rl.jax.transformer for transformer components
- ygoai.rl.jax.modules for utility modules

**Configuration**:
```python
{
    "num_layers": 2,
    "num_channels": 128,
    "rnn_channels": 512,
    "rnn_type": "lstm",
    "film": True,
    "noam": True,
    "embedding_shape": (13473, 128),
    "dtype": "float32",
    "param_dtype": "float32"
}
```

**Error Handling**: 
- Invalid configuration throws ValueError
- Shape mismatches cause RuntimeError
- Memory issues trigger OOMError

**Monitoring**: 
- Model parameter count tracking
- Forward pass timing
- Memory usage monitoring

### Integration/External System Interfaces

#### JAX Computation Integration

**System Name**: `JAX`
**Purpose**: High-performance numerical computing for neural network operations
**Protocol**: JAX computation graph and JIT compilation
**Authentication**: None - local computation
**Request/Response Examples**:

JIT Compilation:
```python
@jax.jit
def forward_pass(params, obs, rstate):
    return agent.apply(params, obs, rstate)

# Compiled function for efficient execution
```

Gradient Computation:
```python
grad_fn = jax.grad(loss_fn)
gradients = grad_fn(params, batch)
```

**Error Handling**: 
- Compilation errors for incompatible operations
- Memory errors for large computations
- Shape errors for tensor mismatches

**Rate Limits**: 
- GPU memory constraints
- Compilation time for new functions
- Computation time for forward/backward passes

**Data Mapping**: 
- Model parameters → JAX arrays
- Observations → JAX tensors
- Gradients → JAX gradients

#### Flax Neural Network Integration

**System Name**: `Flax`
**Purpose**: Neural network library built on JAX
**Protocol**: Flax module system and parameter management
**Authentication**: None - local library
**Request/Response Examples**:

Module Definition:
```python
class RNNAgent(nn.Module):
    num_channels: int = 128
    
    @nn.compact
    def __call__(self, x):
        return self.encoder(x)
```

Parameter Management:
```python
params = agent.init(key, sample_input)
output = agent.apply(params, input_data)
```

**Error Handling**: 
- Module definition errors
- Parameter initialization failures
- Shape inference errors

**Rate Limits**: 
- Parameter count limits
- Memory usage constraints
- Compilation time for complex modules

**Data Mapping**: 
- Module definitions → Parameter structures
- Input data → Module outputs
- Parameters → Serialized checkpoints

### Business Logic/Workflow Abstractions

#### Neural Network Inference Workflow

**Workflow Name**: `NeuralAgentInference`
**Business Purpose**: Process game states through neural network for action selection
**Process Steps**:

```
1. State Encoding
   ├── Extract card information
   ├── Process global game state
   ├── Encode available actions
   ├── Create action history features
   └── Generate validity masks

2. Feature Processing
   ├── Card feature extraction via CardEncoder
   ├── Action feature extraction via ActionEncoder
   ├── Global state processing via GlobalEncoder
   └── Transformer layer processing

3. RNN State Management
   ├── Update RNN state based on game phase
   ├── Handle episode boundaries
   ├── Manage player switching
   └── Maintain temporal consistency

4. Policy and Value Computation
   ├── Actor network for action probabilities
   ├── Critic network for state values
   ├── FiLM conditioning if enabled
   └── Output processing and masking

5. Action Selection
   ├── Convert logits to probabilities
   ├── Apply validity masks
   ├── Sample actions from policy
   └── Return selected actions and values
```

**Decision Points**: 
- RNN type selection (LSTM, GRU, RWKV, none)
- FiLM conditioning usage
- Opponent information inclusion
- Batch normalization application

**Rollback/Compensation**: 
- State reset on episode boundaries
- Error handling for invalid states
- Fallback to default actions on failure

**Business Rules**: 
- State encoding follows environment specification
- RNN state updates respect game phase
- Action masking ensures valid selections
- Value estimates guide training

**State Transitions**: 
- Observation → Encoding → Processing → RNN → Actor/Critic → Output
- Episode boundary → State reset
- Player switch → State management
- Error → Fallback handling

## Dependencies Map

### Imports Analysis

Internal Dependencies:
- ygoai.rl.jax.transformer from ygoai.rl.jax - [Purpose: Transformer layer implementations]
- ygoai.rl.jax.modules from ygoai.rl.jax - [Purpose: Utility neural network modules]
- ygoai.rl.jax.rwkv from ygoai.rl.jax - [Purpose: RWKV attention implementation]

External Dependencies:
- flax.linen - [Purpose: Neural network layer definitions]
- jax - [Purpose: High-performance numerical computing]
- jax.numpy - [Purpose: Array operations and mathematical functions]
- numpy - [Purpose: Array utilities and type handling]

### Exports Analysis

What this directory exposes and why:
- RNNAgent - [Primary neural network agent for YGO reinforcement learning]
- ModelArgs - [Configuration for agent architecture]
- EncoderArgs - [Configuration for encoder components]
- RNDModel - [Random Network Distillation model for exploration]

## Integration Points

- **Called by**: scripts/cleanba.py for training, scripts/eval.py for evaluation
- **Calls**: ygoai.rl.jax.transformer for transformer layers, ygoai.rl.jax.modules for utilities
- **Data dependencies**: Game observations from ygoenv, card embeddings from ygoai.embed
- **Event producers**: Action logits and value estimates for training
- **Event consumers**: Training scripts for parameter updates

## Complexity Indicators

- **Cognitive load**: High - [Reasoning: Complex neural network architecture with multiple components]
- **Change frequency**: Medium - [How often agent architecture is modified for research]
- **Test coverage**: Low - [Estimated coverage of neural network testing]
- **Performance sensitivity**: High - [Whether performance is critical for training and inference]
- **Error impact**: High - [Blast radius if agent fails - training/inference stops] 